#include "pch.h"
#include "ShieldGroupSetupGenerator.h"
#include "GetShieldTris.h"




#ifdef SHIELD_BUMPS
#define CULL_TESTS 32 // 256 is perfect for ISB group size of 32 ish. big numbers prolly better for larger groups?
#else
#define CULL_TESTS 16
#endif



static float maxSetupCount = 497.f;
static float nonCulledSetups = 0.f;


enum class cullType {
	dontCull,
	culledForVelocityDir,
	culledForVelocityMag,
	culledForHitboxBox,
	culledForHitboxSpheres,
	culledForPreciseHitbox,
	culledForWeird,
	culledForDistance,
	culledForDuplicate,
	killTheRest,
};

struct CullSimulationResults 
{


	std::array<cullType, CULL_TESTS> CullResults;
	bool shouldJump;
	int ticksAfterGoForShieldOn; // 5 to 20 /////(41 - ticksBeforeGoToJump)
};


//constexpr std::array<float, CULL_TESTS> angleIndexToFloatArray()
//{
//	std::array<float, CULL_TESTS> out;
//	for (int angleIndex = 0; angleIndex < CULL_TESTS; angleIndex++)
//	{
//		if (angleIndex == 0) out.at(0) == 0.f;
//		out.at(angleIndex) == DirectX::XM_2PI / CULL_TESTS * angleIndex;
//	}
//	return out;
//}

float angleIndexToFloat(int index)
{
	return DirectX::XM_2PI / (float)CULL_TESTS * (float)index;
}


// first index is jumps-before-go-ticks
// second index is ticks-after-go-for-shield-on

std::vector<std::pair<float, float>> getRangleRangeFromCullResults(CullSimulationResults& cullResultUnExpanded)
{
#ifndef ENABLE_CULLING
	nonCulledSetups += 1.f;
	return std::vector<std::pair<float, float>>({ { 0.f, DirectX::XM_2PI } });
#endif

	bool anySuccess = false;
	CullSimulationResults cullResultExpanded = cullResultUnExpanded;






	for (int i = 0; i < CULL_TESTS; i++)
	{
		if (cullResultUnExpanded.CullResults.at(i) == cullType::dontCull)
		{
			anySuccess = true;
			if (i == 0) // derp i forgot modulo doesn't really work how I expect on negative n
			{
				cullResultExpanded.CullResults.at(CULL_TESTS - 1) = cullResultUnExpanded.CullResults.at(i);
			}
			else
			{
				cullResultExpanded.CullResults.at((i - 1) % CULL_TESTS) = cullResultUnExpanded.CullResults.at(i);
			}


			cullResultExpanded.CullResults.at((i + 1) % CULL_TESTS) = cullResultUnExpanded.CullResults.at(i);
		}
	}

	if (!anySuccess)
	{
		LOG_ONCE(PLOG_DEBUG << "!anySuccess");
		return {};
	}


	std::vector<std::pair<float, float>> leftRightAngleRanges = {};

	auto& cullRes = cullResultExpanded.CullResults;

	auto iteratorOfFirstSuccess = std::ranges::find_if(cullRes, [](auto& b) {return b == cullType::dontCull; }); // guarenteed to be valid iterator because of anySuccess check
	auto successCount = std::ranges::count_if(cullRes, [](auto& b) { return b == cullType::dontCull; }); // guarenteed to be between 3 and CULL_TESTS

	LOG_ONCE_CAPTURE(PLOG_DEBUG << "expanded successCount: " << s, s = successCount);

	assert(successCount >= 3 && successCount <= CULL_TESTS);
	assert(iteratorOfFirstSuccess != cullRes.end());


	if (successCount == cullRes.size())
	{
		return { std::pair<float, float>(0, DirectX::XM_2PI) };
	}

	auto it = cullRes.begin();

	while (it != cullRes.end())
	{
		if ((*it) == cullType::dontCull)
		{
			auto nextFailure = std::find_if(it, cullRes.end(), [](auto& b) {return b != cullType::dontCull; });

			float startAngle;
			if (it == cullRes.begin())
			{
				startAngle = 0.f;
			}
			else
			{
				startAngle = angleIndexToFloat(std::distance(cullRes.begin(), it));
			}

			float endAngle = 0.f;
			if (nextFailure == cullRes.end())
			{
				endAngle = DirectX::XM_2PI;
			}
			else
			{
				endAngle = angleIndexToFloat(std::distance(cullRes.begin(), nextFailure - 1));
			}

			leftRightAngleRanges.push_back(std::pair<float, float>(startAngle, endAngle));

			it = nextFailure;
		}
		else
		{
			it++;
		}

	}
	nonCulledSetups += ((float)successCount / (float)CULL_TESTS);
	return leftRightAngleRanges;
}


