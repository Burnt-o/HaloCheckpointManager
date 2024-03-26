#include "pch.h"
#include "ForceTeleport.h"
#include "SettingsStateAndEvents.h"
#include "MultilevelPointer.h"
#include "MidhookContextInterpreter.h"
#include "PointerDataStore.h"
#include "SettingsStateAndEvents.h"
#include "Datum.h"
#include "RuntimeExceptionHandler.h"
#include "DIContainer.h"
#include "IMCCStateHook.h"
#include "GetPlayerDatum.h"
#include "IMakeOrGetCheat.h"
#include "GetPlayerViewAngle.h"
#include "GetObjectPhysics.h"
//#include "UnfreezeObjectPhysics.h"


class ForceTeleportSimple : public ForceTeleportImplUntemplated
{
private:
	GameState mGame;

	//callbacks
	ScopedCallback<ActionEvent> mForceTeleportEventCallback;
	ScopedCallback<ActionEvent> forceTeleportAbsoluteFillCurrentCallback;
	ScopedCallback<ActionEvent> forceTeleportAbsoluteCopyCallback;
	ScopedCallback<ActionEvent> forceTeleportAbsolutePasteCallback;


	// injected services
	std::weak_ptr<IMCCStateHook> mccStateHookWeak;
	std::weak_ptr<IMessagesGUI> messagesGUIWeak;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;
	std::weak_ptr<SettingsStateAndEvents> settingsWeak;
	std::weak_ptr< GetPlayerDatum> getPlayerDatumWeak;
	std::optional<std::weak_ptr<GetPlayerViewAngle>> getPlayerViewAngleWeak;
	std::weak_ptr< GetObjectPhysics> getObjectPhysicsWeak;
	//std::optional<std::weak_ptr< UnfreezeObjectPhysics>> unfreezeObjectPhysicsWeak;



	void onForceTeleportEvent()
	{
		bool teleportingPlayer = true;
		try
		{
			lockOrThrow(mccStateHookWeak, mccStateHook);
			if (!mccStateHook->isGameCurrentlyPlaying(mGame)) return;

			lockOrThrow(settingsWeak, settings);

			Datum datumToTeleport;
			if (settings->forceTeleportApplyToPlayer->GetValue())
			{
				lockOrThrow(getPlayerDatumWeak, getPlayerDatum);
				datumToTeleport = getPlayerDatum->getPlayerDatum();
			}
			else
			{
				teleportingPlayer = false;
				datumToTeleport = settings->forceTeleportCustomObject->GetValue();
			}
			PLOG_INFO << "Teleporting object with datum: " << datumToTeleport;

			// test for manual vs relative settings being in an invalid state (neither enabled or both enabled)
			// In a valid state, xor ing the values will return true.
			// If state invalid, we'll just set it to relative and display a message saying we fixed it.
			if ((settings->forceTeleportManual->GetValue() ^ settings->forceTeleportForward->GetValue()) == false)
			{
				settings->forceTeleportManual->GetValueDisplay() = false;
				settings->forceTeleportManual->UpdateValueWithInput();
				settings->forceTeleportForward->GetValueDisplay() = true;
				settings->forceTeleportForward->UpdateValueWithInput();
				lockOrThrow(messagesGUIWeak, messagesGUI);
				messagesGUI->addMessage("Force Teleport radio button was in invalid state. \nHCM has set it to a valid state (relative)");
			}


			if (settings->forceTeleportManual->GetValue())
			{
				teleportObjectToAbsolute(settings->forceTeleportAbsoluteVec3->GetValue(), datumToTeleport);
			}
			else 
			{
				teleportObjectRelativeToPlayer(settings->forceTeleportRelativeVec3->GetValue(), datumToTeleport);
			}

		}
		catch (HCMRuntimeException ex)
		{
			ex.prepend((teleportingPlayer ? "Error teleporting player: " : "Error teleporting custom object: "));
			runtimeExceptions->handleMessage(ex);
		}

	}



