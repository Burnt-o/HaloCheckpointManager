#include "pch.h"
#include "OptionalCheatManager.h"
#include "OptionalCheatEnum.h"
#include "IOptionalCheat.h"

#include "TogglePause.h"
#include "ToggleFreeCursor.h"
#include "ToggleBlockInput.h"

#include "ForceCheckpoint.h"
#include "ForceRevert.h"
#include "ForceDoubleRevert.h"

#include "InjectCheckpoint.h"
#include "InjectCore.h"
#include "DumpCheckpoint.h"
#include "DumpCore.h"
#include "ForceCoreSave.h"
#include "ForceCoreLoad.h"
#include "Speedhack.h"
#include "Invulnerability.h"
#include "GetPlayerDatum.h"
#include "GetObjectAddress.h"
#include "AIFreeze.h"
#include "CommandConsoleManager.h"
#include "GetCurrentDifficulty.h"
#include "IgnoreCheckpointChecksum.h"
#include "AdvanceTicks.h"
#include "ForceFutureCheckpoint.h"
#include "GameTickEventHook.h"
#include "ForceTeleport.h"
#include "ForceLaunch.h"
#include "GetPlayerViewAngle.h"
#include "GetObjectPhysics.h"
#include "GetHavokComponent.h"
#include "GetHavokAnchorPoint.h"
#include "UnfreezeObjectPhysics.h"
#include "GetObjectAddressCLI.h"
#include "Medusa.h"
#include "NaturalCheckpointDisable.h"
#include "InfiniteAmmo.h"
#include "BottomlessClip.h"
#include "DisplayPlayerInfo.h"
#include "GetObjectHealth.h"
#include "GetAggroData.h"
#include "GetTagName.h"
#include "GetObjectTagName.h"
#include "GetBipedsVehicleDatum.h"
#include "GetNextObjectDatum.h"
#include "FreeCamera.h"
#include "GetGameCameraData.h"
#include "UpdateGameCameraData.h"
#include "UserCameraInputReader.h"
#include "ThirdPersonRendering.h"
#include "BlockPlayerCharacterInput.h"
#include "GetCurrentRNG.h"
#include "EditPlayerViewAngle.h"
#include "EditPlayerViewAngleID.h"
#include "SwitchBSP.h"
#include "GetCurrentBSP.h"
#include "DisableScreenEffects.h"
#include "HideHUD.h"
#include "OBSBypassCheck.h"
#include "SetPlayerHealth.h"
#include "Waypoint3D.h"
#include "ViewAngle3D.h"
#include "Render3DEventProvider.h"
#include "MeasurePlayerDistanceToObject.h"
#include "SkullToggler.h"
#include "TriggerOverlay.h"
#include "GetTriggerData.h"
#include "UpdateTriggerLastChecked.h"
#include "ObjectTableRange.h"
#include "HideWatermarkCheck.h"
#include "PlayerPositionToClipboard.h"
#include "TriggerFilter.h"
#include "GetPlayerTriggerPosition.h"
#include "TrackTriggerEnterExit.h"
#include "ShieldInputPrinter.h"
#include "RevertEventHook.h"
#include "SensDriftOverlay.h"
#include "TriggerFilterModalDialogManager.h"
#include "GetScenarioAddress.h"
#include "GetDebugString.h"
#include "TagBlockReader.h"
#include "TagTableRange.h"
#include "GetSoftCeilingData.h"
#include "SoftCeilingOverlay.h"
#include "GetCurrentBSPSet.h"
#include "SwitchBSPSet.h"
#include "GetCurrentZoneSet.h"
#include "BSPChangeHookEvent.h"
#include "ZoneSetChangeHookEvent.h"
#include "BSPSetChangeHookEvent.h"
#include "GetActiveStructureDesignTags.h"
#include "TagReferenceReader.h"
#include "HaloScriptOutputHookEvent.h"
#include "DisableBarriers.h"
#include "GetPlayerDatumPresenter.h"
#include "FreeCameraFOVOverride.h"
#include "GetAbilityData.h"
#include "AbilityMeterOverlay.h"
#include "GetTagAddressPresenter.h"
#include "SoundClassGain.h"
#include "ChangeOOBBackground.h"
#include "DisableFog.h"
#include "GameEngineFunctions.h"
#include "GameEngineDetail.h"
#include "ForceMissionRestart.h"