void cullResultToBumpSetup(BumpSetup& originalBumpSetup, std::vector<CullSimulationResults>& cullResultsUnExpanded)
{
	originalBumpSetup.leftRightAngleRanges.clear();

#ifndef ENABLE_CULLING
	for (auto& cullRes : cullResultsUnExpanded)
	{
		originalBumpSetup.leftRightAngleRanges.push_back({ {0.f, DirectX::XM_2PI} });
	}
	return;
#endif

	
	for (auto& cullRes : cullResultsUnExpanded)
	{
		originalBumpSetup.leftRightAngleRanges.push_back(getRangleRangeFromCullResults(cullRes));
	}

	assert(originalBumpSetup.leftRightAngleRanges.size() == originalBumpSetup.goInputs.size());

}




cullType isPotentialTeleport(BumpPlayerObject* playerObject, int totalTickCount = 0)
{
#ifndef ENABLE_CULLING
	return cullType::dontCull;
#endif





	// test that the players total velocity is, say, at least 5% of crouch-backwards speed when totalTickCount is over, say, 10.
	// this should cull out scenarios where the player got stuck on something (ie a nook)
	if (totalTickCount > 15)
	{
		constexpr float crouchBackwardsSpeed = 0.021f;

		auto horiVelocity = (SimpleMath::Vector2)playerObject->velocity;

		if (horiVelocity.Length() < (crouchBackwardsSpeed * 0.05f))
		{
			return cullType::culledForVelocityMag;
		}
	}



	constexpr float collisionRadius = 0.2f;
	constexpr float standingCollisionHeight = 0.7f;
	constexpr float crouchingCollisionHeight = 0.5f; // todo, add check for crouch





#ifdef SHIELD_BUMPS
	// Simple fast test against bounding box of shield verts
	{
		// generate the bounding box from vertices of the shield if not done yet
		static DirectX::BoundingOrientedBox shieldBoundingBox;
		static bool shieldBoundingBoxGen = false;

		if (shieldBoundingBoxGen == false)
		{
			PLOG_DEBUG << "generating shield bounding box";
			std::vector<SimpleMath::Vector3> shieldVerts;
			auto worldShieldTris = getShieldTris();
			for (const auto& tri : worldShieldTris)
			{
				shieldVerts.push_back(tri.at(0)); shieldVerts.push_back(tri.at(1)); shieldVerts.push_back(tri.at(2));
			}
			PLOG_DEBUG << "kay";

			for (auto& tri : worldShieldTris)
			{
				PLOG_DEBUG << "shield world vert: ";
				PLOG_DEBUG << "a: " << tri.at(0);
				PLOG_DEBUG << "b: " << tri.at(1);
				PLOG_DEBUG << "c: " << tri.at(2);
			}

			std::stringstream ss;
			ss << "<waypoint3DList>";
			for (auto& tri : worldShieldTris)
			{


				for (auto& vert : tri)
				{
					ss << std::format("<waypoint> <position> {:8f}, {:8f}, {:8f}</position> </waypoint>", vert.x, vert.y, vert.z);
				}
			}
			ss << "</waypoint3DList>";
			PLOG_DEBUG << "as waypoints:";
			PLOG_DEBUG << ss.str();


			// this is a bad hack that relies on the fact that nested arrays are contigious. I'm pretttty sure the standard guarentees this but still feels icky
			//DirectX::BoundingOrientedBox::CreateFromPoints(shieldBoundingBox, worldShieldTris.size() * 3, worldShieldTris.data()->data(), sizeof(SimpleMath::Vector3)); // this call is crashing lol
			DirectX::BoundingOrientedBox::CreateFromPoints(shieldBoundingBox, shieldVerts.size(), shieldVerts.data(), sizeof(SimpleMath::Vector3)); 

			PLOG_DEBUG << "shield bounding center: " << (SimpleMath::Vector3)shieldBoundingBox.Center;
			PLOG_DEBUG << "shield bounding extents: " << (SimpleMath::Vector3)shieldBoundingBox.Extents;

			shieldBoundingBoxGen = true;
		}



		LOG_ONCE(PLOG_DEBUG << "testing players collision against shield bounding box");
		auto playerBox = DirectX::BoundingBox::BoundingBox({ playerObject->position.x, playerObject->position.y, playerObject->position.z + ((0.7f + 0.2f)/ 2.f)}, {0.2f, 0.2f, 0.7f + 0.2f});



		LOG_ONCE_CAPTURE(PLOG_DEBUG << "playerbox pos: " << p, p = (SimpleMath::Vector3)playerBox.Center);

		// super fast test
		if (shieldBoundingBox.Contains(playerBox) == ContainmentType::DISJOINT) return cullType::culledForHitboxBox;

		auto playerLow = DirectX::BoundingSphere::BoundingSphere({ playerObject->position.x, playerObject->position.y , playerObject->position.z + collisionRadius }, collisionRadius);
		auto playerMed = DirectX::BoundingSphere::BoundingSphere({ playerObject->position.x, playerObject->position.y , playerObject->position.z + collisionRadius + (standingCollisionHeight / 2) }, collisionRadius);
		auto playerHigh = DirectX::BoundingSphere::BoundingSphere({ playerObject->position.x, playerObject->position.y , playerObject->position.z + standingCollisionHeight - collisionRadius }, collisionRadius);

		if (shieldBoundingBox.Contains(playerLow) == ContainmentType::DISJOINT
			&& shieldBoundingBox.Contains(playerMed) == ContainmentType::DISJOINT
			&& shieldBoundingBox.Contains(playerHigh) == ContainmentType::DISJOINT) return cullType::culledForHitboxSpheres;

	}
#endif


	#ifdef CULL_FOR_VELOCITY_DIR
		// My understanding of teleports is that they can only send you in the direction facing opposite semi-circle of your velocity.
		// So we test our velocity direction vs our direction wrt to the left and right part of the good teleport volume.
		// if both are negative then we cannot possibly get a teleport to that area.
	{
		LOG_ONCE(PLOG_DEBUG << "testing players velocity direction against target destination");
		typedef SimpleMath::Vector2 vec2;
		auto leftDir = (vec2)goodTeleportVolumeLeft - (vec2)playerObject->position;
		auto rightDir = (vec2)goodTeleportVolumeRight - (vec2)playerObject->position;

		auto pvel = ((vec2)playerObject->velocity);


		auto leftDot = pvel.Dot(leftDir);
		auto rightDot = pvel.Dot(rightDir);

		if (pvel == SimpleMath::Vector2::Zero || leftDot >= 0.f || rightDot >= 0.f)
		{
			return cullType::culledForVelocityDir;
		}

		
	}
#endif


	return cullType::dontCull;


}



