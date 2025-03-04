#include "pch.h"
#include "UpdateTriggerLastChecked.h"
#include "GetTriggerData.h"
#include "Render3DEventProvider.h"
#include "ModuleHook.h"
#include "MultilevelPointer.h"
#include "GetTriggerData.h"
#include "PointerDataStore.h"
#include "RuntimeExceptionHandler.h"
#include "IMakeOrGetCheat.h"
#include "SettingsStateAndEvents.h"
#include "MidhookContextInterpreter.h"
#include "Datum.h"
#include "GetPlayerDatum.h"
#include "GameTickEventHook.h"
#include "DynamicStructFactory.h"



template<GameState::Value mGame>
class UpdateTriggerLastCheckedImplByIndex : public TokenSharedRequestProvider
{
private:

	//injected services
	std::weak_ptr <GetTriggerData> getTriggerDataWeak;
	std::weak_ptr< GameTickEventHook> gameTickEventHookWeak;
	std::shared_ptr <GetPlayerDatum> getPlayerDatum;
	std::shared_ptr< RuntimeExceptionHandler> runtimeExceptions;
	std::shared_ptr< SettingsStateAndEvents> settings;


	// data
	static inline UpdateTriggerLastCheckedImplByIndex<mGame>* instance = nullptr;
	std::shared_ptr<ModuleInlineHook> updateTriggerCheckHook;
	std::shared_ptr<MultilevelPointer> pointInsideTriggerFunction;
	std::atomic_bool destructionGuard = false;



	virtual void updateService() override
	{
		PLOG_DEBUG << "UpdateTriggerLastCheckedImplByIndex serviceIsRequested ? " << (serviceIsRequested() ? "true" : "false");
		updateTriggerCheckHook->setWantsToBeAttached(serviceIsRequested());
	}



	static bool UpdateTriggerCheckHookFunction(uint32_t triggerIndex, Datum entityDatum)
	{
		try
		{
			ScopedAtomicBool lock(instance->destructionGuard);

			LOG_ONCE(PLOG_DEBUG << "UpdateTriggerCheckHookFunction running first time");

			bool wasSuccessfulCheck = instance->updateTriggerCheckHook->getInlineHook().call<bool, uint32_t, Datum>(triggerIndex, entityDatum);

			if (entityDatum != instance->getPlayerDatum->getPlayerDatum())
			{
				LOG_ONCE_CAPTURE(PLOG_DEBUG << "TriggerCheck checking non-player datum: " << ed, ed = entityDatum);
				LOG_ONCE_CAPTURE(PLOG_DEBUG << "with triggerIndex: " << ti, ti = triggerIndex);
				return wasSuccessfulCheck;
			}

			if (wasSuccessfulCheck)
			{
				LOG_ONCE_CAPTURE(PLOG_DEBUG << "successful player trigger check on trigger index: " << ti, ti = triggerIndex);
			}


			lockOrThrow(instance->getTriggerDataWeak, getTriggerData);

			auto filteredTriggerData = getTriggerData->getFilteredTriggers();



			for (auto& [triggerPointer, triggerData] : *filteredTriggerData.get())
			{
				if (triggerData.triggerIndex == triggerIndex)
				{
					if (triggerData.isBSPTrigger == false)
					{
						lockOrThrow(instance->gameTickEventHookWeak, gameTickEventHook);
						auto currentTick = gameTickEventHook->getCurrentGameTick();

						LOG_ONCE_CAPTURE(PLOG_DEBUG << "updating timeLastChecked of trigger with index: " << t, t = triggerIndex);
						triggerData.tickLastChecked = currentTick;
						triggerData.lastCheckSuccessful = wasSuccessfulCheck;
						triggerData.printedMessageForLastCheck = false;
					}

					break;
				}
			}

			return wasSuccessfulCheck;

		}
		catch (HCMRuntimeException ex)
		{
			instance->runtimeExceptions->handleMessage(ex);
			return false;
		}

	}


public:
	UpdateTriggerLastCheckedImplByIndex(GameState game, IDIContainer& dicon)
		:
		settings(dicon.Resolve<SettingsStateAndEvents>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
		getTriggerDataWeak(resolveDependentCheat(GetTriggerData)),
		getPlayerDatum(resolveDependentCheat(GetPlayerDatum)),
		gameTickEventHookWeak(resolveDependentCheat(GameTickEventHook))
	{

		instance = this;
		auto ptr = dicon.Resolve<PointerDataStore>().lock();
		auto updateTriggerLastCheckedFunction = ptr->getData < std::shared_ptr<MultilevelPointer>>(nameof(updateTriggerLastCheckedFunction), game);
		updateTriggerCheckHook = ModuleInlineHook::make(game.toModuleName(), updateTriggerLastCheckedFunction, UpdateTriggerCheckHookFunction);

		auto pointInsideTriggerFunction = ptr->getData < std::shared_ptr<MultilevelPointer>>(nameof(pointInsideTriggerFunction), game);

	}

	~UpdateTriggerLastCheckedImplByIndex()
	{

		if (destructionGuard)
		{
			destructionGuard.wait(true);
		}
		updateTriggerCheckHook->setWantsToBeAttached(false);
		instance = nullptr;
	}

};






