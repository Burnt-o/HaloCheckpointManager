#include "pch.h"
#include "Renderer3DImpl.h"
#include "directxtk\SimpleMath.h"
#include "closestPointOnPlanarFaceToPoint.h"



float measurePlanePointDistance(const SimpleMath::Plane& plane, const SimpleMath::Vector3& point)
{
	return plane.DotNormal((point - plane));
}



template<GameState::Value mGame>
bool Renderer3DImpl<mGame>::pointOnScreen(const SimpleMath::Vector3& worldPointPosition)
{
	return this->frustumViewWorld.Contains(worldPointPosition) != DirectX::ContainmentType::DISJOINT;
}




template<GameState::Value mGame>
SimpleMath::Vector3 Renderer3DImpl<mGame>::worldPointToScreenPosition(SimpleMath::Vector3 worldPointPosition, bool shouldFlipBehind)
{
	LOG_ONCE(PLOG_VERBOSE << "world to screen");
	auto worldPosTransformed = SimpleMath::Vector4::Transform({ worldPointPosition.x, worldPointPosition.y, worldPointPosition.z, 1.f },
		SimpleMath::Matrix::CreateWorld(
			SimpleMath::Vector3::Zero,
			SimpleMath::Vector3::Forward,
			SimpleMath::Vector3::Up
		));

	auto clipSpace = SimpleMath::Vector4::Transform(worldPosTransformed, viewProjectionMatrix);

	// result above is actually in homogenous space, divide all components by w to get clipspace.
	clipSpace = clipSpace / clipSpace.w;


	if (shouldFlipBehind)
	{
		// points behind camera need to have their clipSpace mirrored back the otherdirection
		if ((worldPointPosition - this->cameraPosition).Dot(this->cameraDirection) < 0)
		{
			clipSpace.x *= -1.f;
			clipSpace.y *= -1.f;
		}
	}
	
	// convert from clipSpace (-1..+1) to screenSpace (0..Width, 0..Height)
	auto out = SimpleMath::Vector3
	(
		(1.f + clipSpace.x) * 0.5 * this->screenSize.x,
		(1.f - clipSpace.y) * 0.5 * this->screenSize.y,
		clipSpace.z
	);


	return out;
}

template<GameState::Value mGame>
SimpleMath::Vector3 Renderer3DImpl<mGame>::worldPointToScreenPositionClamped(SimpleMath::Vector3 worldPointPosition , int screenEdgeOffset , bool* appliedClamp)
{
	if (pointOnScreen(worldPointPosition))
	{
		if (appliedClamp)
			*appliedClamp = false;
		return worldPointToScreenPosition(worldPointPosition, false);
	}
	else
	{
		if (appliedClamp)
			*appliedClamp = true;
		// will fall through to clamp logic
	}


		enum class sideDirection{ left, right, bottom, top};
		typedef std::tuple<sideDirection, SimpleMath::Vector3, float> sideAndClosestPointAndDistance;

		// get closest point to each face
		std::array<sideAndClosestPointAndDistance, 4> closestSidePoints =
		{
			sideAndClosestPointAndDistance(sideDirection::left, closestPointOnPlanarFaceToPoint(this->frustumViewWorldSideFaces.leftFrustum, this->frustumViewWorldSidePlanes.leftFrustum, worldPointPosition), 0),
			sideAndClosestPointAndDistance(sideDirection::right, closestPointOnPlanarFaceToPoint(this->frustumViewWorldSideFaces.rightFrustum, this->frustumViewWorldSidePlanes.rightFrustum, worldPointPosition), 0),
			sideAndClosestPointAndDistance(sideDirection::bottom, closestPointOnPlanarFaceToPoint(this->frustumViewWorldSideFaces.bottomFrustum,this->frustumViewWorldSidePlanes.bottomFrustum, worldPointPosition), 0),
			sideAndClosestPointAndDistance(sideDirection::top, closestPointOnPlanarFaceToPoint(this->frustumViewWorldSideFaces.topFrustum, this->frustumViewWorldSidePlanes.topFrustum,  worldPointPosition), 0)
		};

		// measure distance
		for (sideAndClosestPointAndDistance& side : closestSidePoints)
		{
			std::get<float>(side) = SimpleMath::Vector3::Distance(std::get<SimpleMath::Vector3>(side), worldPointPosition);
		}

		auto closest = std::min_element(std::begin(closestSidePoints), std::end(closestSidePoints), [](const auto& a, const auto& b) { return std::get<float>(a) < std::get<float>(b); });


		auto out = worldPointToScreenPosition(std::get<SimpleMath::Vector3>(*closest), false);

		if (screenEdgeOffset == 0)
			return out;

		switch (std::get<sideDirection>(*closest))
		{
		case sideDirection::left:
			out.x += screenEdgeOffset;
			break;

		case sideDirection::right:
			out.x -= screenEdgeOffset;
			break;

		case sideDirection::top:
			out.y += screenEdgeOffset;
			break;

		case sideDirection::bottom:
			out.y -= screenEdgeOffset;
			break;
		}
		
		return out;



}

