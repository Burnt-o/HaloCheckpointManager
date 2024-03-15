#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "SettingsStateAndEvents.h"
#include "ModuleHook.h"
#include "MultilevelPointer.h"
#include "PointerDataStore.h"
#include "IMCCStateHook.h"

class IgnoreCheckpointChecksum : public IOptionalCheat
{
private:
	GameState mGame;

	// event callbacks
	ScopedCallback <ToggleEvent> mInjectionIgnoresChecksumCallbackHandle;

	// injected services
	std::weak_ptr<IMessagesGUI> messagesGUIWeak;
	std::weak_ptr<IMCCStateHook> mccStateHookWeak;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;

	//data
	std::shared_ptr<ModulePatch> ignoreChecksumPatch;

	// primary event callback
	void onToggleChange(bool& newValue)
	{
		try
		{
			lockOrThrow(mccStateHookWeak, mccStateHook);
			lockOrThrow(messagesGUIWeak, messagesGUI);


			if (mccStateHook->isGameCurrentlyPlaying(mGame))
			{
				PLOG_DEBUG << "onToggleChange: newval: " << newValue;
				messagesGUI->addMessage(newValue ? "Checkpoint checksums will be ignored on revert." : "Checkpoint checksums will NOT be ignored on revert.");
			}
			// gets set whether or not game is currently playing
			ignoreChecksumPatch->setWantsToBeAttached(newValue);
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}





	}

public:
	IgnoreCheckpointChecksum(GameState gameImpl, IDIContainer& dicon) :
		mInjectionIgnoresChecksumCallbackHandle(dicon.Resolve<SettingsStateAndEvents>().lock()->injectionIgnoresChecksum->valueChangedEvent, [this](bool& newValue) { onToggleChange(newValue); }),
		messagesGUIWeak(dicon.Resolve<IMessagesGUI>()),
		mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
		mGame(gameImpl),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>())
	{
		auto ptr = dicon.Resolve<PointerDataStore>().lock();
		auto checksumCheckFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(checksumCheckFunction), mGame);

		PLOG_DEBUG << "getting checksumIgnoreCode";
		auto checksumIgnoreCode = ptr->getVectorData<byte>(nameof(checksumIgnoreCode), mGame);
		PLOG_DEBUG << "got checksumIgnoreCode, size: " << checksumIgnoreCode->size();
		ignoreChecksumPatch = ModulePatch::make(mGame.toModuleName(), checksumCheckFunction, *checksumIgnoreCode.get());

		ignoreChecksumPatch->setWantsToBeAttached(dicon.Resolve<SettingsStateAndEvents>().lock()->injectionIgnoresChecksum->GetValue());
	}
	~IgnoreCheckpointChecksum() { PLOG_VERBOSE << "~" << getName(); }

std::string_view getName() override { return nameof(IgnoreCheckpointChecksum); }

};