#include "DIContainer.h"
#include "map.h"
#include <boost\preprocessor.hpp>
#include "DirPathContainer.h"
#include "IMakeOrGetCheat.h"




typedef std::map<std::pair<GameState, OptionalCheatEnum>, std::shared_ptr<IOptionalCheat>> CheatCollection;

class OptionalCheatManager::OptionalCheatStore 
{
public:

private:

	// Cheats that depend on other cheats will need to access this to check if they're already initialised before attempting to init it themselves.
	// This collection is the central owner keeping all the optional cheats alive until HCM shuts down. 
		// Once the OptionalCheatManager goes out of scope in App.h, the OptionalCheatStore destructor will be called, this cheatCollection will be reset, and thus all the IOptionalCheats will in turn have their destructors called etc etc
	std::shared_ptr<CheatCollection> cheatCollection = std::make_shared<CheatCollection>();
	DIContainer<IMakeOrGetCheat, SettingsStateAndEvents, PointerDataStore, IGetMCCVersion, IMCCStateHook, ISharedMemory, IMessagesGUI, RuntimeExceptionHandler, DirPathContainer, ModalDialogRenderer, ControlServiceContainer, RenderEvent, DirectXRenderEvent, HotkeyDefinitions> dicon;


public:
	OptionalCheatStore(std::shared_ptr<IMakeOrGetCheat> cheatConstructor, 
		std::shared_ptr<SettingsStateAndEvents> settings,
		std::shared_ptr<PointerDataStore> ptr,
		std::shared_ptr<IGetMCCVersion> ver,
		std::shared_ptr<IMCCStateHook> mccStateHook,
		std::shared_ptr<ISharedMemory> sharedMem,
		std::shared_ptr<IMessagesGUI> mes,
		std::shared_ptr<RuntimeExceptionHandler> exp,
		std::string dirPath,
		std::shared_ptr<ModalDialogRenderer> modal,
		std::shared_ptr<ControlServiceContainer> control,
		std::shared_ptr<RenderEvent> overlayRenderEvent,
		std::shared_ptr<DirectXRenderEvent> foregroundDirectXRenderEvent,
		std::shared_ptr<HotkeyDefinitions> hotkeyDefinitions)
		:
		// Create a Dependency-Injection container with the dependencies that the cheats will need.
		// Remember: you need to register types as the base interface the optionalCheats will want to resolve
		dicon(cheatConstructor, settings, ptr, ver, mccStateHook, sharedMem, mes, exp, std::make_shared<DirPathContainer>(dirPath), modal, control, overlayRenderEvent, foregroundDirectXRenderEvent, hotkeyDefinitions)
	{ 
		
	}

	~OptionalCheatStore()
	{
		PLOG_DEBUG << "~OptionalCheatStore";
		// important: the cheat collection must be destroyed before the dicon, so the cheat destructors are still guarenteed access to their services

		for (auto& [info, cheat] : *cheatCollection.get())
		{
			PLOG_DEBUG << "Releasing reference to: " << cheat->getName();
			cheat.reset();
		}



		cheatCollection.reset();
		PLOG_DEBUG << "finished clearing cheat collection";
		// now the only pointers to the optionalCheats are those that point inwardly rats' nest style
	}

	friend class OptionalCheatConstructor;

};

class OptionalCheatConstructor : public IMakeOrGetCheat
{

private:
	// weak reference to the stores cheatCollection while we construct cheats. We will reset this when we're done.
	std::shared_ptr<CheatCollection> cheatCollection;
	std::mutex cheatCollectionMutex; // so multiple threads don't access the map simultaneously
	std::mutex cheatCollectionMutex2; // so multiple threads don't access the map simultaneously
public:
	OptionalCheatConstructor()
	{

	}

