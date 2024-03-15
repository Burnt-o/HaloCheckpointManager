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
#include "GetObjectAddress.h"
#include "IMakeOrGetCheat.h"

#include "ObjectTypes.h"

class GetObjectAddressCLI : public IOptionalCheat
{
private:
	// which game is this implementation for
	GameState mGame;

	// event callbacks
	ScopedCallback<ActionEvent> getObjectAddressGUIEventCallback;



	// injected services
	std::weak_ptr<IMCCStateHook> mccStateHookWeak;
	std::weak_ptr<IMessagesGUI> messagesGUIWeak;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;
	std::weak_ptr<SettingsStateAndEvents> mSettingsWeak;
	std::weak_ptr<GetObjectAddress> getObjectAddressWeak;

	// primary event callback
	void onGetObjectAddressGUIEvent()
	{
		try
		{
			lockOrThrow(mccStateHookWeak, mccStateHook);

			if (!mccStateHook->isGameCurrentlyPlaying(mGame)) return;


			lockOrThrow(mSettingsWeak, settings);
			lockOrThrow(getObjectAddressWeak, getObjectAddress);

			Datum actualDatum = settings->getObjectAddressDWORD->GetValue();

			PLOG_DEBUG << "onGetObjectAddressGUIEvent processing datum: " << actualDatum << " for game: " << mGame.toString();

			CommonObjectType objectType;
			auto objectAddress = getObjectAddress->getObjectAddress(actualDatum, &objectType);

			lockOrThrow(messagesGUIWeak, messagesGUI);

			std::stringstream ss;
			ss << std::hex << std::uppercase << objectAddress;
			PLOG_VERBOSE << "Object Address: " << ss.str();

			messagesGUI->addMessage(std::format("Object Address: 0x{}\nObject Type: {}", ss.str(), magic_enum::enum_name<CommonObjectType>(objectType)));
			ImGui::SetClipboardText(ss.str().c_str());
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}




public:

	GetObjectAddressCLI(GameState game, IDIContainer& dicon)
		: mGame(game),
		getObjectAddressGUIEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->getObjectAddressEvent, [this]() {onGetObjectAddressGUIEvent(); }),
		mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
		messagesGUIWeak(dicon.Resolve<IMessagesGUI>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
		mSettingsWeak(dicon.Resolve<SettingsStateAndEvents>())

	{
		PLOG_VERBOSE << "constructing GetObjectAddressCLI OptionalCheat for game: " << mGame.toString();
		getObjectAddressWeak = resolveDependentCheat(GetObjectAddress);
	}

	virtual std::string_view getName() override {
		return nameof(GetObjectAddressCLI);
	}

	~GetObjectAddressCLI()
	{
		PLOG_VERBOSE << "~" << getName();
	}

};
