#include "pch.h"
#include "BumpTestingTool.h"
#include "SettingsStateAndEvents.h"
#include "RuntimeExceptionHandler.h"
#include "IMakeOrGetCheat.h"
#include <Random>
#include "ModuleHook.h"
#include "PointerDataStore.h"
#include "GetPlayerDatum.h"
#include "EntityPhysicsSimulator.h"
#include "GetObjectAddress.h"
#include "BumpPlayerStruct.h"
#include "BumpSetupGenerator.h"
#include "DirPathContainer.h"
#include "GlobalKill.h"
#include "BumpControlDefs.h"
#include "ObjectTableRange.h"
#include "GetCurrentBSP.h"
#include "ShieldGroupSetupGenerator.h"
#ifdef SHIELD_BUMPS
#include "BumpShieldStruct.h"
#endif


struct ShieldTimeReversalArray
{
	std::array<bool, ISB_INPUT_LENGTH + EXTRA_TICKS_AFTER_LAST_INPUT> shouldTestShieldOnThisTick;
	int lastTrueValue;
};

Datum shieldDatum = Datum();


//DirectX::BoundingBox expandedTeleportVolume;



float distanceToTeleportVolume(const std::vector<DirectX::BoundingBox>& checkVolumes, SimpleMath::Vector3 playerPos)
{
	assert(checkVolumes.size() > 0 && "checkVolumes was empty?!");

	float distToTeleVol;
	auto unitTeleDir = checkVolumes.begin()->Center - playerPos;
	unitTeleDir.Normalize();
	if (checkVolumes.begin()->Intersects(playerPos, unitTeleDir, distToTeleVol))
	{
		return distToTeleVol;
	}
	else
	{
		assert(false && "this should never be hit");
	}



}




float getRandomViewAngle()
{
	// random float from 0 to 2pi. 
	static std::default_random_engine e;
	static std::uniform_real_distribution<> dis(0.f, DirectX::XM_2PI);
	return dis(e);
}

// handles the fact that max might be "less" than min. (view angles wraparound from 2pi to 0)
float getViewAngleDistance(float min, float max)
{
	if (min < max) return max - min;
	if (min == max) return 0;

	max += DirectX::XM_2PI;
	return max - min;
}


bool viewAngleIsBetween(float test, float right, float left)
{
	if (left > right)
	{
		return (test >= right && test <= left);
	}
	else
	{
		return (test >= right || test <= left);
	}
}





class BumpTestingTool::BumpTestingToolImpl {
private:
	static inline BumpTestingToolImpl* instance = nullptr;

	// cached data
	Datum playerDatum;
	BumpPlayerObject* playerObject;
#ifdef SHIELD_BUMPS
	BumpShieldObject* shieldObject;
#endif


	// callbacks
	ScopedCallback<ToggleEvent> mToggleCallback;


	// injected services
	std::weak_ptr<GetPlayerDatum> getPlayerDatumWeak;
	std::weak_ptr<EntityPhysicsSimulator> entityPhysicsSimulatorWeak;
	std::weak_ptr<GetObjectAddress> getObjectAddressWeak;
	std::weak_ptr<ObjectTableRange> objectTableRangeWeak;
	std::weak_ptr<GetCurrentBSP> getCurrentBSPWeak;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;


	std::string logFilePath;
	std::string sortedlogfilepath;
	std::string interestinglogfilepath;

	std::vector<std::pair<std::string, float>> teleportsSortedByAngleRange;

	void onToggleEvent(bool& newValue)
	{
		try
		{
			if (newValue)
			{
				lockOrThrow(entityPhysicsSimulatorWeak, entityPhysicsSimulator);
				entityPhysicsSimulator->beginSimulation(begin);
			}


		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}

	}


	enum class testResultEnum {
		NoTele,
		PotentialTele,
		GoodTele
	};

	typedef std::pair<SimpleMath::Vector3, SimpleMath::Vector3> playerPosLastAndCurTick;

	typedef std::pair<testResultEnum, std::optional<playerPosLastAndCurTick>> testResult;


	struct LogTestResult
	{
		const BumpSetup& bumpSetup; const Nook& nook; testResult& tResult; float distanceTravelled; float fractionDistanceTravelled; float angle; SimpleMath::Vector3 telePos;
	};

	void logPotentialTele(const LogTestResult& logTestResult, long long setupcount)
	{
#ifdef LOG_POTENTIAL_TELES

		static std::hash<std::string> stringHasher;

		struct potentialTeleInfo { float angleOfLongest; float closestDistToTeleVol; float fractionDistanceTravelled; BumpSetup setup; Nook nook; SimpleMath::Vector3 telePos; std::string desc; };


		static std::unordered_map<BumpSetup, potentialTeleInfo, BumpSetupHasher> potentialTeleMap {};


		bool isNewOrBetter = potentialTeleMap.find(logTestResult.bumpSetup) == potentialTeleMap.end() || potentialTeleMap.at((logTestResult.bumpSetup)).closestDistToTeleVol > logTestResult.distanceTravelled;

		if (isNewOrBetter)
		{

			potentialTeleInfo currentTeleInfo;
			currentTeleInfo.angleOfLongest = logTestResult.angle;
			currentTeleInfo.telePos = logTestResult.telePos;
			currentTeleInfo.closestDistToTeleVol = logTestResult.distanceTravelled;
			currentTeleInfo.setup = logTestResult.bumpSetup;
			currentTeleInfo.nook = logTestResult.nook;
			currentTeleInfo.fractionDistanceTravelled = logTestResult.fractionDistanceTravelled;

			std::stringstream ss;
			ss << "Discovered teleport at angle " << std::setprecision(10) << currentTeleInfo.angleOfLongest << " to position " << vec3ToString(currentTeleInfo.telePos, 3) << std::endl;
			ss << std::format("{:.2f}", currentTeleInfo.fractionDistanceTravelled * 100.f) << "% Distance to tele volume: " << std::format("{:.2f}", currentTeleInfo.closestDistToTeleVol) << " units remaining" << std::endl;
			ss << "Nook: " << currentTeleInfo.nook.first << std::endl;
			ss << "SetupCount: " << setupcount << std::endl;
			ss << "Setup was: " << std::endl << currentTeleInfo.setup << std::endl;

			currentTeleInfo.desc = ss.str();
			potentialTeleMap.insert_or_assign((currentTeleInfo.setup), currentTeleInfo);



			std::vector < potentialTeleInfo*> potTeleSorted = {};
			for (auto& potTele : potentialTeleMap)
			{
				potTeleSorted.emplace_back(&potTele.second);
			}

			std::sort(potTeleSorted.begin(), potTeleSorted.end(), [](const auto& a, const auto& b) { return a->closestDistToTeleVol < b->closestDistToTeleVol; });

			std::ofstream interestingFile(interestinglogfilepath); // DON'T open in append mode (overwrite instead)
			for (auto& potTele : potTeleSorted)
			{
				interestingFile << potTele->desc;
			}


		}
#endif
	}




	static void begin(EntityPhysicsSimulator::AdvancePhysicsTicks* advancePhysics)
	{
		instance->main(advancePhysics);
	}