	template<GameState::Value mGame>
	class UpdateTriggerLastCheckedImplEntityPointer : public TokenSharedRequestProvider
	{
	private:

		//injected services
		std::weak_ptr <GetTriggerData> getTriggerDataWeak;
		std::weak_ptr< GameTickEventHook> gameTickEventHookWeak;
		std::shared_ptr <GetPlayerDatum> getPlayerDatum;
		std::shared_ptr< RuntimeExceptionHandler> runtimeExceptions;
		std::shared_ptr< SettingsStateAndEvents> settings;


		// data
		static inline UpdateTriggerLastCheckedImplEntityPointer<mGame>* instance = nullptr;
		std::shared_ptr<ModuleInlineHook> updateTriggerCheckHook;
		std::atomic_bool destructionGuard = false;

		enum class entityTriggerInfoDataFields { Datum };
		std::shared_ptr<DynamicStruct<entityTriggerInfoDataFields>> entityTriggerInfoDataStruct;


		virtual void updateService() override
		{
			PLOG_DEBUG << "UpdateTriggerLastCheckedImplEntityPointer serviceIsRequested ? " << (serviceIsRequested() ? "true" : "false");
			updateTriggerCheckHook->setWantsToBeAttached(serviceIsRequested());
		}


		static bool UpdateTriggerCheckHookFunctionSig1(uint32_t triggerIndex, uintptr_t pEntityInfo)
		{
			try
			{
				ScopedAtomicBool lock(instance->destructionGuard);

				LOG_ONCE(PLOG_DEBUG << "UpdateTriggerCheckHookFunction running first time");

				bool wasSuccessfulCheck = instance->updateTriggerCheckHook->getInlineHook().fastcall<bool, uint32_t, uintptr_t>(triggerIndex, pEntityInfo);

				LOG_ONCE(PLOG_DEBUG << "inline call succeeded");

				instance->entityTriggerInfoDataStruct->currentBaseAddress = pEntityInfo;
				auto* pEntityDatum = instance->entityTriggerInfoDataStruct->field<Datum>(entityTriggerInfoDataFields::Datum);
				if (IsBadReadPtr(pEntityDatum, sizeof(Datum))) throw HCMRuntimeException(std::format("Bad entity datum read at {:X}", (uintptr_t)pEntityDatum));

				auto entityDatum = *pEntityDatum;

				if (entityDatum != instance->getPlayerDatum->getPlayerDatum())
				{
					LOG_ONCE_CAPTURE(PLOG_DEBUG << "TriggerCheck checking non-player datum: " << ed, ed = entityDatum);
					LOG_ONCE_CAPTURE(PLOG_DEBUG << "with triggerIndex: " << ti, ti = triggerIndex);
					return wasSuccessfulCheck;
				}

				if (wasSuccessfulCheck)
				{
					LOG_ONCE_CAPTURE(PLOG_DEBUG << "successful player trigger check on trigger index: " << ti, ti = triggerIndex);
				}


				lockOrThrow(instance->getTriggerDataWeak, getTriggerData);

				auto filteredTriggerData = getTriggerData->getFilteredTriggers();



				for (auto& [triggerPointer, triggerData] : *filteredTriggerData.get())
				{
					if (triggerData.triggerIndex == triggerIndex)
					{
						if (triggerData.isBSPTrigger == false)
						{
							lockOrThrow(instance->gameTickEventHookWeak, gameTickEventHook);
							auto currentTick = gameTickEventHook->getCurrentGameTick();

							LOG_ONCE_CAPTURE(PLOG_DEBUG << "updating timeLastChecked of trigger with index: " << t, t = triggerIndex);
							triggerData.tickLastChecked = currentTick;
							triggerData.lastCheckSuccessful = wasSuccessfulCheck;
							triggerData.printedMessageForLastCheck = false;
						}

						break;
					}
				}

				return wasSuccessfulCheck;

			}
			catch (HCMRuntimeException ex)
			{
				instance->runtimeExceptions->handleMessage(ex);
				return false;
			}

		}


