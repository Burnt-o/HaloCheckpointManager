#include "pch.h"
#include "Renderer3DImpl.h"
#include "directxtk\SimpleMath.h"

//#include "Mathematics\DistPointTriangle.h"



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
		(1.f + clipSpace.x) * 0.5f * this->screenSize.x,
		(1.f - clipSpace.y) * 0.5f * this->screenSize.y,
		clipSpace.z
	);



#ifdef HCM_DEBUG
// TODO: output points that lay far outside the screen boundary on the y axis do not have quite the correct x position. 
// Specifically, the more outside the y bounds they are, the more their position becomes too close to the center of the screen than it ought to be.
// The problem is probably in our clipspace -> screenSpace conversion, surely ? I've double checked online and it should be exactly correct.
// Maybe it's a float precision error as certain values get close to 0 / infinity (ie clipSpace.w being close to zero would be bad).
// But why is it only affecting x axis?

	// ACTUALLY the issue is with points BEHIND the camera. they're wrapping around. That's the issue. I need points that wrap around to terminate at the screen bounds ? idk


#endif

	return out;
}

template<GameState::Value mGame>
SimpleMath::Vector3 Renderer3DImpl<mGame>::worldPointToScreenPositionClamped(SimpleMath::Vector3 worldPointPosition , int screenEdgeOffset , bool* appliedClamp)
{
https://github.com/jinincarnate/off-screen-indicator/blob/master/Off%20Screen%20Indicator/Assets/Scripts/OffScreenIndicatorCore.cs

	auto screenPosition = worldPointToScreenPosition(worldPointPosition, true);

	DEBUG_KEY_LOG('8', PLOG_DEBUG << "pre  clamp screen pos: " << screenPosition; );

	// if point is on screen it doesn't need clamping
	if (pointOnScreen(worldPointPosition))
	{
		if (appliedClamp) *appliedClamp = false;
		return screenPosition;
	}
	// else, need to clamp.
	if (appliedClamp) *appliedClamp = true;

	// Need to align unclampedScreenPos to center of screen
	screenPosition.x -= this->screenCenter.x;
	screenPosition.y -= this->screenCenter.y;

	// find angle between centre of screen and the unclamped screen pos.
	float screenPosAngle = std::atan2f(screenPosition.y, screenPosition.x);
	float screenPosSlope = std::tanf(screenPosAngle);

	// line equation stuff. 
	if (screenPosition.x > 0)
	{
		// screenPosition is off the the right of the screen. New screen position needs to be at x-max., with y position y = mx.
		screenPosition = SimpleMath::Vector3(this->screenCenter.x, this->screenCenter.x * screenPosSlope, screenPosition.z);
	}
	else
	{
		screenPosition = SimpleMath::Vector3(-this->screenCenter.x, -this->screenCenter.x * screenPosSlope, screenPosition.z);
	}

	if (screenPosition.y > this->screenCenter.y)
	{
		screenPosition = SimpleMath::Vector3(this->screenCenter.y / screenPosSlope, this->screenCenter.y, screenPosition.z);
	}
	else if (screenPosition.y < -this->screenSize.y)
	{
		screenPosition = SimpleMath::Vector3(-this->screenCenter.y / screenPosSlope, -this->screenCenter.y, screenPosition.z);
	}

	// bring screenPos back to it's original reference 
	screenPosition.x += this->screenCenter.x;
	screenPosition.y += this->screenCenter.y;

	DEBUG_KEY_LOG('8', PLOG_DEBUG << "post clamp screen pos: " << screenPosition; );

	return screenPosition;

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
		model.cornersScreenPosition[i] = worldPointToScreenPosition(model.corners[i], true);
	}

	
#define v3tv2(x) static_cast<SimpleMath::Vector2>(x)


	// Draw the 6 faces of the trigger model. If not culled.
#define wrapsAround(p1, p2, p3, p4) frustumViewWorldBackwards.Intersects(model.corners[p1], model.corners[p2], model.corners[p3]) || frustumViewWorldBackwards.Intersects(model.corners[p1], model.corners[p3], model.corners[p4])
#define drawFace(p1, p2, p3, p4) if (!wrapsAround(p1, p2, p3 ,p4)) { ImGui::GetForegroundDrawList()->AddQuadFilled(v3tv2(model.cornersScreenPosition[p1]), v3tv2(model.cornersScreenPosition[p2]), v3tv2(model.cornersScreenPosition[p3]), v3tv2(model.cornersScreenPosition[p4]), fillColor); }

	// magic numbers derived from DirectX::g_BoxOffset
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

