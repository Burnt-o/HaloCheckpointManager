#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "IMCCStateHook.h"
#include "MultilevelPointer.h"
#include "PointerDataStore.h"
#include "IMessagesGUI.h"
#include "SettingsStateAndEvents.h"
#include "RuntimeExceptionHandler.h"
#include "Datum.h"
#include "TagTableRange.h"
#include "IMakeOrGetCheat.h"


class GetTagAddressPresenter : public IOptionalCheat
{
private:
	// which game is this implementation for
	GameState mGame;

	// event callbacks
	ScopedCallback<ActionEvent> getTagAddressGUIEventCallback;



	// injected services
	std::weak_ptr<IMCCStateHook> mccStateHookWeak;
	std::weak_ptr<IMessagesGUI> messagesGUIWeak;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;
	std::weak_ptr<SettingsStateAndEvents> mSettingsWeak;
	std::weak_ptr<TagTableRange> tagTableRangeWeak;

	// primary event callback
	void onGetTagAddressGUIEvent()
	{
		try
		{
			lockOrThrow(mccStateHookWeak, mccStateHook);

			if (!mccStateHook->isGameCurrentlyPlaying(mGame)) return;


			lockOrThrow(mSettingsWeak, settings);
			lockOrThrow(tagTableRangeWeak, tagTableRange);

			Datum actualDatum = settings->getTagAddressDWORD->GetValue();

			PLOG_DEBUG << "onGetObjectAddressGUIEvent processing datum: " << actualDatum << " for game: " << mGame.toString();

			auto tagAddress = tagTableRange->getTagByDatum(actualDatum);
			if (!tagAddress)
				throw tagAddress.error();

			lockOrThrow(messagesGUIWeak, messagesGUI);

			std::stringstream ss;
			ss << std::hex << std::uppercase << tagAddress.value().tagAddress;
			PLOG_VERBOSE << "Tag Address: " << ss.str();

			messagesGUI->addMessage(std::format("Tag Address: 0x{}\n", ss.str()));
			ImGui::SetClipboardText(ss.str().c_str());
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}




public:

	GetTagAddressPresenter(GameState game, IDIContainer& dicon)
		: mGame(game),
		getTagAddressGUIEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->getTagAddressEvent, [this]() {onGetTagAddressGUIEvent(); }),
		mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
		messagesGUIWeak(dicon.Resolve<IMessagesGUI>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
		mSettingsWeak(dicon.Resolve<SettingsStateAndEvents>())

	{
		PLOG_VERBOSE << "constructing GetTagAddressPresenter OptionalCheat for game: " << mGame.toString();
		tagTableRangeWeak = resolveDependentCheat(TagTableRange);
	}

	virtual std::string_view getName() override {
		return nameof(GetObjectAddressCLI);
	}

	~GetTagAddressPresenter()
	{
		PLOG_VERBOSE << "~" << getName();
	}

};