#ifdef SHIELD_BUMPS
BumpSetup cullISBGroupSetup(const Nook& nook, BumpSetup bumpSetup, EntityPhysicsSimulator::AdvancePhysicsTicks* advancePhysics, BumpPlayerObject* playerObject, BumpShieldObject* shieldObject, Datum playerDatum)
{
	std::vector<CullSimulationResults> resultVec;
	shieldObject->setShieldState(false);
	for (int angleIndex = 0; angleIndex < CULL_TESTS; angleIndex++)
	{
		float angle = angleIndexToFloat(angleIndex);

	


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

		auto positionBeforeIdleStanceAdjust = playerObject->position;

		// set idle input
			setInputs(bumpSetup.goIdleInput);
			if (contains(bumpSetup.goIdleInput, Input::Crouch))
				playerObject->crouchPercent = 1.f;
			else
				playerObject->crouchPercent = 0.f;

			advancePhys(1);
			setInputs(bumpSetup.goIdleInput);
			advancePhys(1);


		//bool cullForWeird = false;
		//if (playerObject->position != positionBeforeIdleStanceAdjust)
		//{
		//	LOG_ONCE(PLOG_DEBUG << "position after stance adjustment did not match, abandoning setup");
		//	cullForWeird = true;
		//	killTheRest = true;
		//}


		auto collapsedInputs = bumpSetup.GetCollapsedGoInputVector();
		int cullInputsBefore = collapsedInputs.size() > 1;
		if (collapsedInputs.size() == 2)
		{
			cullInputsBefore = collapsedInputs.at(0).second;
		}
		else if (collapsedInputs.size() == 3)
		{
			cullInputsBefore = collapsedInputs.at(0).second + collapsedInputs.at(1).second;
		}


		// go time!
		SimpleMath::Vector3 playerPositionLastTick = playerObject->position;

		int totalTickCount = 0;
		for (auto& goInput : bumpSetup.goInputs)
		{

			playerObject->setInput(goInput);

			advancePhys(1);


			while (totalTickCount >= resultVec.size())
			{
				CullSimulationResults cullRes;
				std::fill(cullRes.CullResults.begin(), cullRes.CullResults.end(), cullType::culledForWeird);
				cullRes.ticksAfterGoForShieldOn = totalTickCount;
				resultVec.push_back(cullRes);
				LOG_ONCE(PLOG_DEBUG << "pushing back new cull result to populate");
			}




			if (totalTickCount < cullInputsBefore)
			{
				resultVec.at(totalTickCount).CullResults.at(angleIndex) = cullType::culledForDuplicate;
			}
#ifdef SHIELD_BUMPS
			else if (playerObject->position.z < (shieldPosition.z - 1.f) || SimpleMath::Vector3::Distance(shieldPosition, playerObject->position) > 1)
			{
				resultVec.at(totalTickCount).CullResults.at(angleIndex) = cullType::culledForDistance;
			}
#endif
			else
			{
				resultVec.at(totalTickCount).CullResults.at(angleIndex) = isPotentialTeleport(playerObject, totalTickCount);
			}



				
			playerPositionLastTick = playerObject->position;
			totalTickCount++;
		
		}


	}





	nonCulledSetups = 0.f;
	maxSetupCount = resultVec.size();


	cullResultToBumpSetup(bumpSetup, resultVec);
	PLOG_INFO << "Culled setups down to " << nonCulledSetups << "/" << maxSetupCount << ", or cull % of: " << std::setprecision(10) << (100.f - ((nonCulledSetups / maxSetupCount) * 100.f));
	return bumpSetup;


}
#endif



