#include "pch.h"
#include "ForceLaunch.h"
#include "SettingsStateAndEvents.h"
#include "MultilevelPointer.h"
#include "MidhookContextInterpreter.h"
#include "PointerManager.h"
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

class ForceLaunchSimple : public ForceLaunchImplUntemplated
{
private:
	GameState mGame;

	//callbacks
	ScopedCallback<ActionEvent> mForceLaunchEventCallback;


	// injected services
	std::weak_ptr<IMCCStateHook> mccStateHookWeak;
	std::weak_ptr<IMessagesGUI> messagesGUIWeak;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;
	std::weak_ptr<SettingsStateAndEvents> settingsWeak;
	std::weak_ptr< GetPlayerDatum> getPlayerDatumWeak;
	std::optional<std::weak_ptr<GetPlayerViewAngle>> getPlayerViewAngleWeak;
	std::weak_ptr< GetObjectPhysics> getObjectPhysicsWeak;
	//std::optional<std::weak_ptr< UnfreezeObjectPhysics>> unfreezeObjectPhysicsWeak;




	void onForceLaunchEvent()
	{
		try
		{
			lockOrThrow(mccStateHookWeak, mccStateHook);
			if (!mccStateHook->isGameCurrentlyPlaying(mGame)) return;

			lockOrThrow(settingsWeak, settings);

			Datum datumToLaunch;
			if (settings->forceLaunchApplyToPlayer->GetValue())
			{
				lockOrThrow(getPlayerDatumWeak, getPlayerDatum);
				datumToLaunch = getPlayerDatum->getPlayerDatum();
			}
			else
			{
				datumToLaunch = settings->forceLaunchCustomObject->GetValue();
			}
			PLOG_INFO << "Launching object with datum: " << datumToLaunch;


			if (settings->forceLaunchManual->GetValue())
			{
				LaunchObjectAbsolute(settings->forceLaunchAbsoluteVec3->GetValue(), datumToLaunch);
			}
			else if (settings->forceLaunchForward->GetValue())
			{
				LaunchObjectRelativeToPlayer(settings->forceLaunchRelativeVec3->GetValue(), datumToLaunch);
			}
			else
			{
				throw HCMRuntimeException("forceLaunch settings were in an invalid state (neither enabled), try deleting HCMInternalConfig then restarting HCM");
			}

			if (settings->forceLaunchForward->GetValue() && settings->forceLaunchManual->GetValue())
			{
				throw HCMRuntimeException("forceLaunch settings were in an invalid state (both enabled), try deleting HCMInternalConfig then restarting HCM");
			}
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}

	}



public:
	ForceLaunchSimple(GameState game, IDIContainer& dicon)
		: mGame(game),
		settingsWeak(dicon.Resolve<SettingsStateAndEvents>()),
		mForceLaunchEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->forceLaunchEvent, [this]() {onForceLaunchEvent(); }),
		mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
		messagesGUIWeak(dicon.Resolve<IMessagesGUI>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
		getPlayerDatumWeak(resolveDependentCheat(GetPlayerDatum)),
		getObjectPhysicsWeak(resolveDependentCheat(GetObjectPhysics))
	{


		try
		{
			getPlayerViewAngleWeak = resolveDependentCheat(GetPlayerViewAngle);
		}
		catch (HCMInitException ex)
		{
			PLOG_ERROR << "Could not resolve getPlayerViewAngleWeak: " << ex.what();
		}

		//try
		//{
		//	unfreezeObjectPhysicsWeak = resolveDependentCheat(UnfreezeObjectPhysics);
		//}
		//catch (HCMInitException ex)
		//{
		//	PLOG_ERROR << "Could not resolve unfreezeObjectPhysicsWeak: " << ex.what();
		//}
	}

	void LaunchObjectAbsolute(SimpleMath::Vector3 coords, Datum datumToLaunch); // throws HCMRuntimes
	void LaunchObjectRelativeToPlayer(SimpleMath::Vector3 coords, Datum datumToLaunch); // throws HCMRuntimes
};




ForceLaunch::ForceLaunch(GameState gameImpl, IDIContainer& dicon)
{
	pimpl = std::make_unique<ForceLaunchSimple>(gameImpl, dicon);
}

ForceLaunch::~ForceLaunch() = default;




void ForceLaunchSimple::LaunchObjectAbsolute(SimpleMath::Vector3 coords, Datum datumToLaunch) // throws HCMRuntimes
{

	lockOrThrow(getObjectPhysicsWeak, getObjectPhysics);
	auto* currentVelocityMutable = getObjectPhysics->getObjectVelocityMutable(datumToLaunch);

	*currentVelocityMutable = coords;

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
	messagesGUI->addMessage(std::format("Launching with velocity {:.2f}, {:.2f}, {:.2f}", coords.x, coords.y, coords.z));
}


void ForceLaunchSimple::LaunchObjectRelativeToPlayer(SimpleMath::Vector3 coords, Datum datumToLaunch) // throws HCMRuntimes
{
	lockOrThrow(settingsWeak, settings);
	if (!getPlayerViewAngleWeak.has_value()) throw HCMRuntimeException("could not resolve getPlayerViewAngleWeak");

	lockOrThrow(getPlayerViewAngleWeak.value(), getPlayerViewAngle);

	auto playerViewAngle = getPlayerViewAngle->getPlayerViewAngle();


	lockOrThrow(getObjectPhysicsWeak, getObjectPhysics);
	auto* currentVelocityMutable = getObjectPhysics->getObjectVelocityMutable(datumToLaunch);

	// now let's do some math to figure out what the new velocity should be
	SimpleMath::Vector3 normalisedLookDirForward =
	{
		std::cos(playerViewAngle.y) * std::cos(playerViewAngle.x),
		std::cos(playerViewAngle.y) * std::sin(playerViewAngle.x),
		std::sin(playerViewAngle.y) 
	};

	normalisedLookDirForward.Normalize();

	if (settings->forceLaunchForwardIgnoreZ->GetValue())
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

	*currentVelocityMutable = *currentVelocityMutable
		+ (normalisedLookDirForward * coords.x) // forward component of user input 
		+ (normalisedLookDirRight * coords.y) // right component of user input
		+ (normalisedLookDirUp * coords.z) // up component of user input
		;

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
	messagesGUI->addMessage(std::format("Launching with velocity {:.2f}, {:.2f}, {:.2f}", coords.x, coords.y, coords.z));

}