	void fillPosition()
	{
		try
		{
			lockOrThrow(mccStateHookWeak, mccStateHook);
			if (!mccStateHook->isGameCurrentlyPlaying(mGame)) return;

			lockOrThrow(settingsWeak, settings);

			Datum datumToFill;
			if (settings->forceTeleportApplyToPlayer)
			{
				lockOrThrow(getPlayerDatumWeak, getPlayerDatum);
				datumToFill = getPlayerDatum->getPlayerDatum();
			}
			else
			{
				datumToFill = settings->forceTeleportCustomObject->GetValue();
			}

			lockOrThrow(getObjectPhysicsWeak, getObjectPhysics);
			const auto* currentPosition = getObjectPhysics->getObjectPosition(datumToFill);

			settings->forceTeleportAbsoluteVec3->GetValueDisplay() = *currentPosition;
			settings->forceTeleportAbsoluteVec3->UpdateValueWithInput();
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}

	void copyPosition()
	{
		try
		{
			lockOrThrow(mccStateHookWeak, mccStateHook);
			if (!mccStateHook->isGameCurrentlyPlaying(mGame)) return;

			lockOrThrow(settingsWeak, settings);

			settings->forceTeleportAbsoluteVec3->serialiseToClipboard();
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}

	void pastePosition()
	{
		try
		{
			lockOrThrow(mccStateHookWeak, mccStateHook);
			if (!mccStateHook->isGameCurrentlyPlaying(mGame)) return;

			lockOrThrow(settingsWeak, settings);

			settings->forceTeleportAbsoluteVec3->deserialiseFromClipboard();
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}
	void teleportObjectToAbsolute(SimpleMath::Vector3 coords, Datum entityToTeleport); // throws HCMRuntimes
	void teleportObjectRelativeToPlayer(SimpleMath::Vector3 coords, Datum entityToTeleport); // throws HCMRuntimes

public:
	ForceTeleportSimple(GameState game, IDIContainer& dicon)
		: mGame(game),
		settingsWeak(dicon.Resolve<SettingsStateAndEvents>()),
		mForceTeleportEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->forceTeleportEvent, [this]() {onForceTeleportEvent(); }),
		mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
		messagesGUIWeak(dicon.Resolve<IMessagesGUI>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
		getPlayerDatumWeak(resolveDependentCheat(GetPlayerDatum)),
		getObjectPhysicsWeak(resolveDependentCheat(GetObjectPhysics)),
		forceTeleportAbsoluteFillCurrentCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->forceTeleportAbsoluteFillCurrent, [this]() { fillPosition(); }),
		forceTeleportAbsoluteCopyCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->forceTeleportAbsoluteCopy, [this]() { copyPosition(); }),
		forceTeleportAbsolutePasteCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->forceTeleportAbsolutePaste, [this]() { pastePosition(); })
	{

		try
		{
			getPlayerViewAngleWeak = resolveDependentCheat(GetPlayerViewAngle);
		}
		catch (HCMInitException ex)
		{
			PLOG_ERROR << "Could not resolve getPlayerViewAngleWeak: " << ex.what();
		}

	}




	virtual void teleportPlayerTo(SimpleMath::Vector3 position) override
	{
		lockOrThrow(getPlayerDatumWeak, getPlayerDatum);
		auto playerDatum = getPlayerDatum->getPlayerDatum();
		if (playerDatum.isNull()) throw HCMRuntimeException("Null player datum!");
		teleportObjectToAbsolute(position, playerDatum);
	}
	virtual void teleportEntityTo(SimpleMath::Vector3 position, Datum entityDatum) override
	{
		teleportObjectToAbsolute(position, entityDatum);
	}
};




ForceTeleport::ForceTeleport(GameState gameImpl, IDIContainer& dicon)
{
	pimpl = std::make_unique < ForceTeleportSimple>(gameImpl, dicon);
}

ForceTeleport::~ForceTeleport() = default;




void ForceTeleportSimple::teleportObjectToAbsolute(SimpleMath::Vector3 coords, Datum entityToTeleport) // throws HCMRuntimes
{
	lockOrThrow(settingsWeak, settings);



	lockOrThrow(getObjectPhysicsWeak, getObjectPhysics);
	auto currentPositionMutableAndVisual = getObjectPhysics->getObjectPositionMutableAndVisual(entityToTeleport);

	*currentPositionMutableAndVisual.first = coords;
	*currentPositionMutableAndVisual.second = coords;

	//if (unfreezeObjectPhysicsWeak.has_value())
	//{
	//	lockOrThrow(unfreezeObjectPhysicsWeak.value(), unfreezeObjectPhysics);
	//	unfreezeObjectPhysics->unfreezeObjectPhysics(playerDatum);
	//}
	//else
	//{
	//	PLOG_ERROR << "unfreezeObjectPhysics service failed, cannot unfreeze physics";
	//}


	lockOrThrow(messagesGUIWeak, messagesGUI);
	messagesGUI->addMessage(std::format("Teleporting to {:.2f}, {:.2f}, {:.2f}", currentPositionMutableAndVisual.first->x, currentPositionMutableAndVisual.first->y, currentPositionMutableAndVisual.first->z));
}


