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
#include "GetPlayerDatum.h"
#include "IMakeOrGetCheat.h"


class GetPlayerDatumPresenter : public IOptionalCheat
{
private:
	// which game is this implementation for
	GameState mGame;

	// event callbacks
	ScopedCallback<ActionEvent> getPlayerDatumEventCallback;



	// injected services
	std::weak_ptr<IMCCStateHook> mccStateHookWeak;
	std::weak_ptr<IMessagesGUI> messagesWeak;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;
	std::weak_ptr<GetPlayerDatum> getPlayerDatumWeak;

	// primary event callback
	void onGetPlayerDatumEvent()
	{
		try
		{
			lockOrThrow(mccStateHookWeak, mccStateHook);
			if (mccStateHook->isGameCurrentlyPlaying(mGame) == false)
				return;

			lockOrThrow(getPlayerDatumWeak, getPlayerDatum)
			auto playerDatum = getPlayerDatum->getPlayerDatum();

			lockOrThrow(messagesWeak, messages);
			messages->addMessage(std::format("Player Datum: 0x{}", playerDatum.toString()));
			ImGui::SetClipboardText(playerDatum.toString().c_str());
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}




public:

	GetPlayerDatumPresenter(GameState game, IDIContainer& dicon)
		: mGame(game),
		getPlayerDatumEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->getPlayerDatumEvent, [this]() {onGetPlayerDatumEvent(); }),
		mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
		messagesWeak(dicon.Resolve<IMessagesGUI>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>())

	{
		PLOG_VERBOSE << "constructing GetPlayerDatumPresenter OptionalCheat for game: " << mGame.toString();
		getPlayerDatumWeak = resolveDependentCheat(GetPlayerDatum);
	}

	virtual std::string_view getName() override {
		return nameof(GetObjectAddressCLI);
	}

	~GetPlayerDatumPresenter()
	{
		PLOG_VERBOSE << "~" << getName();
	}

};
