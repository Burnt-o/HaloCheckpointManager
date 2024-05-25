#include "pch.h"
#include "EntityPhysicsSimulator.h"
#include "SettingsStateAndEvents.h"
#include "IMakeOrGetCheat.h"
#include <Random>
#include "ModuleHook.h"
#include "PointerDataStore.h"
#include "BumpControlDefs.h"
#include "MidhookFlagInterpreter.h"
//#define BLOCK_FOR_KEYPRESS

class EntityPhysicsSimulator::EntityPhysicsSimulatorImpl {
private:
	static inline EntityPhysicsSimulatorImpl* instance = nullptr;

	// this physics function is specific to collisions with level geometry
	std::shared_ptr<ModuleInlineHook> entityPhysicsTickFunctionPointerHook;
	typedef bool(__fastcall* EntityPhysicsTick)(Datum);

	// cached data
	EntityPhysicsTick entityPhysicsTickFunction;





	std::shared_ptr<ModuleMidHook> correctThreadContextMidhook; // EntityPhysicsTick function needs to be run from the correct thread context that the game usually uses, as it uses Thread Local Storage to store data about anniversary-collision.
	std::shared_ptr<ModulePatch> removeTrap1Patch; // A trap that gets hit for some reason when calling EntityPhysicsTick multiples within a regular tick
	std::shared_ptr<ModulePatch> removeTrap2Patch; // ditto

#ifndef SHIELD_BUMPS
	std::shared_ptr<ModulePatch> psuedoTickPerformancePatch7CrashFix; // fixes crash associated with psuedoTickPerformancePatch7
#endif
	static inline Datum shieldDatum = Datum();

#ifdef SHIELD_BUMPS
	// instead of testing all scenery objects for collision, only test the shield we're bumping against (shieldDatum from bumpControlDefs). roughly 10% perf increase.

	static void onTestSceneryObjectCollisionMidhookFunction(SafetyHookContext& ctx)
	{
		if (((Datum)ctx.r11).index != shieldDatum.index) // if object isn't our shield
		{
			// set the zero flag to true (forcing a jump that skips collision check call)
			instance->testSceneryObjectCollisionMidhookFlagInterpreter.setFlag(ctx);
		}
		else
		{
			LOG_ONCE(PLOG_DEBUG << "testing shield object collision");
		}
	}


	MidhookFlagInterpreter testSceneryObjectCollisionMidhookFlagInterpreter{ MidhookFlagInterpreter::RFlag::zero, true };
	std::shared_ptr<ModuleMidHook> testSceneryObjectCollisionMidhook;
#endif

	// improve performance by nopping some "useless" game function calls
	std::vector<std::shared_ptr<ModulePatch>> psuedoTickPerformancePatches;
	//std::shared_ptr<ModulePatch> pseudoTickRenderingPerformancePatch;


	std::atomic_bool correctThreadContextMidhookCleanupLock = false; // so we can cleanup the correctThreadContextMidhook from outside the context of that thread
	std::atomic_bool simulationThreadRunning = false; // destruction guard

	static void onCorrectThreadContextMidhookFunction(SafetyHookContext& ctx)
	{
		if (instance != nullptr && instance->queuedSimulationThreads.empty() == false)
		{
			ScopedAtomicBool destructionLock(instance->simulationThreadRunning);

			while (instance->queuedSimulationThreads.empty() == false)
			{
				auto* threadToBegin = instance->queuedSimulationThreads.back();
				instance->queuedSimulationThreads.pop_back();

				//uintptr_t temp;
				//instance->importantPhysicsCounter->resolve(&temp);
				//instance->importantPhysicsCounterCached = (byte*)temp;
				//instance->importantPhysicsFlagCached = (byte*)(temp + 2);

				instance->removeTrap1Patch->setWantsToBeAttached(true);
				instance->removeTrap2Patch->setWantsToBeAttached(true);
#ifndef SHIELD_BUMPS
				instance->psuedoTickPerformancePatch7CrashFix->setWantsToBeAttached(true);
#else
				instance->testSceneryObjectCollisionMidhook->setWantsToBeAttached(true);
#endif
				for (auto& patch : instance->psuedoTickPerformancePatches)
				{
					patch->setWantsToBeAttached(true);
				}
				//instance->pseudoTickRenderingPerformancePatch->setWantsToBeAttached(true);
				instance->entityPhysicsTickFunction = instance->entityPhysicsTickFunctionPointerHook->getInlineHook().original<EntityPhysicsTick>();

				// this call will block until the called thread finishes!
				(*threadToBegin)(advancePhysicsTick);

			}
			
			// cleanup/disable hooks
			for (auto& patch : instance->psuedoTickPerformancePatches)
			{
				patch->setWantsToBeAttached(false);
			}
#ifndef SHIELD_BUMPS
			instance->psuedoTickPerformancePatch7CrashFix->setWantsToBeAttached(false);
#else
			instance->testSceneryObjectCollisionMidhook->setWantsToBeAttached(false);
#endif
			instance->entityPhysicsTickFunctionPointerHook->setWantsToBeAttached(false);

			instance->removeTrap1Patch->setWantsToBeAttached(false);
			instance->removeTrap2Patch->setWantsToBeAttached(false);
			//instance->pseudoTickRenderingPerformancePatch->setWantsToBeAttached(false);

			// we want to disable correctThreadContextMidhook too but doing that from within the hook function would cause a crash as the func tries to go back to a trampoline that doesn't exist.
			// instead we hand off that job to a seperate thread using an atomic bool to ensure this thread has finished first before the hook is disabled.
			ScopedAtomicBool lockCleanup(instance->correctThreadContextMidhookCleanupLock);
			std::thread cleanupThread = std::thread([cleanupfunc = cleanupCorrectThreadContextHook]() { cleanupfunc(); });
			cleanupThread.detach();
		}
		
	}