	// Loop over each cheat-game combo in requiredServices, pushing them into our cheatCollection as we make them (or telling info about it if failed construction).
	// Needs to be split away from constructor so we can use a OptionalCheatConstructor weak_ptr to OptionalCheatStore.
	void createCheats(std::weak_ptr<OptionalCheatManager::OptionalCheatStore> cheatStore, std::shared_ptr<IGUIRequiredServices> reqSer, std::shared_ptr<OptionalCheatInfo> info)
	{
		if (cheatStore.expired()) throw HCMInitException("Bad cheatStore weak ptr");
		cheatCollection = cheatStore.lock()->cheatCollection;
		PLOG_DEBUG << "Looping over required services";

		
		// Create cheats on multiple threads. Only one thread will access the cheatCollection at a time, but this way exceptions won't block execution of the next getOrMakeCheat
		std::vector<std::thread> createCheatThreads;
		
		for (const std::pair<GameState, OptionalCheatEnum>& gameCheatPair : reqSer->getAllRequiredServices())
		{
			auto& th = createCheatThreads.emplace_back(std::thread([gameCheatPair, cheatStore,info, this]() {
				try
				{
					std::lock_guard<std::mutex> lock(cheatCollectionMutex2);
					// Try to create the cheat. 
					getOrMakeCheat(gameCheatPair, cheatStore.lock()->dicon);
					info->setInfo(gameCheatPair, {});
				}
				catch (HCMInitException ex)
				{
					// Opdate OptionalCheatInfoManager with cheat construction failure. GUIElementManager will check this later to know what GUIElements will work or not
					info->setInfo(gameCheatPair, { ex });
				}
				catch (std::bad_weak_ptr ex)
				{
					// Cheat constructors ought to be catching bad_weak_ptr on their own but juuuuust in case
					HCMInitException converted(std::format("std::bad_weak_ptr exception! {}", ex.what()));
					info->setInfo(gameCheatPair, { converted });
				}
				}));
			

		}

		// Wait for all threads to finish
		PLOG_DEBUG << "Blocking until createCheatThreads finish execution";
		for (auto& th : createCheatThreads)
		{
			if (th.joinable())
			{
				th.join();
				PLOG_VERBOSE << "joining thread";
			}
				
		}
		PLOG_DEBUG << "createCheatThreads finished execution";

		// Our shared ref to the cheatCollection is no longer required. The cheatCollection will continue to live over in OptionalCheatStore
		cheatCollection.reset();

	}

	// Very likely to throw HCMInitExceptions so be ready to catch them.
	// Called in OptionalCheatConstructor createCheats for loop, AND via dicon by cheats that depend on other cheats (nested cheat construction).
	std::shared_ptr< IOptionalCheat> getOrMakeCheat(const std::pair<GameState, OptionalCheatEnum>& gameCheatPair, IDIContainer& dicon);


};



OptionalCheatManager::OptionalCheatManager(std::shared_ptr<IGUIRequiredServices> reqSer, std::shared_ptr<OptionalCheatInfo> info,
	/* rest is cheat construction stuff that will get stuffed into a DIContainer later*/
	std::shared_ptr<SettingsStateAndEvents> settings,
	std::shared_ptr<PointerDataStore> ptr, 
	std::shared_ptr<IGetMCCVersion> ver, 
	std::shared_ptr<IMCCStateHook> mccStateHook, 
	std::shared_ptr<ISharedMemory> sharedMem, 
	std::shared_ptr<IMessagesGUI> mes, 
	std::shared_ptr<RuntimeExceptionHandler> exp, 
	std::string dirPath, 
	std::shared_ptr<ModalDialogRenderer> modal,
	std::shared_ptr<ControlServiceContainer> control,
	std::shared_ptr<RenderEvent> overlayRenderEvent,
	std::shared_ptr<DirectXRenderEvent> foregroundDirectXRenderEvent,
	std::shared_ptr<HotkeyDefinitions> hotkeyDefinitions)
	: constructorPimpl(std::make_shared<OptionalCheatConstructor>())

