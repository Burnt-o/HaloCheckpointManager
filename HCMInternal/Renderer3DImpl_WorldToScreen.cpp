#include "pch.h"
#include "Renderer3DImpl.h"
#include "directxtk\SimpleMath.h"

#define v3tv2(x) static_cast<SimpleMath::Vector2>(x)

template<GameState::Value mGame>
std::vector<SimpleMath::Vector3> Renderer3DImpl<mGame>::faceClippedToFrustum(const faceView& face)
{
	static_assert(false, "TODO: faceClippedToFrustum");
	//return { {0,0,0} };
}

template<GameState::Value mGame>
std::array<VertexInfo, 2> Renderer3DImpl<mGame>::edgeClippedToFrustum(const edgeView& edge)
{
	static_assert(false, "TODO: edgeClippedToFrustum");
	//return {};
}


template<GameState::Value mGame>
void  Renderer3DImpl<mGame>::renderFace(const faceView& face, uint32_t color) 
{
	// Test if all of the points are inside the camera frustum
	if (std::all_of(face.begin(), face.end(), [this](const VertexInfo* v) { return this->frustumViewWorld.Contains(v->worldPosition); }))
	{
		// then we don't need to do anything fancy, render the square!
		ImGui::GetForegroundDrawList()->AddQuadFilled(v3tv2(face[0]->screenPosition), v3tv2(face[1]->screenPosition), v3tv2(face[2]->screenPosition), v3tv2(face[3]->screenPosition), color);
	}

	// uh oh, at least one of the vertexes lies outside the frustum! We'll need to construct a new planar polygon that is the intersection of the original face AND the view frustum.
	// This new polygon may have between 3 and 8 vertices.

	// get the clipped polygon
	auto newPolygon = faceClippedToFrustum(face); 


	// calculate the new screen positions of this polygon
	std::vector<ImVec2> newPolygonScreenPositions;
	for (auto& v : newPolygon)
	{
		newPolygonScreenPositions.emplace_back(v3tv2(worldPointToScreenPosition(v, false)));
	}

	// draw it!
	ImGui::GetForegroundDrawList()->AddConvexPolyFilled(newPolygonScreenPositions.data(), newPolygonScreenPositions.size(), color);

}


template<GameState::Value mGame>
void  Renderer3DImpl<mGame>::renderEdge(const edgeView& edge, uint32_t color)
{
	// Test if all of the points are inside the camera frustum
	if (std::all_of(edge.begin(), edge.end(), [this](const VertexInfo* v) { return this->frustumViewWorld.Contains(v->worldPosition); }))
	{
		// then we don't need to do anything fancy, render the line!
		ImGui::GetForegroundDrawList()->AddLine(v3tv2(edge[0]->screenPosition), v3tv2(edge[1]->screenPosition), color);
	}

	//  uh oh, at least one of the vertexes lies outside the frustum! We'll need to construct a new line that is the intersection of the original edge AND the view frustum.
	// This new line will of course be a segment of the original line.

	// get the clipped line
	auto newLine = edgeClippedToFrustum(edge);

	// calulate the new lines screen positions
	std::array<ImVec2, 2> newLineScreenPositions = {
		v3tv2(worldPointToScreenPosition(newLine[0].worldPosition, false)),
		v3tv2(worldPointToScreenPosition(newLine[1].worldPosition, false)),
	};

	// draw it!
	ImGui::GetForegroundDrawList()->AddLine(newLineScreenPositions[0], newLineScreenPositions[1], color);

}

template<GameState::Value mGame>
bool Renderer3DImpl<mGame>::pointBehindCamera(const SimpleMath::Vector3& point)
{
	return (point - this->cameraPosition).Dot(this->cameraDirection) < 0;
}