	static void cleanupCorrectThreadContextHook()
	{
		if (instance->correctThreadContextMidhookCleanupLock)
		{
			instance->correctThreadContextMidhookCleanupLock.wait(true);
		}
		Sleep(10);
		instance->correctThreadContextMidhook->setWantsToBeAttached(false); 
	}

	static void advancePhysicsTick(Datum entityDatum, int count)
	{
		for (int i = 0; i < count; i++)
		{
			instance->entityPhysicsTickFunction(entityDatum);
		}
	}


	static bool __fastcall entityPhysicsTickFunctionHook(Datum entityDatum)
	{
		return instance->entityPhysicsTickFunction(entityDatum);
	}



	std::vector<SimulationThread*> queuedSimulationThreads;


public:

	EntityPhysicsSimulatorImpl(GameState game, IDIContainer& dicon)

	{
		instance = this;
		auto ptr = dicon.Resolve<PointerDataStore>().lock();

		auto entityPhysicsTickFunctionPointer = ptr->getData < std::shared_ptr<MultilevelPointer>>(nameof(entityPhysicsTickFunctionPointer), game);
		entityPhysicsTickFunctionPointerHook = ModuleInlineHook::make(game.toModuleName(), entityPhysicsTickFunctionPointer, entityPhysicsTickFunctionHook);


		auto correctThreadContextMidhookFunction = ptr->getData < std::shared_ptr<MultilevelPointer>>(nameof(correctThreadContextMidhookFunction), game);
		correctThreadContextMidhook = ModuleMidHook::make(game.toModuleName(), correctThreadContextMidhookFunction, onCorrectThreadContextMidhookFunction);

		auto removeTrap1PatchFunction = ptr->getData < std::shared_ptr<MultilevelPointer>>(nameof(removeTrap1PatchFunction), game);
		auto removeTrap1PatchCode = ptr->getVectorData<byte>(nameof(removeTrap1PatchCode), game);
		removeTrap1Patch = ModulePatch::make(game.toModuleName(), removeTrap1PatchFunction, *removeTrap1PatchCode.get());

		auto removeTrap2PatchFunction = ptr->getData < std::shared_ptr<MultilevelPointer>>(nameof(removeTrap2PatchFunction), game);
		auto removeTrap2PatchCode = ptr->getVectorData<byte>(nameof(removeTrap2PatchCode), game);
		removeTrap2Patch = ModulePatch::make(game.toModuleName(), removeTrap2PatchFunction, *removeTrap2PatchCode.get());

		auto psuedoTickPerformancePatch1Function = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(psuedoTickPerformancePatch1), game);
		psuedoTickPerformancePatches.push_back(ModulePatch::makeNOPCall(game.toModuleName(), psuedoTickPerformancePatch1Function));

#ifndef SHIELD_BUMPS
		// this code is responsible for object collision with non-level-geo stuff like scenery, deployed shields etc
		// 10% performance improvement if you don't care about that though.
		auto psuedoTickPerformancePatch2Function = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(psuedoTickPerformancePatch2), game);
		psuedoTickPerformancePatches.push_back(ModulePatch::makeNOPCall(game.toModuleName(), psuedoTickPerformancePatch2Function));

#else
		// midhook in the same area of code to make it ONLY test collision with the shield object we care about, instead of all objects
		auto testSceneryObjectCollisionMidhookFunction = ptr->getData < std::shared_ptr<MultilevelPointer>>(nameof(testSceneryObjectCollisionMidhookFunction), game);
		testSceneryObjectCollisionMidhook = ModuleMidHook::make(game.toModuleName(), testSceneryObjectCollisionMidhookFunction, onTestSceneryObjectCollisionMidhookFunction);

#endif

		auto psuedoTickPerformancePatch3Function = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(psuedoTickPerformancePatch3), game);
		psuedoTickPerformancePatches.push_back(ModulePatch::makeNOPCall(game.toModuleName(), psuedoTickPerformancePatch3Function));

		auto psuedoTickPerformancePatch4Function = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(psuedoTickPerformancePatch4), game);
		psuedoTickPerformancePatches.push_back(ModulePatch::makeNOPCall(game.toModuleName(), psuedoTickPerformancePatch4Function));

		auto psuedoTickPerformancePatch5Function = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(psuedoTickPerformancePatch5), game);
		psuedoTickPerformancePatches.push_back(ModulePatch::makeNOPCall(game.toModuleName(), psuedoTickPerformancePatch5Function));

		auto psuedoTickPerformancePatch6Function = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(psuedoTickPerformancePatch6), game);
		psuedoTickPerformancePatches.push_back(ModulePatch::makeNOPCall(game.toModuleName(), psuedoTickPerformancePatch6Function));