	void analyseTeleport(const Nook& nook, 
		BumpSetup& bumpSetup, 
		float angle, 
		EntityPhysicsSimulator::AdvancePhysicsTicks* advancePhysics, 
		const std::vector<DirectX::BoundingBox>& checkTeleVolume, 
		float discoveryAngle, 
		SimpleMath::Vector3 discoveryPosition, 
		std::array<float, 3> discoveryJitterMeasure, 
#ifdef SHIELD_BUMPS
		ShieldTimeReversalArray& shieldTimeReversalArray,
#endif  
		long long setupCount,
		std::optional<int> tickToTest = std::nullopt)
	{
		PLOG_DEBUG << "found a good teleport! analysing";
		// Test min and max angle of teleport
		float minAngle = angle;
		float maxAngle = angle;

		SimpleMath::Vector3 longestTravelPosition = discoveryPosition;
		float longestTravelDistance = SimpleMath::Vector3::Distance(discoveryPosition, nook.second);
		float longestTravelAngle = discoveryAngle;


		std::array<float, 3> maxJitterMeasure = discoveryJitterMeasure;

#define TEST_SMALLEST_NEXT

		std::array<float, 3> tempJitterMeasure;
#ifdef SHIELD_BUMPS
		while (testSetup(nook, bumpSetup, minAngle, advancePhysics, checkTeleVolume, shieldTimeReversalArray, &tempJitterMeasure, tickToTest).first == testResultEnum::GoodTele)
#else
		while (testSetup(nook, bumpSetup, minAngle, advancePhysics, checkTeleVolume, &tempJitterMeasure, tickToTest).first == testResultEnum::GoodTele)
#endif
		{
			for (int i = 0; i < tempJitterMeasure.size(); i++)
			{
				if (tempJitterMeasure.at(i) > maxJitterMeasure.at(i))
					maxJitterMeasure.at(i) = tempJitterMeasure.at(i);
			}

			float thisTravel = SimpleMath::Vector3::Distance(playerObject->position, nook.second);
			if (thisTravel > longestTravelDistance)
			{
				longestTravelDistance = thisTravel;
				longestTravelPosition = playerObject->position;
				longestTravelAngle = minAngle;
			}

#ifdef TEST_SMALLEST_NEXT
			minAngle = flooredModulo(std::nextafterf(minAngle, -1.f), DirectX::XM_2PI);
#else
			minAngle = flooredModulo(minAngle - 0.00001, DirectX::XM_2PI);
#endif
		}

#ifdef SHIELD_BUMPS
		while (testSetup(nook, bumpSetup, maxAngle, advancePhysics, checkTeleVolume, shieldTimeReversalArray, &tempJitterMeasure, tickToTest).first == testResultEnum::GoodTele)
#else
		while (testSetup(nook, bumpSetup, maxAngle, advancePhysics, checkTeleVolume, &tempJitterMeasure, tickToTest).first == testResultEnum::GoodTele)
#endif
		{
			for (int i = 0; i < tempJitterMeasure.size(); i++)
			{
				if (tempJitterMeasure.at(i) > maxJitterMeasure.at(i))
					maxJitterMeasure.at(i) = tempJitterMeasure.at(i);
			}

			float thisTravel = SimpleMath::Vector3::Distance(playerObject->position, nook.second);
			if (thisTravel > longestTravelDistance)
			{
				longestTravelDistance = thisTravel;
				longestTravelPosition = playerObject->position;
				longestTravelAngle = minAngle;
			}

#ifdef TEST_SMALLEST_NEXT
			maxAngle = flooredModulo(std::nextafterf(maxAngle, 10.f), DirectX::XM_2PI);
#else
			maxAngle = flooredModulo(maxAngle + 0.00001, DirectX::XM_2PI);
#endif
		}


		// Log teleport to file
		std::stringstream ss;
		ss << "Discovered teleport at angle " << std::setprecision(10) << discoveryAngle << " to position " << std::setprecision(10) << discoveryPosition << std::endl;
		ss << "Nook: " << nook.first << std::endl;
		ss << "Minimum angle: " << std::setprecision(10) << minAngle << std::endl;
		ss << "Maximum angle: " << std::setprecision(10) << maxAngle << std::endl;
		ss << "Midpoint angle: " << std::setprecision(10) << flooredModulo(minAngle + (getViewAngleDistance(minAngle, maxAngle) / 2.f), DirectX::XM_2PI) << std::endl;
		ss << "Angle range: " << std::setprecision(10) << getViewAngleDistance(minAngle, maxAngle) << std::endl;
		ss << "Longest travel position: " << std::setprecision(10) << longestTravelPosition << std::endl;
		ss << "Longest travel angle: " << std::setprecision(10) << longestTravelAngle << std::endl;
		ss << "Max jitter 0: " << maxJitterMeasure.at(0) << std::endl;
		ss << "Max jitter 1: " << maxJitterMeasure.at(0) << std::endl;
		ss << "Max jitter 2: " << maxJitterMeasure.at(0) << std::endl;
		ss << "Setup was: " << std::endl << bumpSetup << std::endl;
		ss << "shieldOnTick: " << (tickToTest.has_value() ? tickToTest.value() : -1) << std::endl;
		PLOG_DEBUG << ss.str();
		// add to unsorted log file


		std::ofstream unsortedFile(logFilePath, std::ios::app); // open in append mode
		unsortedFile << ss.str();

		// add to sorted vec. teleports are sorted by angle range (more angle range means the setup is less precise and thus better)
		teleportsSortedByAngleRange.push_back(std::pair<std::string, float>(ss.str(), getViewAngleDistance(minAngle, maxAngle)));
		std::sort(teleportsSortedByAngleRange.begin(), teleportsSortedByAngleRange.end(), [](const auto& a, const auto& b) { return a.second > b.second; });

		// NOT opened in append mode .
		std::ofstream sortedFile(sortedlogfilepath);
		sortedFile << "Setupcount: " << setupCount << std::endl;

#ifdef SHIELD_BUMPS
		sortedFile << "Shield Bumps. Which shield:  " << WHICH_SHIELD << std::endl;
#else
		sortedFile << "Regular Bumps. Is Tau:  ";
#ifdef TEST_TAU_NOOK
		sortedFile << "true" << std::endl;
#else
		sortedFile << "false" << std::endl;
#endif

#endif

		sortedFile << std::endl;
		for (auto& tele : teleportsSortedByAngleRange)
		{
			sortedFile << tele.first;
		}
	}


	bool isGoodTeleport(const std::vector<DirectX::BoundingBox>& checkTeleVolumes)
	{
		for (auto& vol : checkTeleVolumes)
		{
			if (vol.Contains(playerObject->position)) return true;
		}
		return false;
	}

