#include "pch.h"
#include "NaturalCheckpointDisable.h"
#include "PointerManager.h"
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
	std::weak_ptr<IMCCStateHook> mccStateHook;
	std::weak_ptr<IMessagesGUI> messagesGUI;
	std::weak_ptr<RuntimeExceptionHandler> runtimeExceptions;

	//data
	std::shared_ptr<ModulePatch> naturalCheckpointDisablePatch;



	// primary event callback
	void onToggleChange(bool& newValue)
	{
		naturalCheckpointDisablePatch->setWantsToBeAttached(newValue);
		PLOG_DEBUG << "onToggleChange: newval: " << newValue;


		if (mccStateHook.lock()->isGameCurrentlyPlaying(mGame))
		{
			messagesGUI.lock()->addMessage(newValue ? "Natural Checkpoints disabled." : "Natural Checkpoints re-enabled.");
		}

	}



public:
	NaturalCheckpointDisableImpl(GameState game, IDIContainer& dicon) :
		mGame(game),
		mNaturalCheckpointDisableCallbackHandle(dicon.Resolve<SettingsStateAndEvents>().lock()->naturalCheckpointDisable->valueChangedEvent, [this](bool& n) { onToggleChange(n); }),
		mccStateHook(dicon.Resolve<IMCCStateHook>()),
		messagesGUI(dicon.Resolve<IMessagesGUI>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>())
	{
		auto ptr = dicon.Resolve<PointerManager>().lock();
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