#include "pch.h"
#include "InfiniteAmmo.h"
#include "IMCCStateHook.h"
#include "IMessagesGUI.h"
#include "SettingsStateAndEvents.h"
#include "RuntimeExceptionHandler.h"
#include "MultilevelPointer.h"
#include "PointerManager.h"

template <GameState::Value gameT>
class InfiniteAmmoImplSetFlag : public IInfiniteAmmoImpl
{
private:
	
	// which game is this implementation for
	GameState mGame;

	// event callbacks
	ScopedCallback<ToggleEvent> mInfiniteAmmoCallbackHandle;

	// injected services
	std::weak_ptr<IMCCStateHook> mccStateHookWeak;
	std::weak_ptr<IMessagesGUI> messagesGUIWeak;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;

	//data
	std::shared_ptr<MultilevelPointer> infiniteAmmoFlag;

	// primary event callback
	void onInfiniteAmmoToggle(bool& newValue)
	{



		try
		{
			lockOrThrow(mccStateHookWeak, mccStateHook);
			lockOrThrow(messagesGUIWeak, messagesGUI);
			
			if (mccStateHook->isGameCurrentlyPlaying(mGame) == false) return;

			PLOG_DEBUG << "onInfiniteAmmoToggle called, newValue: " << newValue;

			byte newFlagValue = newValue ? 1 : 0;
			if (!infiniteAmmoFlag->writeData(&newFlagValue)) throw HCMRuntimeException(std::format("Failed to write infiniteAmmoFlag flag {}", MultilevelPointer::GetLastError()));

			messagesGUI->addMessage(newValue ? "Infinite Ammo enabled." : "Infinite Ammo disabled");
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}

	}


public:
	InfiniteAmmoImplSetFlag(GameState gameImpl, IDIContainer& dicon)
		: mGame(gameImpl),
		mInfiniteAmmoCallbackHandle(dicon.Resolve<SettingsStateAndEvents>().lock()->infiniteAmmoToggle->valueChangedEvent, [this](bool& n) {onInfiniteAmmoToggle(n); }),
		mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
		messagesGUIWeak(dicon.Resolve<IMessagesGUI>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>())
	{

		auto ptr = dicon.Resolve<PointerManager>().lock();
		infiniteAmmoFlag = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(infiniteAmmoFlag), mGame);
	}

	// unlike most cheats we want to make sure we set the flag back to zero when HCM closes
	~InfiniteAmmoImplSetFlag()
	{
		byte newFlagValue = 0;
		if (!infiniteAmmoFlag->writeData(&newFlagValue)) PLOG_ERROR << (std::format("Failed to write infiniteAmmoFlag flag {}", MultilevelPointer::GetLastError()));
	}
};



InfiniteAmmo::InfiniteAmmo(GameState gameImpl, IDIContainer& dicon)
{
	switch (gameImpl)
	{
	case GameState::Value::Halo1:
		pimpl = std::make_unique<InfiniteAmmoImplSetFlag<GameState::Value::Halo1>>(gameImpl, dicon);
		break;


	default:
		throw HCMInitException("not impl yet");
	}
}

InfiniteAmmo::~InfiniteAmmo()
{
	PLOG_DEBUG << "~" << getName();
}