	bool isPotentialTeleport(const SimpleMath::Vector3& posLast, const SimpleMath::Vector3& posCurr, const std::vector<DirectX::BoundingBox>& checkTeleVolumes)
	{
		if (SimpleMath::Vector2::Distance((SimpleMath::Vector2)posLast, (SimpleMath::Vector2)posCurr) < .5f) return false;

		auto unitDir = posCurr - posLast;
		unitDir.Normalize();

		float dontCare;

		for (auto& vol : goodTeleportVolumeExpanded) // naughty, breaks tests
		{
			if (vol.Intersects(posCurr, unitDir, dontCare)) return true;
		}

		return false;
	}





	testResult testSetup(Nook nook, 
		BumpSetup& bumpSetup, 
		float angle, 
		EntityPhysicsSimulator::AdvancePhysicsTicks* advancePhysics, 
		const std::vector<DirectX::BoundingBox>& checkTeleVolume,
#ifdef SHIELD_BUMPS
	 ShieldTimeReversalArray& shieldTimeReveralArray, 
#endif  
		std::array<float, 3>* outJitterMeasure = nullptr,
		std::optional<int> onlyCheckTick = std::nullopt)
	{



		std::optional<std::pair<std::pair<testResultEnum, playerPosLastAndCurTick>, int>> bestOut;

#ifndef SHIELD_TIME_REVERSAL
		for (int tickToTurnShieldOn = 0; tickToTurnShieldOn < shieldTimeReveralArray.lastTrueValue; tickToTurnShieldOn++)
		{
			if (shieldTimeReveralArray.shouldTestShieldOnThisTick.at(tickToTurnShieldOn) == false)
				continue;

			bumpSetup.shieldOnTickLastResult = tickToTurnShieldOn;
#endif


#ifdef SHIELD_BUMPS
shieldObject->setShieldState(false);
#endif

	playerObject->velocity = SimpleMath::Vector3::Zero;
	playerObject->position = safeStandingSpot;
	playerObject->setViewAngle(angle);
	playerObject->crouchPercent = 0.f;
	playerObject->setInput(Input::None); 
	advancePhys(1);
	playerObject->setInput(Input::None);

		playerObject->velocity = SimpleMath::Vector3::Zero;
		playerObject->position = nook.second;
		playerObject->crouchPercent = 0.f;
		advancePhys(1);




		// jitter test
		auto testStablePosition = playerObject->position;
		advancePhys(1);
		if (playerObject->position != testStablePosition)
		{

			float distanceToExpected = SimpleMath::Vector3::Distance(testStablePosition, playerObject->position);

#ifndef IGNORE_JITTER
			PLOG_ERROR << "unstable position during setup: ";
			PLOG_ERROR << "expected position: " << testStablePosition;
			PLOG_ERROR << "observed position: " << playerObject->position;
			PLOG_ERROR << "distance: " << distanceToExpected;
#endif

			if (outJitterMeasure)
				(*outJitterMeasure)[0] = distanceToExpected;

		}

		// pre setups (standing only)
		for (auto& inpPair : bumpSetup.setupInputs)
		{
			if (contains(inpPair.first.first, Input::Crouch))
			{
				playerObject->crouchPercent = 1.f;
				playerObject->setInput(Input::Crouch);
				advancePhys(1);
				playerObject->setInput(Input::Crouch);
				advancePhys(1);
				playerObject->setInput(Input::Crouch);
				advancePhys(1);
			}
			else
			{
				playerObject->crouchPercent = 0.f;
				playerObject->setInput(Input::None);
				advancePhys(1);
				playerObject->setInput(Input::None);
				advancePhys(1);
				playerObject->setInput(Input::None);
				advancePhys(1);
			}


			for (int tick = 0; tick < inpPair.first.second; tick++)
			{
				playerObject->setInput(inpPair.first.first);
				advancePhys(1);
			}

			playerObject->setInput(Input::None);
			for (int i = 0; i < (inpPair.first.second + 3); i++)
			{
				advancePhys(1);
			}
		}

		//auto positionBeforeIdleStanceAdjust = playerObject->position;

		// set idle input
		setInputs(bumpSetup.goIdleInput);
		if (contains(bumpSetup.goIdleInput, Input::Crouch))
			playerObject->crouchPercent = 1.f;
		else
			playerObject->crouchPercent = 0.f;

		advancePhys(1);
		setInputs(bumpSetup.goIdleInput);
		advancePhys(1);

		//if (playerObject->position != positionBeforeIdleStanceAdjust)
		//{
		//	LOG_ONCE(PLOG_DEBUG << "position after stance adjustment did not match, abandoning setup");
		//	return { testResultEnum::NoTele, std::nullopt };
		//}


#ifdef SHIELD_BUMPS
#ifdef SHIELD_TIME_REVERSAL





		//BumpPlayerObject playerStatePrevious = *playerObject;
		// go time!
		if (onlyCheckTick.has_value())
		{
			for (int goTickCount = 0; goTickCount < bumpSetup.goInputs.size(); goTickCount++)
			{

				auto playerPositionLastTick = playerObject->position;
				auto currentInput = bumpSetup.goInputs.at(goTickCount);
				playerObject->setInput(currentInput);
				bumpSetup.shieldOnTickLastResult = goTickCount;

				
				if (onlyCheckTick.value() == goTickCount)
				{
					shieldObject->setShieldState(true);
					advancePhys(1);
					if (isGoodTeleport(checkTeleVolume))
					{
						return { testResultEnum::GoodTele, playerPosLastAndCurTick{playerPositionLastTick, playerObject->position } };
					}
					else if (isPotentialTeleport(playerPositionLastTick, playerObject->position, checkTeleVolume))
					{
						return { testResultEnum::PotentialTele, playerPosLastAndCurTick{ playerPositionLastTick, playerObject->position } };

					}
					else
					{
						return { testResultEnum::NoTele, std::nullopt };
					}
				}

				advancePhys(1);

			}
		}
		else
		{
			for (int goTickCount = 0; goTickCount < bumpSetup.goInputs.size(); goTickCount++)
			{

				auto playerPositionLastTick = playerObject->position;
				auto currentInput = bumpSetup.goInputs.at(goTickCount);
				playerObject->setInput(currentInput);
				bumpSetup.shieldOnTickLastResult = goTickCount;

				if (shieldTimeReveralArray.lastTrueValue < goTickCount)
				{
					//PLOG_DEBUG << "eh at ";
					//PLOG_DEBUG << "shieldTimeReveralArray.lastTrueValue " << shieldTimeReveralArray.lastTrueValue;
					//PLOG_DEBUG << "goTickCount " << goTickCount;
					if (bestOut.has_value())
					{
						bumpSetup.shieldOnTickLastResult = bestOut.value().second;
						return bestOut.value().first;
					}
					else
						return { testResultEnum::NoTele, std::nullopt };
				}





				if (shieldTimeReveralArray.shouldTestShieldOnThisTick.at(goTickCount))
				{
					BumpPlayerObject prevState = *playerObject;

					shieldObject->setShieldState(true);
					advancePhys(1);

					if (isGoodTeleport(checkTeleVolume))
					{
						return { testResultEnum::GoodTele, playerPosLastAndCurTick{playerPositionLastTick, playerObject->position } };
					}
					else if (isPotentialTeleport(playerPositionLastTick, playerObject->position, checkTeleVolume))
					{
						if (bestOut.has_value())
						{
							LOG_ONCE(PLOG_DEBUG << "bestout already had value!");
							if (SimpleMath::Vector3::Distance(playerPositionLastTick, playerObject->position) > SimpleMath::Vector3::Distance(bestOut.value().first.second.first, bestOut.value().first.second.second))
								bestOut = { { testResultEnum::PotentialTele, playerPosLastAndCurTick{ playerPositionLastTick, playerObject->position }}, goTickCount };
							else
								LOG_ONCE(PLOG_DEBUG << "current pot is worse tho");
						}
						else
						{
							bestOut = { { testResultEnum::PotentialTele, playerPosLastAndCurTick{ playerPositionLastTick, playerObject->position }}, goTickCount };
						}
					}



					shieldObject->setShieldState(false);
					*playerObject = prevState;


					//playerObject->position = playerPositionLastTick;
					//playerObject->velocity = prevVel;
					//playerObject->crouchPercent = prevCrc;
					//advancePhys(1);



				}


				advancePhys(1);


				if (SimpleMath::Vector3::Distance(nook.second, playerObject->position) > 3.f)
				{
					LOG_ONCE(PLOG_DEBUG << "too far away from nook!!!");
					if (bestOut.has_value())
					{
						bumpSetup.shieldOnTickLastResult = bestOut.value().second;
						return bestOut.value().first;
					}
					else
						return { testResultEnum::NoTele, std::nullopt };
				}






			}

		}
		if (bestOut.has_value())
		{
			bumpSetup.shieldOnTickLastResult = bestOut.value().second;
			return bestOut.value().first;
		}
		else
			return { testResultEnum::NoTele, std::nullopt };

#else

			// go time!
			SimpleMath::Vector3 playerPositionLastTick = playerObject->position;
			for (int goTickCount = 0; goTickCount < bumpSetup.goInputs.size(); goTickCount++)
			{

				auto currentInput = bumpSetup.goInputs.at(goTickCount);
				playerObject->setInput(currentInput);








				if (goTickCount == tickToTurnShieldOn)
				{

					shieldObject->setShieldState(true);
					advancePhys(1);

					if (isGoodTeleport(checkTeleVolume))
					{
						return { testResultEnum::GoodTele, playerPosLastAndCurTick{playerPositionLastTick, playerObject->position } };
					}
					else if (isPotentialTeleport(playerPositionLastTick, playerObject->position, checkTeleVolume))
					{
						//PLOG_DEBUG << "potentiallllll1 on t " << goTickCount;
						if (bestOut.has_value())
						{
							PLOG_DEBUG << "bestout already had value!";
							if (SimpleMath::Vector3::Distance(playerPositionLastTick, playerObject->position) > SimpleMath::Vector3::Distance(bestOut.value().first.second.first, bestOut.value().first.second.second))
								bestOut = { { testResultEnum::PotentialTele, playerPosLastAndCurTick{ playerPositionLastTick, playerObject->position }}, goTickCount };
							else
								PLOG_DEBUG << "current pot is worse tho";
						}
						else
						{
							bestOut = { { testResultEnum::PotentialTele, playerPosLastAndCurTick{ playerPositionLastTick, playerObject->position }}, goTickCount };
						}

					}

					shieldObject->setShieldState(false);
					break;

				}
				else
				{
					advancePhys(1);
				}







				if (SimpleMath::Vector3::Distance(nook.second, playerObject->position) > 3.f)
				{
					LOG_ONCE(PLOG_DEBUG << "too far away from nook!!!");
					break;
				}





				playerPositionLastTick = playerObject->position;


			}
		}

		if (bestOut.has_value())
		{
			bumpSetup.shieldOnTickLastResult = bestOut.value().second;
			return bestOut.value().first;
		}
		else
			return { testResultEnum::NoTele, std::nullopt };
#endif
#else
				// go time!
		SimpleMath::Vector3 playerPositionLastTick = playerObject->position;
		int totalTickCount = 0;
		for (auto& goInput : bumpSetup.goInputs)
		{

			playerObject->setInput(goInput);

			advancePhys(1);

			if (isGoodTeleport(checkTeleVolume))
			{
				return { testResultEnum::GoodTele, playerPosLastAndCurTick{playerPositionLastTick, playerObject->position } };
			}

			if (isPotentialTeleport(playerPositionLastTick, playerObject->position, checkTeleVolume))
			{
				return { testResultEnum::PotentialTele, playerPosLastAndCurTick{playerPositionLastTick, playerObject->position } };
			}

			if (SimpleMath::Vector3::Distance(nook.second, playerObject->position) > 3.f)
			{
				LOG_ONCE(PLOG_DEBUG << "too far away from nook!!!");
				return { testResultEnum::NoTele, std::nullopt };
			}


			playerPositionLastTick = playerObject->position;
			totalTickCount++;

		}
#endif

		return { testResultEnum::NoTele, std::nullopt };



	}

