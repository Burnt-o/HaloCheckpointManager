#include "pch.h"
#include "DisableBarriers.h"
#include "PointerDataStore.h"
#include "MultilevelPointer.h"
#include "ModuleHook.h"
#include "RuntimeExceptionHandler.h"
#include "IMessagesGUI.h"
#include "SettingsStateAndEvents.h"
#include "IMCCStateHook.h"

class DisableBarriers::DisableBarriersImpl
{
private:
	
	// callbacks
	ScopedCallback<ToggleEvent> disableBarriersToggleCallback;

	// injected services
	std::weak_ptr< IMessagesGUI> messagesWeak;
	std::weak_ptr< IMCCStateHook> mccStateHookWeak;
	std::shared_ptr< RuntimeExceptionHandler> runtimeExceptions;

	// data
	GameState mGame;
	std::shared_ptr<ModulePatch> disableBarriersPatch;

	void onDisableBarriersToggle(bool& newValue)
	{
		try
		{
			lockOrThrow(mccStateHookWeak, mccStateHook);
			if (mccStateHook->isGameCurrentlyPlaying(mGame))
			{
				lockOrThrow(messagesWeak, messages);
				messages->addMessage(std::format("Barriers {}.", (newValue ? "Disabled" : "Enabled")));
			}

			disableBarriersPatch->setWantsToBeAttached(newValue);
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}

	}



public:
	DisableBarriersImpl(GameState game, IDIContainer& dicon)
		: mGame(game),
		mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
		messagesWeak(dicon.Resolve<IMessagesGUI>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
		disableBarriersToggleCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->disableBarriersToggle->valueChangedEvent, [this](bool& n) {onDisableBarriersToggle(n); })
	{
		auto ptr = dicon.Resolve<PointerDataStore>().lock();

		auto disableBarriersPatchFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(disableBarriersPatchFunction), game);
		auto disableBarriersPatchCode = ptr->getVectorData<byte>(nameof(disableBarriersPatchCode), game);
		disableBarriersPatch = ModulePatch::make(game.toModuleName(), disableBarriersPatchFunction, *disableBarriersPatchCode.get());
	}
};


DisableBarriers::DisableBarriers(GameState game, IDIContainer& dicon)
{
	pimpl = std::make_unique< DisableBarriersImpl>(game, dicon);
}

DisableBarriers::~DisableBarriers() = default;