#ifndef SHIELD_BUMPS
		// not sure why but this performance patch also breaks shield bumps lol
		auto psuedoTickPerformancePatch7Function = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(psuedoTickPerformancePatch7), game);
		psuedoTickPerformancePatches.push_back(ModulePatch::makeNOPCall(game.toModuleName(), psuedoTickPerformancePatch7Function));
#endif

#ifndef CONSIDER_ANNIVERSARY_GEO
		// the following code is responsible for handling anniversay-geometry collision. We don't need that for shield bumps.
		auto psuedoTickPerformancePatch8Function = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(psuedoTickPerformancePatch8), game);
		psuedoTickPerformancePatches.push_back(ModulePatch::makeNOPCall(game.toModuleName(), psuedoTickPerformancePatch8Function));

#endif

#ifndef SHIELD_BUMPS
		auto psuedoTickPerformancePatch7CrashFixFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(psuedoTickPerformancePatch7CrashFixFunction), game);
		auto psuedoTickPerformancePatch7CrashFixCode = ptr->getVectorData<byte>(nameof(psuedoTickPerformancePatch7CrashFixCode), game);
		psuedoTickPerformancePatch7CrashFix = ModulePatch::make(game.toModuleName(), psuedoTickPerformancePatch7CrashFixFunction, *psuedoTickPerformancePatch7CrashFixCode.get());
#endif


		// one of hte player collision check calls but doesn't seem to be neccessary?
		// TODO: rerun verification
		auto psuedoTickPerformancePatch9Function = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(psuedoTickPerformancePatch9), game);
		psuedoTickPerformancePatches.push_back(ModulePatch::makeNOPCall(game.toModuleName(), psuedoTickPerformancePatch9Function));

		// related to movement input but doesn't seem necessary
		auto psuedoTickPerformancePatch10Function = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(psuedoTickPerformancePatch10), game);
		psuedoTickPerformancePatches.push_back(ModulePatch::makeNOPCall(game.toModuleName(), psuedoTickPerformancePatch10Function));



		//// something in physics but doesn't seem necessaru
		//auto psuedoTickPerformancePatch11Function = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(psuedoTickPerformancePatch11), game);
		//psuedoTickPerformancePatches.push_back(ModulePatch::makeNOPCall(game.toModuleName(), psuedoTickPerformancePatch11Function));

		//// something in physics but doesn't seem necessary (same area as 11)
		//auto psuedoTickPerformancePatch12Function = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(psuedoTickPerformancePatch12), game);
		//psuedoTickPerformancePatches.push_back(ModulePatch::makeNOPCall(game.toModuleName(), psuedoTickPerformancePatch12Function));

		// this code seems to be responsible for setting where in the bsp the camera should be. doesn't matter if we don't care about rendering.
		//auto pseudoTickRenderingPerformancePatchFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(pseudoTickRenderingPerformancePatch), game);
		//auto pseudoTickRenderingPerformancePatchCode = ptr->getVectorData<byte>(nameof(pseudoTickRenderingPerformancePatchCode), game);
		//pseudoTickRenderingPerformancePatch = ModulePatch::make(game.toModuleName(), pseudoTickRenderingPerformancePatchFunction, *pseudoTickRenderingPerformancePatchCode.get());


				// function responsible checking for and applying fall damage to player.
		auto psuedoTickPerformancePatch13Function = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(psuedoTickPerformancePatch13), game);
		psuedoTickPerformancePatches.push_back(ModulePatch::makeNOPCall(game.toModuleName(), psuedoTickPerformancePatch13Function));

	}

	~EntityPhysicsSimulatorImpl()
	{
		if (simulationThreadRunning)
		{
			simulationThreadRunning.wait(true);
		}
		instance = nullptr;
	}

	void beginSimulation(SimulationThread* beginThread)
	{
		queuedSimulationThreads.push_back(beginThread);
		entityPhysicsTickFunctionPointerHook->setWantsToBeAttached(true);
		removeTrap1Patch->setWantsToBeAttached(true);
		removeTrap2Patch->setWantsToBeAttached(true);
		correctThreadContextMidhook->setWantsToBeAttached(true);
	}

	static void setShieldDatum(Datum datum)
	{
		shieldDatum = datum;
	}


};








EntityPhysicsSimulator::EntityPhysicsSimulator(GameState gameImpl, IDIContainer& dicon)
{
	pimpl = std::make_unique< EntityPhysicsSimulatorImpl>(gameImpl, dicon);
}

EntityPhysicsSimulator::~EntityPhysicsSimulator() = default;

void EntityPhysicsSimulator::beginSimulation(SimulationThread* beginThread)
{
	pimpl->beginSimulation(beginThread);
}

void EntityPhysicsSimulator::setShieldDatum(Datum shieldDatum)
{
	return EntityPhysicsSimulatorImpl::setShieldDatum(shieldDatum);
}