#ifdef HCM_DEBUG

template<GameState::Value mGame>
void Renderer3DImpl<mGame>::renderTriggerModelSortedDebug(TriggerModel& model, uint32_t fillColor, uint32_t outlineColor)
{

	// Cull if none of the trigger is visible.
	if (this->frustumViewWorld.Contains(model.box) == false) return;

	// Calculate screen positions.
	for (int i = 0; i < 8; i++)
	{
		model.cornersScreenPosition[i] = worldPointToScreenPosition(model.corners[i], true);
	}

#define v3tv2(x) static_cast<SimpleMath::Vector2>(x)

	// Draw the 6 faces of the trigger model. If not culled.
#define wrapsAround(p1, p2, p3, p4) frustumViewWorldBackwards.Intersects(model.corners[p1], model.corners[p2], model.corners[p3]) || frustumViewWorldBackwards.Intersects(model.corners[p1], model.corners[p3], model.corners[p4])
#define drawFace(p1, p2, p3, p4, color) if (!wrapsAround(p1, p2, p3 ,p4)) { ImGui::GetForegroundDrawList()->AddQuadFilled(v3tv2(model.cornersScreenPosition[p1]), v3tv2(model.cornersScreenPosition[p2]), v3tv2(model.cornersScreenPosition[p3]), v3tv2(model.cornersScreenPosition[p4]), color); }

	// hm. order matters if faces are different colours. The closest thing needs to get drawn last. Can i just sort faces by distance to center point?

	// array of indexes into model.corners/model.cornersScreenPosition
	typedef std::array<int, 4> face;

	// face, distance to face, colour to draw that face.
	typedef std::tuple<face, float, uint32_t> faceInfo;

	// magic numbers derived from DirectX::g_BoxOffset
	std::array<faceInfo, 6 > faces = {
	faceInfo(face{ 0, 1, 2, 3 }, 0.f, ImGui::ColorConvertFloat4ToU32(SimpleMath::Vector4(1.f, 0.f, 0.f, 1.f))),
	faceInfo(face{ 4, 5, 6, 7 }, 0.f, ImGui::ColorConvertFloat4ToU32(SimpleMath::Vector4(0.f, 1.f, 0.f, 1.f))),
	faceInfo(face{ 0, 1, 5, 4 }, 0.f, ImGui::ColorConvertFloat4ToU32(SimpleMath::Vector4(0.f, 0.f, 1.f, 1.f))),
	faceInfo(face{ 0, 3, 7, 4 }, 0.f, ImGui::ColorConvertFloat4ToU32(SimpleMath::Vector4(1.f, 1.f, 0.f, 1.f))),
	faceInfo(face{ 1, 2, 6, 5 }, 0.f, ImGui::ColorConvertFloat4ToU32(SimpleMath::Vector4(1.f, 0.f, 1.f, 1.f))),
	faceInfo(face{ 2, 3, 7, 6 }, 0.f, ImGui::ColorConvertFloat4ToU32(SimpleMath::Vector4(0.f, 1.f, 1.f, 1.f))),
	};

	// measure distance to center of each face
	for (auto& f : faces)
	{
		auto centerPosition = (model.corners[std::get<face>(f)[0]] + model.corners[std::get<face>(f)[1]] + model.corners[std::get<face>(f)[2]] + model.corners[std::get<face>(f)[3]]) / 4.f;
		std::get<float>(f) = SimpleMath::Vector3::DistanceSquared(this->cameraPosition, centerPosition);
	}

	// sort by distance
	std::sort(faces.begin(), faces.end(), [](const auto& a, const auto& b) { return std::get<float>(a) > std::get<float>(b); });

#define drawSortedFace(index) drawFace(std::get<face>(faces.at(index))[0], std::get<face>(faces.at(index))[1], std::get<face>(faces.at(index))[2], std::get<face>(faces.at(index))[3], std::get<uint32_t>(faces.at(index)));

	drawSortedFace(0);
	drawSortedFace(1);
	drawSortedFace(2);
	drawSortedFace(3);
	drawSortedFace(4);
	drawSortedFace(5);


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
#endif


// explicit template instantiation
template class Renderer3DImpl<GameState::Value::Halo1>;
template class Renderer3DImpl<GameState::Value::Halo2>;
template class Renderer3DImpl<GameState::Value::Halo3>;
template class Renderer3DImpl<GameState::Value::Halo3ODST>;
template class Renderer3DImpl<GameState::Value::HaloReach>;
template class Renderer3DImpl<GameState::Value::Halo4>;