void ForceTeleportSimple::teleportObjectRelativeToPlayer(SimpleMath::Vector3 coords, Datum entityToTeleport) // throws HCMRuntimes
{
	lockOrThrow(settingsWeak, settings);
	if (!getPlayerViewAngleWeak.has_value()) throw HCMRuntimeException("could not resolve getPlayerViewAngleWeak");

	lockOrThrow(getPlayerViewAngleWeak.value(), getPlayerViewAngle);

	auto playerViewAngle = getPlayerViewAngle->getPlayerViewAngle();


	lockOrThrow(getPlayerDatumWeak, getPlayerDatum);
	Datum playerDatum = getPlayerDatum->getPlayerDatum();


	// teleport relative to players position, even if we're teleporting an object other than the player
	lockOrThrow(getObjectPhysicsWeak, getObjectPhysics);
	auto const playersCurrentPosition= getObjectPhysics->getObjectPosition(playerDatum);

	// now let's do some math to figure out what the new position should be
	SimpleMath::Vector3 normalisedLookDirForward =
	{
		std::cos(playerViewAngle.y) * std::cos(playerViewAngle.x),
		std::cos(playerViewAngle.y) * std::sin(playerViewAngle.x),
		std::sin(playerViewAngle.y)
	};

	normalisedLookDirForward.Normalize();

	if (settings->forceTeleportForwardIgnoreZ->GetValue())
	{
		// need to renormalise the lookDir without the z value;
		float totalLookMagnitude = std::abs(normalisedLookDirForward.x) + std::abs(normalisedLookDirForward.y);
		float renormalisationFactor = 1 / totalLookMagnitude;
		normalisedLookDirForward.x *= renormalisationFactor;
		normalisedLookDirForward.y *= renormalisationFactor;
		normalisedLookDirForward.z = 0;
	}


	auto normalisedLookDirRight = SimpleMath::Vector3::Distance(SimpleMath::Vector3::UnitZ, normalisedLookDirForward) < 0.001 // gimbal lock check
		? SimpleMath::Vector3::UnitX // choose an arbitary xy angle as our right vec to avoid gimbal lock
		: normalisedLookDirForward.Cross(SimpleMath::Vector3::UnitZ);

	normalisedLookDirRight.Normalize();

	auto normalisedLookDirUp = normalisedLookDirRight.Cross(normalisedLookDirForward);

	normalisedLookDirUp.Normalize();
	
	SimpleMath::Vector3 newPosition = * playersCurrentPosition
		+ (normalisedLookDirForward * coords.x) // forward component of user input 
		+ (normalisedLookDirRight * coords.y) // right component of user input
		+ (normalisedLookDirUp * coords.z); // up component of user input


	auto currentPositionMutableAndVisual = getObjectPhysics->getObjectPositionMutableAndVisual(entityToTeleport);
	*currentPositionMutableAndVisual.first = newPosition;
	*currentPositionMutableAndVisual.second = newPosition;

	//if (unfreezeObjectPhysicsWeak.has_value())
	//{
	//	lockOrThrow(unfreezeObjectPhysicsWeak.value(), unfreezeObjectPhysics);
	//	unfreezeObjectPhysics->unfreezeObjectPhysics(playerDatum);
	//}
	//else
	//{
	//	PLOG_ERROR << "unfreezeObjectPhysics service failed, cannot unfreeze physics";
	//}

	lockOrThrow(messagesGUIWeak, messagesGUI);
	messagesGUI->addMessage(std::format("Teleporting to {:.2f}, {:.2f}, {:.2f}", newPosition.x, newPosition.y, newPosition.z));

}



void ForceTeleport::teleportPlayerTo(SimpleMath::Vector3 position) { return pimpl->teleportPlayerTo(position); }
void ForceTeleport::teleportEntityTo(SimpleMath::Vector3 position, Datum entityDatum) { return pimpl->teleportEntityTo(position, entityDatum); }