		static bool UpdateTriggerCheckHookFunctionSig2(uint32_t triggerIndex, uint32_t unknown, uintptr_t pEntityInfo)
		{
			try
			{
				ScopedAtomicBool lock(instance->destructionGuard);

				LOG_ONCE(PLOG_DEBUG << "UpdateTriggerCheckHookFunction running first time");

				bool wasSuccessfulCheck = instance->updateTriggerCheckHook->getInlineHook().fastcall<bool, uint32_t, uint32_t, uintptr_t>(triggerIndex, unknown, pEntityInfo);

				LOG_ONCE(PLOG_DEBUG << "inline call succeeded");

				instance->entityTriggerInfoDataStruct->currentBaseAddress = pEntityInfo;
				auto* pEntityDatum = instance->entityTriggerInfoDataStruct->field<Datum>(entityTriggerInfoDataFields::Datum);
				if (IsBadReadPtr(pEntityDatum, sizeof(Datum))) throw HCMRuntimeException(std::format("Bad entity datum read at {:X}", (uintptr_t)pEntityDatum));

				auto entityDatum = *pEntityDatum;

				if (entityDatum != instance->getPlayerDatum->getPlayerDatum())
				{
					LOG_ONCE_CAPTURE(PLOG_DEBUG << "TriggerCheck checking non-player datum: " << ed, ed = entityDatum);
					LOG_ONCE_CAPTURE(PLOG_DEBUG << "with triggerIndex: " << ti, ti = triggerIndex);
					return wasSuccessfulCheck;
				}

				if (wasSuccessfulCheck)
				{
					LOG_ONCE_CAPTURE(PLOG_DEBUG << "successful player trigger check on trigger index: " << ti, ti = triggerIndex);
				}


				lockOrThrow(instance->getTriggerDataWeak, getTriggerData);

				auto filteredTriggerData = getTriggerData->getFilteredTriggers();



				for (auto& [triggerPointer, triggerData] : *filteredTriggerData.get())
				{
					if (triggerData.triggerIndex == triggerIndex)
					{
						if (triggerData.isBSPTrigger == false)
						{
							lockOrThrow(instance->gameTickEventHookWeak, gameTickEventHook);
							auto currentTick = gameTickEventHook->getCurrentGameTick();

							LOG_ONCE_CAPTURE(PLOG_DEBUG << "updating timeLastChecked of trigger with index: " << t, t = triggerIndex);
							triggerData.tickLastChecked = currentTick;
							triggerData.lastCheckSuccessful = wasSuccessfulCheck;
							triggerData.printedMessageForLastCheck = false;
						}

						break;
					}
				}

				return wasSuccessfulCheck;

			}
			catch (HCMRuntimeException ex)
			{
				instance->runtimeExceptions->handleMessage(ex);
				return false;
			}

		}

public:
	UpdateTriggerLastCheckedImplEntityPointer(GameState game, IDIContainer& dicon)
		:
		settings(dicon.Resolve<SettingsStateAndEvents>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
		getTriggerDataWeak(resolveDependentCheat(GetTriggerData)),
		getPlayerDatum(resolveDependentCheat(GetPlayerDatum)),
		gameTickEventHookWeak(resolveDependentCheat(GameTickEventHook))
	{

		instance = this;
		auto ptr = dicon.Resolve<PointerDataStore>().lock();
		auto updateTriggerLastCheckedFunction = ptr->getData < std::shared_ptr<MultilevelPointer>>(nameof(updateTriggerLastCheckedFunction), game);
		entityTriggerInfoDataStruct = DynamicStructFactory::make<entityTriggerInfoDataFields>(ptr, game);



		if constexpr (mGame == GameState::Value::Halo3 || mGame == GameState::Value::Halo3ODST)
			updateTriggerCheckHook = ModuleInlineHook::make(game.toModuleName(), updateTriggerLastCheckedFunction, UpdateTriggerCheckHookFunctionSig1);
		else if constexpr (mGame == GameState::Value::HaloReach || mGame == GameState::Value::Halo4)
			updateTriggerCheckHook = ModuleInlineHook::make(game.toModuleName(), updateTriggerLastCheckedFunction, UpdateTriggerCheckHookFunctionSig2);


	}

	~UpdateTriggerLastCheckedImplEntityPointer()
	{
		
		if (destructionGuard)
		{
			destructionGuard.wait(true);
		}
		updateTriggerCheckHook->setWantsToBeAttached(false);
		instance = nullptr;
	}


};



UpdateTriggerLastChecked::UpdateTriggerLastChecked(GameState gameImpl, IDIContainer& dicon)
{
	switch (gameImpl)
	{
	case GameState::Value::Halo1:
		pimpl = std::make_shared<UpdateTriggerLastCheckedImplByIndex<GameState::Value::Halo1>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo2:
		pimpl = std::make_shared<UpdateTriggerLastCheckedImplByIndex<GameState::Value::Halo2>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo3:
		pimpl = std::make_shared<UpdateTriggerLastCheckedImplEntityPointer<GameState::Value::Halo3>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo3ODST:
		pimpl = std::make_shared<UpdateTriggerLastCheckedImplEntityPointer<GameState::Value::Halo3ODST>>(gameImpl, dicon);
		break;

	case GameState::Value::HaloReach:
		pimpl = std::make_shared<UpdateTriggerLastCheckedImplEntityPointer<GameState::Value::HaloReach>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo4:
		pimpl = std::make_shared<UpdateTriggerLastCheckedImplEntityPointer<GameState::Value::Halo4>>(gameImpl, dicon);
		break;
	default:
		throw HCMInitException("not impl yet");
	}
}

UpdateTriggerLastChecked::~UpdateTriggerLastChecked() = default;