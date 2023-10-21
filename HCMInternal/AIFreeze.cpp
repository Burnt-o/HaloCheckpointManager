#include "pch.h"
#include "AIFreeze.h"
#include "PointerManager.h"

template<GameState::Value mGame>
class AIFreezeImpl : public AIFreezeImplUntemplated {
	// event callbacks
	ScopedCallback <ToggleEvent> mAIFreezeCallbackHandle;

	// injected services
	std::weak_ptr<IMCCStateHook> mccStateHook;
	std::weak_ptr<IMessagesGUI> messagesGUI;
	std::weak_ptr<RuntimeExceptionHandler> runtimeExceptions;

	//data
	static inline std::shared_ptr<ModuleMidHook> aiFreezeHook;
	static inline std::shared_ptr< MidhookFlagInterpreter> aiFreezeFunctionFlagSetter;





	static void aiFreezeHookFunction(SafetyHookContext& ctx)
	{
		aiFreezeFunctionFlagSetter->setFlag(ctx);
	}



	// primary event callback
	void onToggleChange(bool& newValue)
	{
		aiFreezeHook->setWantsToBeAttached(newValue);
		PLOG_DEBUG << "onToggleChange: newval: " << newValue;


		if (mccStateHook.lock()->isGameCurrentlyPlaying(mGame))
		{
			messagesGUI.lock()->addMessage(newValue ? "AI frozen." : "AI unfrozen.");
		}

	}



public:
	AIFreezeImpl(IDIContainer& dicon) :
		mAIFreezeCallbackHandle(dicon.Resolve<SettingsStateAndEvents>().lock()->aiFreezeToggle->valueChangedEvent, [this](bool& n) { onToggleChange(n); }),
		mccStateHook(dicon.Resolve<IMCCStateHook>()),
		messagesGUI(dicon.Resolve<IMessagesGUI>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>())
	{
		auto ptr = dicon.Resolve<PointerManager>().lock();
		auto aiFreezeFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(aiFreezeFunction), mGame);
		aiFreezeFunctionFlagSetter = ptr->getData<std::shared_ptr<MidhookFlagInterpreter>>(nameof(aiFreezeFunctionFlagSetter), mGame);
		aiFreezeHook = ModuleMidHook::make( GameState(mGame).toModuleName(), aiFreezeFunction, aiFreezeHookFunction);
	}

};

AIFreeze::AIFreeze(GameState gameImpl, IDIContainer& dicon)
{
	switch (gameImpl)
	{
	case GameState::Value::Halo1:
		pimpl = std::make_unique<AIFreezeImpl<GameState::Value::Halo1>>(dicon);
			break;

	case GameState::Value::Halo2:
		pimpl = std::make_unique<AIFreezeImpl<GameState::Value::Halo2>>(dicon);
		break;

	case GameState::Value::Halo3:
		pimpl = std::make_unique<AIFreezeImpl<GameState::Value::Halo3>>(dicon);
		break;

	case GameState::Value::Halo3ODST:
		pimpl = std::make_unique<AIFreezeImpl<GameState::Value::Halo3ODST>>(dicon);
		break;

	case GameState::Value::HaloReach:
		pimpl = std::make_unique<AIFreezeImpl<GameState::Value::HaloReach>>(dicon);
		break;

	case GameState::Value::Halo4:
		pimpl = std::make_unique<AIFreezeImpl<GameState::Value::Halo4>>(dicon);
		break;
	default:
		throw HCMInitException("not impl yet");
	}
}
AIFreeze::~AIFreeze()
{
		PLOG_VERBOSE << "~" << getName();
}