BumpSetup  cullISBSetup(const Nook& nook, BumpSetup bumpSetup, EntityPhysicsSimulator::AdvancePhysicsTicks* advancePhysics, BumpPlayerObject* playerObject, Datum playerDatum)
{
	std::vector<CullSimulationResults> resultVec;
	for (int angleIndex = 0; angleIndex < CULL_TESTS; angleIndex++)
	{
		float angle = angleIndexToFloat(angleIndex);



		playerObject->velocity = SimpleMath::Vector3::Zero;
		playerObject->position = safeStandingSpot;
		playerObject->setViewAngle(angle);

		InpMask inputToSet = Input::None;

		for (int i = 0; i < animWaitTime + 5; i++)
		{
			setInputs(inputToSet);
			advancePhys(1);
		}

		// stay STILL damn you
		for (int i = 0; i < 5; i++)
		{
			playerObject->velocity = SimpleMath::Vector3::Zero;
			playerObject->position = nook.second;
			advancePhys(1);
		}

		// pre setups (standing only)
		for (auto& inpPair : bumpSetup.setupInputs)
		{
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

		auto positionBeforeIdleStanceAdjust = playerObject->position;

		// set idle input
		for (int i = 0; i < animWaitTime + 5; i++)
		{
			setInputs(bumpSetup.goIdleInput);
			advancePhys(1);
		}


		//bool cullForWeird = false;
		//if (playerObject->position != positionBeforeIdleStanceAdjust)
		//{
		//	LOG_ONCE(PLOG_DEBUG << "position after stance adjustment did not match, abandoning setup");
		//	cullForWeird = true;
		//	killTheRest = true;
		//}



		auto collapsedInputs = bumpSetup.GetCollapsedGoInputVector();
		int cullInputsBefore = collapsedInputs.size() > 1;
		if (collapsedInputs.size() == 2)
		{
			cullInputsBefore = collapsedInputs.at(0).second;
		}
		else if (collapsedInputs.size() == 3)
		{
			cullInputsBefore = collapsedInputs.at(0).second + collapsedInputs.at(1).second;
		}


		// go time!
		SimpleMath::Vector3 playerPositionLastTick = playerObject->position;

		int totalTickCount = 0;
		for (auto& goInput : bumpSetup.goInputs)
		{


			playerObject->setInput(goInput);

			advancePhys(1);


			while (totalTickCount >= resultVec.size())
			{
				CullSimulationResults cullRes;
				std::fill(cullRes.CullResults.begin(), cullRes.CullResults.end(), cullType::culledForWeird);
				cullRes.ticksAfterGoForShieldOn = totalTickCount;
				resultVec.push_back(cullRes);
				LOG_ONCE(PLOG_DEBUG << "pushing back new cull result to populate");
			}

#ifdef SHIELD_BUMPS
			if (playerObject->position.z < (shieldPosition.z - 1.f))
				break;

			// if the players over 1 units away from the shield then there's no way
			if (SimpleMath::Vector3::Distance(shieldPosition, playerObject->position) > 1)
				break;

#endif


			if (totalTickCount < cullInputsBefore)
			{
				resultVec.at(totalTickCount).CullResults.at(angleIndex) = cullType::culledForDuplicate;
			}
			else
			{
				resultVec.at(totalTickCount).CullResults.at(angleIndex) = isPotentialTeleport(playerObject, totalTickCount);
			}




			playerPositionLastTick = playerObject->position;
			totalTickCount++;
			
		}





	}





	nonCulledSetups = 0.f;
	maxSetupCount = resultVec.size();




	cullResultToBumpSetup(bumpSetup, resultVec);
	PLOG_INFO << "Culled setups down to " << nonCulledSetups << "/" << maxSetupCount << ", or cull % of: " << std::setprecision(10) << (100.f - ((nonCulledSetups / maxSetupCount) * 100.f));
	return bumpSetup;

}

