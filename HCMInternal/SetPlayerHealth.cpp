#include "pch.h"
#include "SetPlayerHealth.h"
#include "RuntimeExceptionHandler.h"
#include "IMessagesGUI.h"
#include "SettingsStateAndEvents.h"
#include "IMakeOrGetCheat.h"
#include "GetObjectHealth.h"
#include "GetPlayerDatum.h"
#include "IMCCStateHook.h"

class SetPlayerHealth::SetPlayerHealthImpl
{
private:

	GameState mImplGame;

	//event callbacks
	ScopedCallback<ActionEvent> SetPlayerHealthSetCallback;

	// injected services
	std::weak_ptr<IMessagesGUI> messagesGUIWeak;
	std::weak_ptr<SettingsStateAndEvents> settingsWeak;
	std::weak_ptr<GetObjectHealth> getObjectHealthWeak;
	std::weak_ptr<GetPlayerDatum> getPlayerDatumWeak;
	std::weak_ptr<IMCCStateHook> mccStateHookWeak;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;

	void onSetPlayerHealth()
	{
		try
		{

			lockOrThrow(mccStateHookWeak, mccStateHook);
			if (mccStateHook->isGameCurrentlyPlaying(mImplGame) == false) return;


			lockOrThrow(getObjectHealthWeak, getObjectHealth);
			lockOrThrow(getPlayerDatumWeak, getPlayerDatum);
			lockOrThrow(settingsWeak, settings);

			Datum playerDatum = getPlayerDatum->getPlayerDatum();

			if (playerDatum.isNull()) throw HCMRuntimeException("Null player datum!");

			// get mutable pointer to player health/shields
			auto* playerHealth = getObjectHealth->getObjectHealthMutable(playerDatum);
			auto* playerShields = getObjectHealth->getObjectShieldsMutable(playerDatum);

			// write the new value 
			playerHealth->value = settings->setPlayerHealthVec2->GetValue().x;
			playerShields->value = settings->setPlayerHealthVec2->GetValue().y;


			lockOrThrow(messagesGUIWeak, messagesGUI);
			messagesGUI->addMessage(std::format("Setting player health/shields: {}", vec2ToString(settings->setPlayerHealthVec2->GetValue())));

		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}



public:
	SetPlayerHealthImpl(GameState game, IDIContainer& dicon)
		:mImplGame(game),
		messagesGUIWeak(dicon.Resolve<IMessagesGUI>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
		settingsWeak(dicon.Resolve<SettingsStateAndEvents>()),
		mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
		SetPlayerHealthSetCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->setPlayerHealthEvent, [this]() {onSetPlayerHealth(); }),
		getObjectHealthWeak(resolveDependentCheat(GetObjectHealth)),
		getPlayerDatumWeak(resolveDependentCheat(GetPlayerDatum))
	{

	}
};




SetPlayerHealth::SetPlayerHealth(GameState game, IDIContainer& dicon)
{
	pimpl = std::make_unique< SetPlayerHealth::SetPlayerHealthImpl>(game, dicon);
}

SetPlayerHealth::~SetPlayerHealth()
{
	PLOG_DEBUG << "~" << getName();
}