{
	storePimpl = std::make_shared<OptionalCheatStore>(constructorPimpl, settings, ptr, ver, mccStateHook, sharedMem, mes, exp, dirPath, modal, control, overlayRenderEvent, foregroundDirectXRenderEvent, hotkeyDefinitions);

	// Ah yes a cyclic dependency? But actually the storePimpl will go in as a weak ptr, and it doesn't keep it after the method finishes anyway
	constructorPimpl->createCheats(storePimpl, reqSer, info);
}


OptionalCheatManager::~OptionalCheatManager() = default;


std::shared_ptr< IOptionalCheat> OptionalCheatConstructor::getOrMakeCheat(const std::pair<GameState, OptionalCheatEnum>& gameCheatPair, IDIContainer& dicon)
{


	std::shared_ptr<IOptionalCheat> outVal;
	PLOG_VERBOSE << "Creating cheat: " << gameCheatPair.first << "::" << magic_enum::enum_name(gameCheatPair.second);

	// Macro shenanigans to make a switch case for every OptionalCheatEnum to create associated IOptionalCheat class.
	// This macro is why it's crucial the OptionalCheatEnum shares the exact same name as the associated class.
	// The cheatCollectionMutex ensures the cheatCollection is only accessed by one thread at a time.
#define _PPSTUFF_MAKECASE1(_var)																\
case OptionalCheatEnum::_var:																	\
	cheatCollectionMutex.lock();																\
	if(!cheatCollection->contains(gameCheatPair))												\
	{																							\
	cheatCollectionMutex.unlock();																\
	std::shared_ptr<IOptionalCheat> cheat = std::make_shared<_var>(gameCheatPair.first, dicon); \
	cheatCollectionMutex.lock();																\
	cheatCollection->insert(std::make_pair(gameCheatPair, cheat));								\
	}																							\
	cheatCollectionMutex.unlock();																\
	outVal = cheatCollection->at(gameCheatPair);												\
	return outVal;	

	// Boost macro shenanigans so we can parse every optionalCheatEnum at once
#define _PPSTUFF_MAKECASE2(r, d, _var)  _PPSTUFF_MAKECASE1(_var) 
#define _PPSTUFF_MAKECASE_SEQ(vseq) _PPSTUFF_MAKECASE1(BOOST_PP_SEQ_HEAD(vseq)) \
        BOOST_PP_SEQ_FOR_EACH(_PPSTUFF_MAKECASE2,,BOOST_PP_SEQ_TAIL(vseq)) 
#define MAKECASE(...) _PPSTUFF_MAKECASE_SEQ(BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

	switch (gameCheatPair.second)
	{
		MAKECASE(ALLOPTIONALCHEATS1);
		MAKECASE(ALLOPTIONALCHEATS2);
		/* Expands to:
		 
		 case OptionalCheatEnum::ForceCheckpoint: 
		 cheatCollectionMutex.lock();
		 if(!cheatCollection->contains(gameCheatPair))
		 { 
		 cheatCollectionMutex.unlock();
		 std::shared_ptr<IOptionalCheat> cheat = std::make_shared<ForceCheckpoint>(gameCheatPair.first, dicon);
		 cheatCollectionMutex.lock();
		 cheatCollection->at(gameCheatPair) = cheat;
		 }
		 cheatCollectionMutex.unlock();
		 outVal = cheatCollection->at(gameCheatPair);
		 return outVal;
		 

		 .. for every OptionalCheatEnum value */
	default:
		throw HCMInitException("OptionalCheatConstructor::getOrMakeCheat recieved invalid OptionalCheatEnum (did you forget to add an identically named class for your enum entry?)");
	}
}