void clampScreenPosition(SimpleMath::Vector3& screenPosition, const SimpleMath::Vector2& screenCenter, bool reverse = false)
{
	// Need to align unclampedScreenPos to center of screen
	screenPosition.x -= screenCenter.x;
	screenPosition.y -= screenCenter.y;

	// find angle between centre of screen and the unclamped screen pos.
	float screenPosAngle = std::atan2f(screenPosition.y, screenPosition.x);

	if (reverse)
		screenPosAngle = std::fmodf(screenPosAngle + DirectX::XM_2PI, DirectX::XM_2PI) - DirectX::XM_PI;

	float screenPosSlope = std::tanf(screenPosAngle);



	// line equation stuff. 
	if (screenPosition.x > 0)
	{
		// screenPosition is off the the right of the screen. New screen position needs to be at x-max., with y position y = mx.
		screenPosition = SimpleMath::Vector3(screenCenter.x, screenCenter.x * screenPosSlope, screenPosition.z);
	}
	else
	{
		screenPosition = SimpleMath::Vector3(-screenCenter.x, -screenCenter.x * screenPosSlope, screenPosition.z);
	}

	if (screenPosition.y > screenCenter.y)
	{
		screenPosition = SimpleMath::Vector3(screenCenter.y / screenPosSlope, screenCenter.y, screenPosition.z);
	}
	else if (screenPosition.y < -screenCenter.y) // might need to times screenCenter.y by two.
	{
		screenPosition = SimpleMath::Vector3(-screenCenter.y / screenPosSlope, -screenCenter.y, screenPosition.z);
	}

	// bring screenPos back to it's original reference 
	screenPosition.x += screenCenter.x;
	screenPosition.y += screenCenter.y;

	DEBUG_KEY_LOG('8', PLOG_DEBUG << "post clamp screen pos: " << screenPosition; );


}


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

	if (pointBehindCamera(worldPointPosition))
	{
		// points in semisphere behind camera need to have their clipSpace mirrored back the other direction
			clipSpace.x *= -1.f;
			clipSpace.y *= -1.f;

			/* TODO: I think the glitchyness I'm dealing with stems from here. 
				This math is not correct.. somehow.. 
				Maybe this needs to happen in homogenous space? 
			*/

	}
	
	// convert from clipSpace (-1..+1) to screenSpace (0..Width, 0..Height)
	auto out = SimpleMath::Vector3
	(
		(1.f + clipSpace.x) * 0.5f * this->screenSize.x,
		(1.f - clipSpace.y) * 0.5f * this->screenSize.y,
		clipSpace.z
	);

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

	clampScreenPosition(screenPosition, this->screenCenter);
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
		model.vertices[i].screenPosition = worldPointToScreenPosition(model.vertices[i].worldPosition, false);
	}

	// Render faces
	for (auto& f : model.faceViews)
	{
		renderFace(f, fillColor);
	}

	// Render edges
	for (auto& e : model.edgeViews)
	{
		renderEdge(e, outlineColor);
	}


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
		model.vertices[i].screenPosition = worldPointToScreenPosition(model.vertices[i].worldPosition, false);
	}

	// Order faces by world distance from camera. And assign each one a color.
	typedef std::tuple<faceView&, float, uint32_t> sortedFaceView;
	std::array<sortedFaceView, 6> sortedFaceViews = {
		sortedFaceView(model.faceViews[0], 0.f, ImGui::ColorConvertFloat4ToU32(SimpleMath::Vector4(1.f, 0.f, 0.f, 1.f))),
		sortedFaceView(model.faceViews[1], 0.f, ImGui::ColorConvertFloat4ToU32(SimpleMath::Vector4(1.f, 0.f, 0.f, 1.f))),
		sortedFaceView(model.faceViews[2], 0.f, ImGui::ColorConvertFloat4ToU32(SimpleMath::Vector4(1.f, 0.f, 0.f, 1.f))),
		sortedFaceView(model.faceViews[3], 0.f, ImGui::ColorConvertFloat4ToU32(SimpleMath::Vector4(1.f, 0.f, 0.f, 1.f))),
		sortedFaceView(model.faceViews[4], 0.f, ImGui::ColorConvertFloat4ToU32(SimpleMath::Vector4(1.f, 0.f, 0.f, 1.f))),
		sortedFaceView(model.faceViews[5], 0.f, ImGui::ColorConvertFloat4ToU32(SimpleMath::Vector4(1.f, 0.f, 0.f, 1.f))),
			};

	// Measure the world distance from camera:
	for (auto& fv : sortedFaceViews)
	{
		// Center position of face is just all 4 vertexes added up and div by 4.
		auto centerPositionOfFace = std::get<faceView&>(fv)[0]->worldPosition + std::get<faceView&>(fv)[1]->worldPosition + std::get<faceView&>(fv)[2]->worldPosition + std::get<faceView&>(fv)[3]->worldPosition;
		centerPositionOfFace /= 4;
		std::get<float>(fv) = SimpleMath::Vector3::DistanceSquared(this->cameraPosition, centerPositionOfFace);
	}

	// Sort by distance
	std::sort(sortedFaceViews.begin(), sortedFaceViews.end(), [](const auto& a, const auto& b) { return std::get<float>(a) > std::get<float>(b); });


	// Render faces (sorted)
	for (auto& sf : sortedFaceViews)
	{
		renderFace(std::get<faceView&>(sf), std::get<uint32_t>(sf));
	}

	// Render edges (not sorted)
	for (auto& e : model.edgeViews)
	{
		renderEdge(e, outlineColor);
	}


}
#endif


// explicit template instantiation
template class Renderer3DImpl<GameState::Value::Halo1>;
template class Renderer3DImpl<GameState::Value::Halo2>;
template class Renderer3DImpl<GameState::Value::Halo3>;
template class Renderer3DImpl<GameState::Value::Halo3ODST>;
template class Renderer3DImpl<GameState::Value::HaloReach>;
template class Renderer3DImpl<GameState::Value::Halo4>;