#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "SettingsStateAndEvents.h"
#include "ModuleHook.h"
#include "MultilevelPointer.h"
#include "PointerManager.h"
#include "IMCCStateHook.h"

class IgnoreCheckpointChecksum : public IOptionalCheat
{
private:
	GameState mGame;

	// event callbacks
	ScopedCallback <ToggleEvent> mInjectionIgnoresChecksumCallbackHandle;

	// injected services
	std::weak_ptr<IMessagesGUI> messagesGUI;
	std::weak_ptr<IMCCStateHook> mccStateHook;

	//data
	std::shared_ptr<ModulePatch> ignoreChecksumPatch;

	// primary event callback
	void onToggleChange(bool& newValue)
	{
		ignoreChecksumPatch->setWantsToBeAttached(newValue);
		PLOG_DEBUG << "onToggleChange: newval: " << newValue;
		if (mccStateHook.lock()->isGameCurrentlyPlaying(mGame))
		{
			messagesGUI.lock()->addMessage(newValue ? "Checkpoint checksums will be ignored on revert." : "Checkpoint checksums will NOT be ignored on revert.");
		}

	}

public:
	IgnoreCheckpointChecksum(GameState gameImpl, IDIContainer& dicon) :
		mInjectionIgnoresChecksumCallbackHandle(dicon.Resolve<SettingsStateAndEvents>().lock()->injectionIgnoresChecksum->valueChangedEvent, [this](bool& newValue) { onToggleChange(newValue); }),
		messagesGUI(dicon.Resolve<IMessagesGUI>()),
		mccStateHook(dicon.Resolve<IMCCStateHook>()),
		mGame(gameImpl)
	{
		auto ptr = dicon.Resolve<PointerManager>().lock();
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