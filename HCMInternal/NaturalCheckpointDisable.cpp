#include "pch.h"
#include "NaturalCheckpointDisable.h"
#include "PointerDataStore.h"
#include "ModuleHook.h"
#include "IMCCStateHook.h"
#include "IMessagesGUI.h"
#include "RuntimeExceptionHandler.h"
#include "SettingsStateAndEvents.h"

class NaturalCheckpointDisable::NaturalCheckpointDisableImpl  {
private:
	GameState mGame;

	// event callbacks
	ScopedCallback <ToggleEvent> mNaturalCheckpointDisableCallbackHandle;

	// injected services
	std::weak_ptr<IMCCStateHook> mccStateHookWeak;
	std::weak_ptr<IMessagesGUI> messagesGUIWeak;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;

	//data
	std::shared_ptr<ModulePatch> naturalCheckpointDisablePatch;



	// primary event callback
	void onToggleChange(bool& newValue)
	{
		try
		{
			lockOrThrow(mccStateHookWeak, mccStateHook);
			lockOrThrow(messagesGUIWeak, messagesGUI);

			naturalCheckpointDisablePatch->setWantsToBeAttached(newValue);
			PLOG_DEBUG << "onToggleChange: newval: " << newValue;


			if (mccStateHook->isGameCurrentlyPlaying(mGame))
			{
				messagesGUI->addMessage(newValue ? "Natural Checkpoints disabled." : "Natural Checkpoints re-enabled.");
			}
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}



	}



public:
	NaturalCheckpointDisableImpl(GameState game, IDIContainer& dicon) :
		mGame(game),
		mNaturalCheckpointDisableCallbackHandle(dicon.Resolve<SettingsStateAndEvents>().lock()->naturalCheckpointDisable->valueChangedEvent, [this](bool& n) { onToggleChange(n); }),
		mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
		messagesGUIWeak(dicon.Resolve<IMessagesGUI>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>())
	{
		auto ptr = dicon.Resolve<PointerDataStore>().lock();
		auto naturalCheckpointDisableFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(naturalCheckpointDisableFunction), mGame);
		auto naturalCheckpointDisableCode = ptr->getVectorData<byte>(nameof(naturalCheckpointDisableCode), mGame);
		naturalCheckpointDisablePatch = ModulePatch::make(game.toModuleName(), naturalCheckpointDisableFunction, *naturalCheckpointDisableCode.get());
	}

};

NaturalCheckpointDisable::NaturalCheckpointDisable(GameState gameImpl, IDIContainer& dicon)
	: pimpl(std::make_unique<NaturalCheckpointDisableImpl>(gameImpl, dicon))
{
}

NaturalCheckpointDisable::~NaturalCheckpointDisable()
{
	PLOG_VERBOSE << "~" << getName();
}