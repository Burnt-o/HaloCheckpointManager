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

template<GameState::Value mGame>
class UpdateTriggerLastCheckedImpl : public UpdateTriggerLastCheckedUntemplated
{
private:

	//injected services
	std::weak_ptr <GetTriggerData> getTriggerDataWeak;
	std::shared_ptr <GetPlayerDatum> getPlayerDatum;
	std::shared_ptr< RuntimeExceptionHandler> runtimeExceptions;
	std::shared_ptr< SettingsStateAndEvents> settings;


	// data
	static inline UpdateTriggerLastCheckedImpl<mGame>* instance = nullptr;
	std::shared_ptr<ModuleInlineHook> updateTriggerCheckHook;
	std::atomic_bool destructionGuard = false;

	// callbacks
	ScopedCallback<ToggleEvent> triggerOverlayToggleCallback;
	ScopedCallback<ToggleEvent> triggerOverlayCheckFlashToggleCallback;

	void onSettingChanged()
	{
		try
		{
			updateTriggerCheckHook->setWantsToBeAttached(settings->triggerOverlayToggle->GetValue() && settings->triggerOverlayCheckFlashToggle->GetValue());
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}



	static bool UpdateTriggerCheckHookFunction(uint32_t triggerIndex, Datum entityDatum)
	{
		try
		{
			ScopedAtomicBool lock(instance->destructionGuard);

			LOG_ONCE(PLOG_DEBUG << "UpdateTriggerCheckHookFunction running first time");

			bool wasSuccessfulCheck = instance->updateTriggerCheckHook->getInlineHook().call<bool, Datum, uint32_t>(triggerIndex, entityDatum);

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

			auto triggerDataLock = getTriggerData->getTriggerData();

			auto now = std::chrono::steady_clock::now();

			for (auto& [triggerPointer, triggerData] : *triggerDataLock->filteredTriggers.get())
			{



				if (triggerData.triggerIndex == triggerIndex)
				{
					if (triggerData.isBSPTrigger == false)
					{
						LOG_ONCE_CAPTURE(PLOG_DEBUG << "updating timeLastChecked of trigger with index: " << t, t = triggerIndex);
						triggerData.timeLastChecked = now;
						triggerData.lastCheckSuccessful = wasSuccessfulCheck;
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
	UpdateTriggerLastCheckedImpl(GameState game, IDIContainer& dicon)
		:
		settings(dicon.Resolve<SettingsStateAndEvents>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
		getTriggerDataWeak(resolveDependentCheat(GetTriggerData)),
		triggerOverlayToggleCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->triggerOverlayToggle->valueChangedEvent, [this](bool& n) {onSettingChanged(); }),
		triggerOverlayCheckFlashToggleCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->triggerOverlayCheckFlashToggle->valueChangedEvent, [this](bool& n) {onSettingChanged(); }),
		getPlayerDatum(resolveDependentCheat(GetPlayerDatum))
	{

		instance = this;
		auto ptr = dicon.Resolve<PointerDataStore>().lock();
		auto updateTriggerLastCheckedFunction = ptr->getData < std::shared_ptr<MultilevelPointer>>(nameof(updateTriggerLastCheckedFunction), game);
		updateTriggerCheckHook = ModuleInlineHook::make(game.toModuleName(), updateTriggerLastCheckedFunction, UpdateTriggerCheckHookFunction);



	}

	~UpdateTriggerLastCheckedImpl()
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
		pimpl = std::make_unique<UpdateTriggerLastCheckedImpl<GameState::Value::Halo1>>(gameImpl, dicon);
		break;

	//case GameState::Value::Halo2:
	//	pimpl = std::make_unique<UpdateTriggerLastCheckedImpl<GameState::Value::Halo2>>(gameImpl, dicon);
	//	break;

	//case GameState::Value::Halo3:
	//	pimpl = std::make_unique<UpdateTriggerLastCheckedImpl<GameState::Value::Halo3>>(gameImpl, dicon);
	//	break;

	//case GameState::Value::Halo3ODST:
	//	pimpl = std::make_unique<UpdateTriggerLastCheckedImpl<GameState::Value::Halo3ODST>>(gameImpl, dicon);
	//	break;

	//case GameState::Value::HaloReach:
	//	pimpl = std::make_unique<UpdateTriggerLastCheckedImpl<GameState::Value::HaloReach>>(gameImpl, dicon);
	//	break;

	//case GameState::Value::Halo4:
	//	pimpl = std::make_unique<UpdateTriggerLastCheckedImpl<GameState::Value::Halo4>>(gameImpl, dicon);
	//	break;
	default:
		throw HCMInitException("not impl yet");
	}
}

UpdateTriggerLastChecked::~UpdateTriggerLastChecked() = default;