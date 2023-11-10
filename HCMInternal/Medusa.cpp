#include "pch.h"
#include "Medusa.h"
#include "IMCCStateHook.h"
#include "IMessagesGUI.h"
#include "RuntimeExceptionHandler.h"
#include "ModuleHook.h"
#include "MidhookFlagInterpreter.h"
#include "SettingsStateAndEvents.h"
#include "PointerManager.h"
#include "MultilevelPointer.h"

template<GameState::Value mGame>
class MedusaImpl : public MedusaImplUntemplated
{
private:
	// event callbacks
	ScopedCallback <ToggleEvent> mMedusaToggleCallbackHandle;

	// injected services
	std::weak_ptr<IMCCStateHook> mccStateHookWeak;
	std::weak_ptr<IMessagesGUI> messagesGUIWeak;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;

	//data
	static inline std::shared_ptr<ModuleMidHook> medusaHook;
	static inline std::shared_ptr< MidhookFlagInterpreter> medusaFunctionFlagSetter;





	static void medusaHookFunction(SafetyHookContext& ctx)
	{
		medusaFunctionFlagSetter->setFlag(ctx);
	}



	// primary event callback
	void onToggleChange(bool& newValue)
	{
		try
		{
			medusaHook->setWantsToBeAttached(newValue);
			PLOG_DEBUG << "onToggleChange: newval: " << newValue;

			lockOrThrow(mccStateHookWeak, mccStateHook);

			if (mccStateHook->isGameCurrentlyPlaying(mGame))
			{
				lockOrThrow(messagesGUIWeak, messagesGUI);
				messagesGUI->addMessage(newValue ? "Medusa enabled." : "Medusa disabled.");
			}
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
		

	}



public:
	MedusaImpl(GameState gameImpl, IDIContainer& dicon)
		: mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
		messagesGUIWeak(dicon.Resolve<IMessagesGUI>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
		mMedusaToggleCallbackHandle(dicon.Resolve< SettingsStateAndEvents>().lock()->medusaToggle->valueChangedEvent, [this](bool& n) {onToggleChange(n); })
	{
		auto ptr = dicon.Resolve<PointerManager>().lock();
		auto medusaFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(medusaFunction), gameImpl);
		medusaFunctionFlagSetter = ptr->getData< std::shared_ptr< MidhookFlagInterpreter>>(nameof(medusaFunctionFlagSetter), gameImpl);
		medusaHook = ModuleMidHook::make(gameImpl.toModuleName(), medusaFunction, medusaHookFunction);

	}



};



Medusa::Medusa(GameState gameImpl, IDIContainer& dicon)
{
	switch (gameImpl)
	{
	case GameState::Value::Halo1:
		pimpl = std::make_unique <MedusaImpl<GameState::Value::Halo1>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo2:
		pimpl = std::make_unique <MedusaImpl<GameState::Value::Halo2>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo3:
		pimpl = std::make_unique <MedusaImpl<GameState::Value::Halo3>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo3ODST:
		pimpl = std::make_unique <MedusaImpl<GameState::Value::Halo3ODST>>(gameImpl, dicon);
		break;

	case GameState::Value::HaloReach:
		pimpl = std::make_unique <MedusaImpl<GameState::Value::HaloReach>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo4:
		pimpl = std::make_unique <MedusaImpl<GameState::Value::Halo4>>(gameImpl, dicon);
		break;

	}

}
Medusa::~Medusa()
{
	PLOG_DEBUG << "~" << getName();
}