template<GameState::Value mGame>
float Renderer3DImpl<mGame>::cameraDistanceToWorldPoint(SimpleMath::Vector3 worldPointPosition)
{
	return SimpleMath::Vector3::Distance(this->cameraPosition, worldPointPosition);
}



template<GameState::Value mGame>
void Renderer3DImpl<mGame>::renderTriggerModel(TriggerModel& model, uint32_t fillColor, uint32_t outlineColor)
{
	
	// Cull if none of the trigger is visible.
	if (this->frustumViewWorld.Contains(model.box) == false) return;
		
	// Calculate screen positions.
	for (int i = 0; i < 8; i++)
	{
		if ((model.corners[i] - this->cameraPosition).Dot(this->cameraDirection) < 0) // if behind
			model.cornersScreenPosition[i] = worldPointToScreenPositionClamped(model.corners[i], 0, nullptr); // clamp it
		else
			model.cornersScreenPosition[i] = worldPointToScreenPosition(model.corners[i], false); 
	}


	/*
	TODO: fix the remaining glitchyness. for some reason worldPointToScreenPositionClamped is flipping things that are behind the screen.
	
	*/
	
#define v3tv2(x) static_cast<SimpleMath::Vector2>(x)

#define isFaceVisible(p1, p2, p3, p4) this->frustumViewWorld.Intersects(SimpleMath::Plane(model.corners[p1], model.corners[p2], model.corners[p3]))

	// Draw the 6 faces of the trigger model.
#define drawFace(p1, p2, p3, p4) { ImGui::GetForegroundDrawList()->AddQuadFilled(v3tv2(model.cornersScreenPosition[p1]), v3tv2(model.cornersScreenPosition[p2]), v3tv2(model.cornersScreenPosition[p3]), v3tv2(model.cornersScreenPosition[p4]), fillColor); }

	drawFace(0, 1, 2, 3);
	drawFace(4, 5, 6, 7);
	drawFace(0, 1, 5, 4);
	drawFace(0, 3, 7, 4);
	drawFace(1, 2, 6, 5);
	drawFace(2, 3, 7, 6);

	// Draw the 12 line segments connecting each vertexes to make a highlighted outline.
#define drawLine(p1, p2) ImGui::GetForegroundDrawList()->AddLine(v3tv2(model.cornersScreenPosition[p1]), v3tv2(model.cornersScreenPosition[p2]), outlineColor);

	// near face
	drawLine(0, 1)
	drawLine(1, 2)
	drawLine(2, 3)
	drawLine(3, 0)
		
	// far face
	drawLine(4, 5)
	drawLine(5, 6)
	drawLine(6, 7)
	drawLine(7, 4)
		
	// connect the near and far faces
	drawLine(0, 4)
	drawLine(1, 5)
	drawLine(2, 6)
	drawLine(3, 7)


}


// explicit template instantiation
template class Renderer3DImpl<GameState::Value::Halo1>;
template class Renderer3DImpl<GameState::Value::Halo2>;
template class Renderer3DImpl<GameState::Value::Halo3>;
template class Renderer3DImpl<GameState::Value::Halo3ODST>;
template class Renderer3DImpl<GameState::Value::HaloReach>;
template class Renderer3DImpl<GameState::Value::Halo4>;