	void main(EntityPhysicsSimulator::AdvancePhysicsTicks* advancePhysics)
	{
		try
		{
			LOG_ONCE(PLOG_DEBUG << "main start");

			static bool forceShutdown = false;
			forceShutdown = false;

			// cache a bunch of stuff
			lockOrThrow(getPlayerDatumWeak, getPlayerDatum);
			playerDatum = getPlayerDatum->getPlayerDatum();

			lockOrThrow(getObjectAddressWeak, getObjectAddress);
			playerObject = (BumpPlayerObject*)getObjectAddress->getObjectAddress(playerDatum);

			lockOrThrow(getCurrentBSPWeak, getCurrentBSP);


#ifdef SHIELD_BUMPS

			//shieldDatum = 0xE26F0000;
			//shieldObject = (BumpShieldObject*)getObjectAddress->getObjectAddress(shieldDatum);

			shieldObject = nullptr;
			shieldDatum = Datum();

			lockOrThrow(objectTableRangeWeak, objectTableRange)
				auto objectTable = objectTableRange->getObjectTableRange();
			PLOG_DEBUG << "objectTable size: " << objectTable.size();
			PLOG_DEBUG << "Looking for shield object at position: " << vec3ToString(shieldPosition);
			for (auto& objectInfo : objectTable)
			{
				if (objectInfo.objectType != CommonObjectType::Scenery)
					continue;

				struct sceneryPosition
				{
					float pad[6];
					SimpleMath::Vector3 position;
				};

				SimpleMath::Vector3 expectedPosition = shieldPosition;
				
				if (IsBadReadPtr((void*)objectInfo.objectAddress, 4))
				{
					PLOG_ERROR << "bad read ptr goin thru object table " << std::hex << objectInfo.objectAddress;
					continue;
				}

				sceneryPosition* ourObject = (sceneryPosition*)objectInfo.objectAddress;

				PLOG_DEBUG << "read position: " << ourObject->position;

				if (SimpleMath::Vector3::Distance(ourObject->position, expectedPosition) < 0.2f)
				{
					// wahoo
					PLOG_DEBUG << "Found shield object";
					shieldDatum = objectInfo.objectDatum;
					EntityPhysicsSimulator::setShieldDatum(shieldDatum);
					PLOG_DEBUG << "Shield datum: " << shieldDatum;
					shieldObject = (BumpShieldObject*)ourObject;
					PLOG_DEBUG << "Shield object: " << std::hex << objectInfo.objectAddress;
					shieldPosition = ourObject->position;
					break;
				}

			}

			if (!shieldObject)
			{
				HCMRuntimeException ex("Could not find shieldObject!!!");
				runtimeExceptions->handleMessage(ex);
				return;
			}

#endif

			LOG_ONCE_CAPTURE(PLOG_DEBUG << "playerObject: " << std::hex << p, p = (uintptr_t)playerObject);

#ifdef TEST_PHYSICS_SIMULATOR

			PLOG_DEBUG << "TEST_PHYSICS_SIMULATOR";
			setInputs(InpMask(Input::Forward));
			advancePhys(1);


			while (true)
			{
				if (GlobalKill::isKillSet())
					return;

				if ((GetKeyState('6') & 0x8000) && (GetKeyState('9') & 0x8000)) // nice
					return;


				while (GetKeyState('7') & 0x8000)
				{

					PLOG_DEBUG << "advancing";
					setInputs(InpMask(Input::Forward));
					advancePhys(1);
					Sleep(200);
				}
			}
#endif



#ifdef TEST_BUMP_GENERATOR
			PLOG_DEBUG << "TEST_BUMP_GENERATOR ";
			BumpSetupGenerator bumpSetupGenTest(0);
			while (true)
			{
				if (GlobalKill::isKillSet())
					return;;

				if ((GetKeyState('6') & 0x8000) && (GetKeyState('9') & 0x8000)) // nice
					return;

				static int count = 0;
				auto setup = bumpSetupGenTest.getSetup();
				PLOG_DEBUG << "Setup Count: " << count;
				PLOG_DEBUG << setup;
				count++;
			}
#endif


#ifdef TEST_PERFORMANCE
			PLOG_DEBUG << "TEST_PERFORMANCE";
			auto startTimePerf = std::chrono::high_resolution_clock::now();
			static long long count = 0;
			count = 0;
			while (true)
			{
				if (GlobalKill::isKillSet())
					return;

				for (int i = 0; i < 1000; i++)
				{
					advancePhys(1);
					count++;
				}


				if ((GetKeyState('6') & 0x8000) && (GetKeyState('9') & 0x8000)) // nice
				{
					auto endTimePerf = std::chrono::high_resolution_clock::now();
					auto ms_intPerf = std::chrono::duration_cast<std::chrono::milliseconds>(endTimePerf - startTimePerf).count();

					PLOG_DEBUG << "performed " << count << " ticks in " << ms_intPerf << " milliseconds";
					PLOG_DEBUG << "that's " << (((float)count / ((float)ms_intPerf / 1000.f)) / 30.f) << "x gamespeed";
					return;
				}



			}

#endif

#ifdef TEST_DIRECTION_CULL

			for (int i = 0; i < 30; i++)
			{
				playerObject->setInput(Input::Forward);
				advancePhys(1);
			}

			auto leftDir = goodTeleportVolumeLeft - playerObject->position;
			auto rightDir = goodTeleportVolumeRight - playerObject->position;

			auto leftDot = playerObject->velocity.Dot(leftDir);
			auto rightDot = playerObject->velocity.Dot(rightDir);

			PLOG_DEBUG << "leftdot: " << leftDot;
			PLOG_DEBUG << "rightdot: " << rightDot;

			return;


#endif


#ifdef TEST_TIME_REVERSAL
			{
				shieldObject->setShieldState(false);
				auto nook = *nooksToTest.begin();
				float angle = 0.9f;

				playerObject->velocity = SimpleMath::Vector3::Zero;
				playerObject->position = safeStandingSpot;
				playerObject->setViewAngle(angle);
				playerObject->crouchPercent = 0.f;
				playerObject->setInput(Input::None); 
				advancePhys(1);


				playerObject->velocity = SimpleMath::Vector3::Zero;
				playerObject->position = nook.second;
				playerObject->crouchPercent = 0.f;
				advancePhys(1);

				playerObject->setInput(Input::Forward); 

				for (int i = 0; i < 15; i++)
				{
					advancePhys(1);
					PLOG_DEBUG << "tickA: " << i << ", pos: " << vec3ToString(playerObject->position);
				}

				auto expectedPosition = playerObject->position;




				playerObject->velocity = SimpleMath::Vector3::Zero;
				playerObject->position = safeStandingSpot;
				playerObject->setViewAngle(angle);
				playerObject->crouchPercent = 0.f;
				playerObject->setInput(Input::None); 
				advancePhys(1);


				playerObject->velocity = SimpleMath::Vector3::Zero;
				playerObject->position = nook.second;
				playerObject->crouchPercent = 0.f;
				advancePhys(1);

				playerObject->setInput(Input::Forward); 

				for (int i = 0; i < 15; i++)
				{
					if (i == 10)
					{
						auto prevState = *playerObject;
						shieldObject->setShieldState(true);
						advancePhys(1);
						shieldObject->setShieldState(false);
						*playerObject = prevState;
						advancePhys(1);
					}
					else
					{
						advancePhys(1);
					}
					PLOG_DEBUG << "tickB: " << i << ", pos: " << vec3ToString(playerObject->position);
					
				}

				auto observedPosition = playerObject->position;

				if (expectedPosition != observedPosition)
				{
					throw HCMRuntimeException(std::format("TEST_TIME_REVERSAL failed!\n expected position: {}\n observed position: {}\n distance: ",
						vec3ToString(expectedPosition),
						vec3ToString(observedPosition),
						SimpleMath::Vector3::Distance(expectedPosition, observedPosition)));
				}
				else
				{
					PLOG_INFO << "TEST_TIME_REVERSAL passed!";
				}

				// just pick the first nook
				// do a run without any time reversal, store the players position at each tick.

				// then reset and do it with time reversal, after each advance check that it matches the stored position for that tick
			}
#endif



			LOG_ONCE_CAPTURE(PLOG_DEBUG << "player crouch% ptr: " << p, p = &playerObject->crouchPercent);

			BumpSetupGenerator bumpSetupGen(START_SETUP_COUNT);

			PLOG_DEBUG << "Testing known cases";
			for (auto& testingSetup : bumpSetupGen.getTestCases())
			{

				ShieldTimeReversalArray shieldTimeReversalArray;
				std::ranges::fill(shieldTimeReversalArray.shouldTestShieldOnThisTick, true);
				shieldTimeReversalArray.lastTrueValue = shieldTimeReversalArray.shouldTestShieldOnThisTick.size();


#ifdef SHIELD_BUMPS
				this->testSetup(std::get<Nook>(testingSetup), std::get<BumpSetup>(testingSetup), std::get<float>(testingSetup), advancePhysics, std::get < std::vector<DirectX::BoundingBox> >(testingSetup), shieldTimeReversalArray);
#else
				this->testSetup(std::get<Nook>(testingSetup), std::get<BumpSetup>(testingSetup), std::get<float>(testingSetup), advancePhysics, std::get < std::vector<DirectX::BoundingBox> >(testingSetup));
#endif
				

				
				auto expectedPosition = std::get<SimpleMath::Vector3>(testingSetup);
				float distanceToExpected = SimpleMath::Vector3::Distance(expectedPosition, playerObject->position);


				std::stringstream ss;
				ss << "Test case failure, name: " << std::get<Nook>(testingSetup).first;
				ss << ", distance: " << distanceToExpected;
				ss << "\n expected position: " << std::get<SimpleMath::Vector3>(testingSetup);
				ss << "\nactual position: " << playerObject->position;
				if (distanceToExpected > 1.f)
				{
					PLOG_ERROR << ss.str();
					HCMRuntimeException ex(ss.str());
					runtimeExceptions->handleMessage(ex);
				}
				else
				{
					PLOG_DEBUG << "Successful test on known case at nook: " << std::get<Nook>(testingSetup).first;;
				}
			}



			LOG_ONCE(PLOG_DEBUG << "main entering automated test loop");

			auto startTime = std::chrono::high_resolution_clock::now();
			while (true)
			{

				static int totalRunSetups = 0;
				totalRunSetups++;

				// shutdown check
				if (GlobalKill::isKillSet())
					return;

				if (GetKeyState('6') & 0x8000 && GetKeyState('9') & 0x8000) // nice
					return;


				PLOG_DEBUG << "Generating new setup";
				auto setupPlain = bumpSetupGen.getSetup();
				PLOG_DEBUG << "Generated setupPlain: " << setupPlain;




				for (auto& nook : nooksToTest)
				{



					// shutdown check
					if (GlobalKill::isKillSet())
						return;

					if (GetKeyState('6') & 0x8000 && GetKeyState('9') & 0x8000) // nice
						return;

#ifdef SHIELD_BUMPS
					auto setupVar = cullISBGroupSetup(nook, setupPlain, advancePhysics, playerObject, shieldObject, playerDatum);
#else
					auto setupVar = cullISBSetup(nook, setupPlain, advancePhysics, playerObject, playerDatum);
#endif

						// shutdown check
						if (GlobalKill::isKillSet())
							return;

						if (GetKeyState('6') & 0x8000 && GetKeyState('9') & 0x8000) // nice
							return;




							// how far does the teleport need to go?
							// draw a line from nook to center of teleport volume. Where it intersects the volume, take the distance from that point to the nook.
							float requiredDistance = distanceToTeleportVolume(goodTeleportVolume, nook.second);

							float broadStepAngle = pixelAngle * DEPTH_ANGLE_1;

							static std::default_random_engine e;
							static std::uniform_real_distribution<> dis(0.f, broadStepAngle);
							float randomVAoffset = dis(e);
#ifndef RANDOM_VA_OFFSET
							randomVAoffset = 0.f;
#endif

							for (float broadTestingAngle = randomVAoffset; broadTestingAngle < (DirectX::XM_2PI + randomVAoffset); broadTestingAngle += broadStepAngle)
							{
#ifndef SHIELD_BUMPS
								bool isNotCulledAngle = false;
								for (auto& angleTick : setupVar.leftRightAngleRanges)
								{
									for (auto& anglePair : angleTick)
									{
										if (viewAngleIsBetween(broadTestingAngle, anglePair.second, anglePair.first))
										{
											isNotCulledAngle = true;
										}

										if (isNotCulledAngle) break;
									}
									if (isNotCulledAngle) break;
								}
	
								if (isNotCulledAngle == false) continue;

#else
								ShieldTimeReversalArray shieldTimeReversalArray;
								std::ranges::fill(shieldTimeReversalArray.shouldTestShieldOnThisTick, false);

								//PLOG_DEBUG << "setupVar.leftRightAngleRanges.size() " << setupVar.leftRightAngleRanges.size();
								//PLOG_DEBUG << "setupVar.goInputs.size() " << setupVar.goInputs.size();
								//PLOG_DEBUG << "shieldTimeReversalArray.shouldTestShieldOnThisTick.size() " << shieldTimeReversalArray.shouldTestShieldOnThisTick.size();

								assert(setupVar.leftRightAngleRanges.size() == setupVar.goInputs.size());
								assert(shieldTimeReversalArray.shouldTestShieldOnThisTick.size() == setupVar.goInputs.size());

								int furthestValidTick = -1;
								for (int i = 0; i < setupVar.leftRightAngleRanges.size(); i++)
								{
									auto& angleVecs = setupVar.leftRightAngleRanges.at(i);
									bool isValidBroadAngle = false;
									for (auto& angleRange : angleVecs)
									{
										if (viewAngleIsBetween(broadTestingAngle, angleRange.first, angleRange.second))
										{
											isValidBroadAngle = true;
											break;
										}
									}
									if (i >= shieldTimeReversalArray.shouldTestShieldOnThisTick.size())
									{
										PLOG_ERROR << "oooops";
										break;
									}


									if (isValidBroadAngle || broadTestingAngle >= DirectX::XM_2PI)
									{
										shieldTimeReversalArray.shouldTestShieldOnThisTick.at(i) = true;
										furthestValidTick = i;
									}
									else
									{
										shieldTimeReversalArray.shouldTestShieldOnThisTick.at(i) = false;
									}
										
								}
								shieldTimeReversalArray.lastTrueValue = furthestValidTick;
								//PLOG_DEBUG << "ticks to test: " << std::ranges::count_if(shieldTimeReversalArray.shouldTestShieldOnThisTick, [](auto& a) {return a; });
								//PLOG_DEBUG << "furthest tick to test: " << furthestValidTick;
								//auto& a = shieldTimeReversalArray.shouldTestShieldOnThisTick;
								//auto it = std::find_if(a.rbegin(), a.rend(), [](bool b) {return b; });
								//if (it != a.rend())
								//{
								//	shieldTimeReversalArray.lastTrueValue = std::distance(it, a.rend());
								//}
								//else
								//{
								//	shieldTimeReversalArray.lastTrueValue = -1;
								//}
									

#endif



								




								std::array<float, 3> jitterMeasure = { 0.f, 0.f, 0.f };

#ifdef SHIELD_BUMPS
								auto broadTestReturn = testSetup(nook, setupVar, broadTestingAngle, advancePhysics, goodTeleportVolume, shieldTimeReversalArray, &jitterMeasure);
#else
								auto broadTestReturn = testSetup(nook, setupVar, broadTestingAngle, advancePhysics, goodTeleportVolume, &jitterMeasure);
#endif
								if (broadTestReturn.first == testResultEnum::PotentialTele)
								{
									int tickToNarrowOn = setupVar.shieldOnTickLastResult;

									{
									// how far did the teleport  go?
									// draw a line from teleport position to center of teleport volume. Where it intersects the volume, take the distance from that point to the teleport position..
									float distToTeleVolBroad = distanceToTeleportVolume(goodTeleportVolume, broadTestReturn.second.value().second);

									PLOG_DEBUG << "potential tele found at angle " << std::format("{:.6f}", broadTestingAngle) << " to pos: " << vec3ToString(broadTestReturn.second.value().second) << "; dist%: " << ((1.f - (distToTeleVolBroad / requiredDistance)) * 100.f) << " checking narrow angles.";
#ifdef SHIELD_BUMPS
										PLOG_DEBUG << "shieldOnTick: " << setupVar.shieldOnTickLastResult;
#endif
									}

									bool preciseTestPerformed = false;
									float narrowStepAngle = pixelAngle * DEPTH_ANGLE_2;
									float lastNarrowTestingAngle = -1.f;
									for (float narrowTestingAngle = broadTestingAngle - broadStepAngle; narrowTestingAngle < broadTestingAngle + broadStepAngle; narrowTestingAngle += narrowStepAngle)
									{
										if (lastNarrowTestingAngle == narrowTestingAngle)
										{
											narrowTestingAngle = std::nextafterf(narrowTestingAngle, broadTestingAngle + (broadStepAngle / 2.f));
										}
										else
										{
											lastNarrowTestingAngle = narrowTestingAngle;
										}


										if (preciseTestPerformed) // all subangles were checked
											break;

#ifdef SHIELD_BUMPS
										auto narrowTestReturn = testSetup(nook, setupVar, narrowTestingAngle, advancePhysics, goodTeleportVolume, shieldTimeReversalArray, &jitterMeasure, tickToNarrowOn);
#else
										auto narrowTestReturn = testSetup(nook, setupVar, narrowTestingAngle, advancePhysics, goodTeleportVolume, &jitterMeasure, tickToNarrowOn);
#endif


										if (narrowTestReturn.first == testResultEnum::NoTele)
											continue;

										if (narrowTestReturn.first == testResultEnum::GoodTele)
										{
#ifdef SHIELD_BUMPS
											analyseTeleport(nook, setupVar, narrowTestingAngle, advancePhysics, goodTeleportVolume, narrowTestingAngle, narrowTestReturn.second.value().second, jitterMeasure, shieldTimeReversalArray, bumpSetupGen.getSetupCount(), tickToNarrowOn);
#else
											analyseTeleport(nook, setupVar, narrowTestingAngle, advancePhysics, goodTeleportVolume, narrowTestingAngle, narrowTestReturn.second.value().second, jitterMeasure, bumpSetupGen.getSetupCount(), tickToNarrowOn);
#endif
											break;
										}

										// how far did the teleport  go?
										// draw a line from teleport position to center of teleport volume. Where it intersects the volume, take the distance from that point to the teleport position..
										float distToTeleVol = distanceToTeleportVolume(goodTeleportVolume, narrowTestReturn.second.value().second);

										// do we need to check all subangles?
										float fractionDistanceTravelled = (1.f - (distToTeleVol / requiredDistance));
										preciseTestPerformed = fractionDistanceTravelled > TEST_EVERY_ANGLE_WHEN_DISTANCE_FRACTION_OVER;


										LogTestResult logTestResult{ setupVar, nook, narrowTestReturn, distToTeleVol, fractionDistanceTravelled, narrowTestingAngle, narrowTestReturn.second.value().second };


										if (preciseTestPerformed)
										{
	



											PLOG_DEBUG << "performing precise test on tele: " << std::format("{:.6f}", narrowTestingAngle) << " to pos: " << vec3ToString(narrowTestReturn.second.value().second) << "; dist%: " << (fractionDistanceTravelled * 100.f) << " checking precise angles.";
#ifdef SHIELD_BUMPS
											PLOG_DEBUG << "shieldOnTick: " << setupVar.shieldOnTickLastResult;
#endif

											bool reachedMinThresholdAngle = false;
											float minAngle = narrowTestingAngle;
											while (reachedMinThresholdAngle == false)
											{
												
#ifdef SHIELD_BUMPS
												auto preciseTestReturn = testSetup(nook, setupVar, minAngle, advancePhysics, goodTeleportVolume, shieldTimeReversalArray, &jitterMeasure, tickToNarrowOn);
#else
												auto preciseTestReturn = testSetup(nook, setupVar, minAngle, advancePhysics, goodTeleportVolume, &jitterMeasure, tickToNarrowOn);
#endif

												if (preciseTestReturn.first == testResultEnum::GoodTele)
												{
													reachedMinThresholdAngle = true;
#ifdef SHIELD_BUMPS
													analyseTeleport(nook, setupVar, minAngle, advancePhysics, goodTeleportVolume, minAngle, preciseTestReturn.second.value().second, jitterMeasure, shieldTimeReversalArray, bumpSetupGen.getSetupCount(), tickToNarrowOn);
#else
													analyseTeleport(nook, setupVar, minAngle, advancePhysics, goodTeleportVolume, minAngle, preciseTestReturn.second.value().second, jitterMeasure, bumpSetupGen.getSetupCount(), tickToNarrowOn);
#endif
												}

												if (preciseTestReturn.first == testResultEnum::PotentialTele)
												{
													// calculate distance travelled and check if it is still over threshold.
													// if NOT, set reachedMinThresholdAngle to true.

													float distToTeleVolPrecise = distanceToTeleportVolume(goodTeleportVolume, preciseTestReturn.second.value().second);

													float precfractionDistanceTravelled = (1.f - (distToTeleVolPrecise / requiredDistance));

													if (precfractionDistanceTravelled > TEST_EVERY_ANGLE_WHEN_DISTANCE_FRACTION_OVER)
													{
														// replace LogTestResult if this is better
														if (precfractionDistanceTravelled > logTestResult.fractionDistanceTravelled)
														{
															logTestResult.tResult = preciseTestReturn;
															logTestResult.distanceTravelled = distToTeleVolPrecise;
															logTestResult.fractionDistanceTravelled = precfractionDistanceTravelled;
															logTestResult.angle = minAngle;
															logTestResult.telePos = playerObject->position;
														}
													}
													else
													{
														reachedMinThresholdAngle = true;
													}
												}

												if (preciseTestReturn.first == testResultEnum::NoTele)
												{
													reachedMinThresholdAngle = true;
												}

												minAngle = std::nextafterf(minAngle, 0.f);
											}

											bool reachedMaxThresholdAngle = false;
											float maxAngle = narrowTestingAngle;
											while (reachedMaxThresholdAngle == false)
											{
												maxAngle = std::nextafterf(maxAngle, (DirectX::XM_2PI + randomVAoffset));
#ifdef SHIELD_BUMPS
												auto preciseTestReturn = testSetup(nook, setupVar, maxAngle, advancePhysics, goodTeleportVolume, shieldTimeReversalArray, &jitterMeasure, tickToNarrowOn);
#else
												auto preciseTestReturn = testSetup(nook, setupVar, maxAngle, advancePhysics, goodTeleportVolume, &jitterMeasure, tickToNarrowOn);
#endif

												if (preciseTestReturn.first == testResultEnum::GoodTele)
												{
													reachedMaxThresholdAngle = true;
#ifdef SHIELD_BUMPS
													analyseTeleport(nook, setupVar, maxAngle, advancePhysics, goodTeleportVolume, maxAngle, preciseTestReturn.second.value().second, jitterMeasure, shieldTimeReversalArray, bumpSetupGen.getSetupCount(), tickToNarrowOn);
#else
													analyseTeleport(nook, setupVar, maxAngle, advancePhysics, goodTeleportVolume, maxAngle, preciseTestReturn.second.value().second, jitterMeasure, bumpSetupGen.getSetupCount(), tickToNarrowOn);
#endif
												}

												if (preciseTestReturn.first == testResultEnum::PotentialTele)
												{
													// calculate distance travelled and check if it is still over threshold.
													// if NOT, set reachedMinThresholdAngle to true.

													float distToTeleVolPrecise = distanceToTeleportVolume(goodTeleportVolume, preciseTestReturn.second.value().second);

													float precfractionDistanceTravelled = (1.f - (distToTeleVolPrecise / requiredDistance));

													if (precfractionDistanceTravelled > TEST_EVERY_ANGLE_WHEN_DISTANCE_FRACTION_OVER)
													{
														// replace LogTestResult if this is better
														if (precfractionDistanceTravelled > logTestResult.fractionDistanceTravelled)
														{
															logTestResult.tResult = preciseTestReturn;
															logTestResult.distanceTravelled = distToTeleVolPrecise;
															logTestResult.fractionDistanceTravelled = precfractionDistanceTravelled;
															logTestResult.angle = maxAngle;
															logTestResult.telePos = playerObject->position;
														}
													}
													else
													{
														reachedMaxThresholdAngle = true;
													}
												}

												if (preciseTestReturn.first == testResultEnum::NoTele)
												{
													reachedMaxThresholdAngle = true;
												}

											}

											// make narrow/broad angle max angle if max angle bigger so we don't repeat ourselves
											narrowTestingAngle = std::fmaxf(narrowTestingAngle, maxAngle);
											broadTestingAngle = std::fmaxf(broadTestingAngle, maxAngle);

											PLOG_DEBUG << "BEST teleport from precise run: " << std::format("{:.6f}", logTestResult.angle) << " to pos: " << vec3ToString(logTestResult.telePos) << "; dist%: " << (logTestResult.fractionDistanceTravelled * 100.f) << " shieldOnTick: ";
#ifdef SHIELD_BUMPS
											PLOG_DEBUG << "shieldOnTick: " << logTestResult.bumpSetup.shieldOnTickLastResult;
#endif
											logPotentialTele(logTestResult, bumpSetupGen.getSetupCount());




										}

									}

								}
								else if (broadTestReturn.first == testResultEnum::GoodTele)
								{
#ifdef SHIELD_BUMPS
									analyseTeleport(nook, setupVar, broadTestingAngle, advancePhysics, goodTeleportVolume, broadTestingAngle, broadTestReturn.second.value().second, jitterMeasure, shieldTimeReversalArray, bumpSetupGen.getSetupCount());
#else
									analyseTeleport(nook, setupVar, broadTestingAngle, advancePhysics, goodTeleportVolume, broadTestingAngle, broadTestReturn.second.value().second, jitterMeasure, bumpSetupGen.getSetupCount());
#endif
								}
							}










				}
				auto thisRunEndTime = std::chrono::high_resolution_clock::now();
				auto sec_int = std::chrono::duration_cast<std::chrono::seconds>(thisRunEndTime - startTime).count();
				float avg_sec_per_setup = (float)sec_int / (float)totalRunSetups;

				PLOG_DEBUG << "Speed: " << (60.f / avg_sec_per_setup)  << " setups per minute";
				PLOG_DEBUG << "or " << (60.f * 60.f / avg_sec_per_setup) << " setups per hour";
			}


		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}

	}





public:

	BumpTestingToolImpl(GameState game, IDIContainer& dicon)
		:
		mToggleCallback(dicon.Resolve< SettingsStateAndEvents>().lock()->bumpToolRunningToggle->valueChangedEvent, [this](bool& n) {onToggleEvent(n); }),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>().lock()),
		getPlayerDatumWeak(resolveDependentCheat(GetPlayerDatum)),
		getObjectAddressWeak(resolveDependentCheat(GetObjectAddress)),
		entityPhysicsSimulatorWeak(resolveDependentCheat(EntityPhysicsSimulator)),
		objectTableRangeWeak(resolveDependentCheat(ObjectTableRange)),
		getCurrentBSPWeak(resolveDependentCheat(GetCurrentBSP))
	{
		instance = this;

		// setup log file 
		SYSTEMTIME t;
		GetSystemTime(&t);
		logFilePath = dicon.Resolve< DirPathContainer>().lock()->dirPath;
		logFilePath += std::format("{}_{:04}{:02}{:02}_{:02}{:02}{:02}.txt", "teleports", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);

		PLOG_DEBUG << "teleport log file path: " << logFilePath;
		if (!fileExists(logFilePath))
		{
			// create the file. new openings will use append mode.
			auto logfile = std::ofstream(logFilePath);
			logfile << "teleport log begin" << std::endl;

		}

		// // file will be opened WITHOUT append
		sortedlogfilepath = dicon.Resolve< DirPathContainer>().lock()->dirPath;
		sortedlogfilepath += std::format("{}_{:04}{:02}{:02}_{:02}{:02}{:02}.txt", "teleports_sorted", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);

		interestinglogfilepath = dicon.Resolve< DirPathContainer>().lock()->dirPath;
		interestinglogfilepath += std::format("{}_{:04}{:02}{:02}_{:02}{:02}{:02}.txt", "interesting_teles", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);


		//static bool once = false;
		//if (!once)
		//{
		//	once = true;
		//	DirectX::BoundingBox testVol1;
		//	DirectX::BoundingBox::CreateFromPoints(testVol1, SimpleMath::Vector3{-9.42998886f, -32.80875397f, 2.01591563f}, SimpleMath::Vector3{19.57659340f, -8.54473209f, 23.12697792f});
		//	goodTeleportVolumes.push_back(testVol1);
		//}



		//DirectX::BoundingBox::CreateFromPoints(expandedTeleportVolume, SimpleMath::Vector3{-50.42998886f, -45.80875397f, -4.01591563f}, SimpleMath::Vector3{40.57659340f, 10.54473209f, 23.12697792f});



		for (auto& vBounds : goodTeleportVolumeBounds)
		{
			DirectX::BoundingBox vol;
			DirectX::BoundingBox::CreateFromPoints(vol, vBounds.first, vBounds.second);
			goodTeleportVolume.push_back(vol);
		}

		for (auto& vol : goodTeleportVolume)
		{
			DirectX::BoundingBox volEx = vol;
			volEx.Extents = (SimpleMath::Vector3)volEx.Extents * 1.25f;
			goodTeleportVolumeExpanded.push_back(volEx);
		}
	}
};








BumpTestingTool::BumpTestingTool(GameState gameImpl, IDIContainer& dicon)
{
	pimpl = std::make_unique< BumpTestingToolImpl>(gameImpl, dicon);
}

BumpTestingTool::~BumpTestingTool() = default;