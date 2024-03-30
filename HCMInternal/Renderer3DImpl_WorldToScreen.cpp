#include "pch.h"
#include "Renderer3DImpl.h"
#include "directxtk\SimpleMath.h"

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


	// Measure the distance of the line from the worldPointPosition to the closest edge of the cameraFrustum.
	// First get the direction of this line. (the "(this->cameraDirection * 0.0001f)" is to avoid getting near-clipped)
	SimpleMath::Vector3 worldToCameraDir = this->cameraPosition + (this->cameraDirection * 0.0001f) - worldPointPosition;
	float intersectionDistance;
	if (this->frustumViewWorld.Intersects(worldPointPosition, worldToCameraDir, intersectionDistance)) // Then measure the distance to the frustum (should always return true)
	{
		// We want the closest point on the cameraFrustum to the worldPointPosition, so just multiply the direction by the distance and add to the initial position.
		auto clampedWorldPosition = worldPointPosition + (worldToCameraDir * intersectionDistance);

		// Get the screen position of that point
		auto out = worldPointToScreenPosition(clampedWorldPosition, false);

		if (screenEdgeOffset == 0) 
			return out;

		// Need to adjust screen space position by parameter screenEdgeOffset.
		// Have to determine which of the 4 frustrum side planes is closest to the initial world position.
		enum class sideDirection{ left, right, bottom, top};
		typedef std::pair<sideDirection, float> sideAndDistance;

		std::array<sideAndDistance, 4> sideDistanceMeasurements =
		{
			sideAndDistance(sideDirection::left, measurePlanePointDistance(this->frustumViewWorldPlanes.leftFrustum, worldPointPosition)),
			sideAndDistance(sideDirection::right, measurePlanePointDistance(this->frustumViewWorldPlanes.rightFrustum, worldPointPosition)),
			sideAndDistance(sideDirection::bottom, measurePlanePointDistance(this->frustumViewWorldPlanes.bottomFrustum, worldPointPosition)),
			sideAndDistance(sideDirection::top, measurePlanePointDistance(this->frustumViewWorldPlanes.topFrustum, worldPointPosition))
		};

		// switch on the smallest element in above measurement array and offset screen position accordingly.
		auto least = std::min_element(std::begin(sideDistanceMeasurements), std::end(sideDistanceMeasurements), [](const auto& a, const auto& b) { return a.second < b.second; });
		switch (least->first)
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
	else
	{
		// this should never happen if I've done my math right.
		LOG_ONCE(PLOG_ERROR << "point off screen but not interesecting with frustrum - weird");
		return worldPointToScreenPosition(worldPointPosition, true);
	}

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
		model.cornersScreenPosition[i] = worldPointToScreenPositionClamped(model.corners[i], -20, nullptr);
	}
	
#define v3tv2(x) static_cast<SimpleMath::Vector2>(x)


	// Draw the 6 faces of the trigger model.
#define drawFace(p1, p2, p3, p4) ImGui::GetForegroundDrawList()->AddQuadFilled(v3tv2(model.cornersScreenPosition[p1]), v3tv2(model.cornersScreenPosition[p2]), v3tv2(model.cornersScreenPosition[p3]), v3tv2(model.cornersScreenPosition[p4]), fillColor);

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