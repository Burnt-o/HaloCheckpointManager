#include "pch.h"
#include "SwitchBSP.h"
#include "IMCCStateHook.h"
#include "IMessagesGUI.h"
#include "SettingsStateAndEvents.h"
#include "RuntimeExceptionHandler.h"
#include "PointerDataStore.h"
#include "MultilevelPointer.h"

template <GameState::Value gameT>
class SwitchBSPImpl : public ISwitchBSPImpl
{
	// which game is this implementation for
	GameState mGame;

	// event callbacks
	ScopedCallback<ActionEvent> switchBSPEventCallbackHandle;

	// injected services
	std::weak_ptr<IMCCStateHook> mccStateHookWeak;
	std::weak_ptr<IMessagesGUI> messagesGUIWeak;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;
	std::weak_ptr<SettingsStateAndEvents> settingsWeak;

	// data
	std::shared_ptr<MultilevelPointer> switchBSPFunction;

	// primary event callback
	void onSwitchBSPEvent()
	{
		try
		{
			PLOG_DEBUG << "onSwitchBSPEvent";
			lockOrThrow(mccStateHookWeak, mccStateHook);
			if (mccStateHook->isGameCurrentlyPlaying(mGame) == false) return;
			 
			lockOrThrow(settingsWeak, settings);
			lockOrThrow(messagesGUIWeak, messagesGUI);
			int64_t newBSPIndex = settings->switchBSPIndex->GetValue();

			uintptr_t pSwitchBSPFunction;
			if (!switchBSPFunction->resolve(&pSwitchBSPFunction)) throw HCMRuntimeException("Could not resolve pointer to switchBSPFunction function");      

			typedef int64_t(__fastcall *SwitchBSPFunction_t)(int64_t newIndex);
			SwitchBSPFunction_t switchBSPFunction_vptr = static_cast<SwitchBSPFunction_t>((void*)pSwitchBSPFunction);

			PLOG_DEBUG << "Calling switch BSP function at ptr " << (uint64_t)pSwitchBSPFunction << " with arg " << newBSPIndex;
			auto result = switchBSPFunction_vptr(newBSPIndex);

			PLOG_VERBOSE << "useless switchBSP return: " << result; // we get garbage data here cos we're calling a subroutine. would prob need to switch to a inline hook if we cared but it doesn't really matter

			// todo (maybe): get proper return (success value) and throw if failed
			constexpr const char* nameOfThing = (gameT == GameState::Value::Halo1 || gameT == GameState::Value::Halo2) ? "BSP" : "Zone Set";
			messagesGUI->addMessage(std::format("Switching {} to {}", nameOfThing, newBSPIndex));
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
		


	}

public:
	SwitchBSPImpl(GameState gameImpl, IDIContainer& dicon)
		: mGame(gameImpl),
		switchBSPEventCallbackHandle(dicon.Resolve<SettingsStateAndEvents>().lock()->switchBSPEvent, [this]() { onSwitchBSPEvent(); }),
		mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
		messagesGUIWeak(dicon.Resolve<IMessagesGUI>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
		settingsWeak(dicon.Resolve<SettingsStateAndEvents>())

	{
		auto ptr = dicon.Resolve<PointerDataStore>().lock();
		switchBSPFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(switchBSPFunction), mGame);
	}
};





SwitchBSP::SwitchBSP(GameState gameImpl, IDIContainer& dicon)
{
	switch (gameImpl)
	{
		case GameState::Value::Halo1:
			pimpl = std::make_unique<SwitchBSPImpl<GameState::Value::Halo1>>(gameImpl, dicon);
			break;

		case GameState::Value::Halo2:
			pimpl = std::make_unique<SwitchBSPImpl<GameState::Value::Halo2>>(gameImpl, dicon);
			break;

		case GameState::Value::Halo3:
			pimpl = std::make_unique<SwitchBSPImpl<GameState::Value::Halo3>>(gameImpl, dicon);
			break;

		case GameState::Value::Halo3ODST:
			pimpl = std::make_unique<SwitchBSPImpl<GameState::Value::Halo3ODST>>(gameImpl, dicon);
			break;

		case GameState::Value::HaloReach:
			pimpl = std::make_unique<SwitchBSPImpl<GameState::Value::HaloReach>>(gameImpl, dicon);
			break;

		case GameState::Value::Halo4:
			pimpl = std::make_unique<SwitchBSPImpl<GameState::Value::Halo4>>(gameImpl, dicon);
			break;
	default:
		throw HCMInitException("not impl yet");
	}
}

SwitchBSP::~SwitchBSP()
{
	PLOG_DEBUG << "~" << getName();
}