#include "pch.h"
//#include "CarrierBumpAnalyser.h"
//#include "SettingsStateAndEvents.h"
//#include "IMCCStateHook.h"
//#include "GetPlayerDatum.h"
//#include "IMakeOrGetCheat.h"
//#include "GetPlayerViewAngle.h"
//#include "GetObjectPhysics.h"
//#include "RuntimeExceptionHandler.h"
//#include "GameTickEventHook.h"
//#include "TogglePause.h"
//#include "ForceRevert.h"
//#include "time.h"
//#include "DirPathContainer.h"
//
//std::string directoryPath;
//
//// control values, very simple state machine
//enum class CurrentState
//{
//	init,
//	reset,
//	positionObjects,
//	updateChiefStance,
//	waitForExplosion,
//	measureTeleport,
//};
//
//// a single iteration thru the reset -> measureTeleport loop
//struct RunInformation
//{
//	SimpleMath::Vector3 chiefStartPosition;
//	SimpleMath::Vector3 carrierStartPosition;
//	SimpleMath::Vector3 startPositionDifference; // carrierStartPosition minus chiefStartPosition
//	SimpleMath::Vector3 chiefTeleportPosition; // position of chief after the carrier explosion
//	SimpleMath::Vector3 chiefTeleportDifference; // chiefTeleportPosition - chiefStartPosition
//	bool chiefCrouched;
//
//	// deserialiser
//	std::string serialise()
//	{
//		return std::format(
//			"chiefStartPosition: {}\n"
//			"carrierStartPosition: {}\n"
//			"startPositionDifference: {}\n"
//			"chiefTeleportPosition: {}\n"
//			"chiefTeleportDifference: {}\n"
//			"chiefCrouched: {}\n",
//			chiefStartPosition,
//			carrierStartPosition,
//			startPositionDifference,
//			chiefTeleportPosition,
//			chiefTeleportDifference,
//			chiefCrouched
//		);
//	}
//};
//
//
//// all runs
//struct SessionInformation
//{
//	std::vector<RunInformation> runInfoCollection;
//
//
//	~SessionInformation()
//	{
//		if (runInfoCollection.empty())
//		{
//			PLOG_DEBUG << "no runs in run collection, skipping serialisation";
//			return;
//		}
//
//		// dump info to file
//			// setup init time string
//		std::tm tm = {};
//		const time_t t = std::time(nullptr);
//		auto err = localtime_s(&tm, &t);
//		std::stringstream ss;
//		ss << std::put_time(&tm, "%Y-%m-%d %H-%M-%S") << std::endl;
//		std::string initTime = ss.str();
//
//		// setup log file 
//		std::string logFileName = std::format("CarrierBumpAnalysis_{}.txt", initTime);
//		std::string logFilePath = directoryPath + logFileName;
//		PLOG_DEBUG << "dumping carrier bump analysis to " << logFilePath;
//
//		// create/open the file
//		std::ofstream file(logFilePath);
//		// serialise all runs
//		for (unsigned short i = 0; auto& runInfo : runInfoCollection)
//		{
//			i++;
//			file << i << std::endl << runInfo.serialise() << std::endl << std::endl;
//		}
//
//	}
//};
//
//
//
//
//class CarrierBumpAnalyserImpl
//{
//private:
//
//	// stored info
//	SessionInformation sessionInformation;
//	RunInformation currentRun;
//
//	// state machine control values
//	CurrentState currentState = CurrentState::init;
//
//	// state machine control functions
//	void init()
//	{
//		sessionInformation ={};
//		
//		// TODO: 
//		// engage speedhack, maybe disable rendering if possible
//	}
//
//	void reset()
//	{
//		// TODO
//		// revert to checkpoint (the user will have to make an appropiate checkpoint beforehand. The checkpoint will need the carrier to already be landed and not moving, just about to explode, as well as chief standing perfectly still)
//
//		// reset current run info
//		currentRun = {};
//	}
//
//	void positionObjects()
//	{
//		// TODO
//		// place carrier and chief in some random location within a specific area near the door. log these chosen locations in current run
//	}
//
//	void updateChiefStance()
//	{
//		// TODO
//		// randomly pick whether chief should be crouched or standing, apply it, and log the choice in current run
//	}
//
//	bool waitForExplosion(int tickcount)
//	{
//		// TODO
//		// test if the explosion/potential-bump has happened yet. return true if it has NOT happened
//	}
//
//	void measureTeleport()
//	{
//		// TODO
//		// just check where chief is at now.
//		// and test if this new position is in/past the door or not (successful carrier bump)
//		// then log it to current run
//
//		// then add it to runInfoCollection
//		sessionInformation.runInfoCollection.push_back(currentRun);
//	}
//
//
//	
//
//
//
//
//
//
//	GameState mGame;
//
//	//callback
//	ScopedCallback<ToggleEvent> carrierBumpAnalyserToggleCallback;
//
//
//	// injected services
//	std::weak_ptr<IMCCStateHook> mccStateHookWeak;
//	std::weak_ptr<IMessagesGUI> messagesGUIWeak;
//	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;
//	std::weak_ptr<SettingsStateAndEvents> settingsWeak;
//	std::weak_ptr< GetPlayerDatum> getPlayerDatumWeak;
//	std::weak_ptr< GetObjectPhysics> getObjectPhysicsWeak;
//	std::weak_ptr<GameTickEventHook> gameTickEventHookWeak;
//	std::weak_ptr<PauseGame> pauseServiceWeak;
//	std::weak_ptr<ForceRevert> forceRevertWeak;
//
//
//	std::unique_ptr<ScopedServiceRequest> pauseOverrideRequest;
//	std::unique_ptr<ScopedCallback<eventpp::CallbackList<void(int)>>> mGameTickEventCallback;
//
//	void onCarrierBumpAnalyserToggle(bool& newStateValue)
//	{
//		try
//		{
//			lockOrThrow(mccStateHookWeak, mccStateHook);
//			lockOrThrow(gameTickEventHookWeak, gameTickEventHook);
//			lockOrThrow(settingsWeak, settings);
//			lockOrThrow(pauseServiceWeak, pauseService);
//			lockOrThrow(messagesGUIWeak, messagesGUI);
//
//			if (!mccStateHook->isGameCurrentlyPlaying(mGame))
//			{
//				throw HCMRuntimeException("wrong game");
//			}
//
//			// need to destroy old ones BEFORE constructing new ones
//			mGameTickEventCallback.reset();
//			pauseOverrideRequest.reset();
//
//			if (newStateValue == true)
//			{
//				mGameTickEventCallback = std::make_unique<ScopedCallback<eventpp::CallbackList<void(int)>>>(gameTickEventHook->getGameTickEvent(), [this](int i) {onGameTickEvent(i); });
//				// Reset state machine
//				currentState = CurrentState::init;
//			}
//			else
//			{
//				sessionInformation = {}; // force serialisation
//			}
//
//
//		}
//		catch (HCMRuntimeException ex)
//		{
//			runtimeExceptions->handleMessage(ex);
//		}
//	}
//
//	void onGameTickEvent(int tickCount)
//	{
//		switch (currentState)
//		{
//		case CurrentState::init:
//			init();
//			currentState = CurrentState::reset;
//			break;
//
//		case CurrentState::reset:
//			reset();
//			currentState = CurrentState::positionObjects;
//			break;
//
//		case CurrentState::positionObjects:
//			positionObjects();
//			currentState = CurrentState::updateChiefStance;
//			break;
//
//		case CurrentState::updateChiefStance:
//			updateChiefStance();
//			currentState = CurrentState::waitForExplosion;
//			break;
//
//		case CurrentState::waitForExplosion:
//			if (waitForExplosion(tickCount) == false)
//				currentState = CurrentState::measureTeleport;
//			break;
//
//		case CurrentState::measureTeleport:
//			updateChiefStance();
//			currentState = CurrentState::reset;
//			break;
//		}
//	}
//
//public:
//	CarrierBumpAnalyserImpl(GameState game, IDIContainer& dicon)
//		: 
//		mGame(game),
//		carrierBumpAnalyserToggleCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->carrierBumpAnalyserToggle->valueChangedEvent, [this](bool& n) { onCarrierBumpAnalyserToggle(n); }),
//		settingsWeak(dicon.Resolve<SettingsStateAndEvents>()),
//		mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
//		messagesGUIWeak(dicon.Resolve<IMessagesGUI>()),
//		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
//		getPlayerDatumWeak(resolveDependentCheat(GetPlayerDatum)),
//		getObjectPhysicsWeak(resolveDependentCheat(GetObjectPhysics)),
//		gameTickEventHookWeak(resolveDependentCheat(GameTickEventHook)),
//		forceRevertWeak(resolveDependentCheat(ForceRevert))
//	{
//		if (game != (GameState)GameState::Value::Halo1) throw HCMInitException("Only for h1");
//
//		directoryPath = dicon.Resolve<DirPathContainer>().lock().get()->dirPath;
//	}
//};
//
//
//
//
//
//
//
//
//
//
//CarrierBumpAnalyser::CarrierBumpAnalyser(GameState gameImpl, IDIContainer& dicon)
//{
//	pimpl = std::make_unique<CarrierBumpAnalyserImpl>(gameImpl, dicon);
//}
//
//CarrierBumpAnalyser::~CarrierBumpAnalyser() = default;