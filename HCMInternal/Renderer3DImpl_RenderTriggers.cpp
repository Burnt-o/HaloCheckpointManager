#include "pch.h"
#include "Renderer3DImpl.h"
#include "directxtk\SimpleMath.h"
#include "RenderTextHelper.h"

#define v3tv2(x) static_cast<SimpleMath::Vector2>(x)

template<GameState::Value mGame>
std::array<SimpleMath::Vector3, 8> Renderer3DImpl<mGame>::clipFaceToFrustum(const faceView& face)
{
	// Construct 4 line segments from the face, feed them to edge clippedToFrustum
	std::array<std::pair<SimpleMath::Vector3, SimpleMath::Vector3>, 4> lineSegments =
	{
		clipLineSegmentToFrustum(face[0]->worldPosition, face[1]->worldPosition),
		clipLineSegmentToFrustum(face[1]->worldPosition, face[2]->worldPosition),
		clipLineSegmentToFrustum(face[2]->worldPosition, face[3]->worldPosition),
		clipLineSegmentToFrustum(face[3]->worldPosition, face[0]->worldPosition),
	};

	// I wish std::pair was guarenteed to be contiguous, in which case we could cheat and just cast from lineSegments to output.
	// Alas we must copy. Maybe the compiler will optimize it for us.
	return { lineSegments[0].first, lineSegments[0].second, lineSegments[1].first, lineSegments[1].second, lineSegments[2].first, lineSegments[2].second , lineSegments[3].first, lineSegments[3].second };

}

template<GameState::Value mGame>
std::pair<SimpleMath::Vector3, SimpleMath::Vector3> Renderer3DImpl<mGame>::clipLineSegmentToFrustum(const SimpleMath::Vector3& start, const SimpleMath::Vector3& end)
{
	// https://stackoverflow.com/questions/77836/how-do-i-clip-a-line-segment-against-a-frustum

	std::optional<DirectX::XMVECTOR> intersect1;
	std::optional<DirectX::XMVECTOR> intersect2;
	for (auto& frustumPlane : this->frustumViewWorldPlanes)
	{
		// PROBLEM: this is testing against PLANE, not FACE. do we really need to add a extra check for if the intersect point lies on the face? ugh.
		auto maybeIntersect = DirectX::XMPlaneIntersectLine(frustumPlane, start, end);
		if (DirectX::XMVector3IsNaN(maybeIntersect) == false) // have we got a intersect?
		{
			if (intersect1.has_value() == false) // is this the first one?
			{
				intersect1 = maybeIntersect;
			}
			else // it must be the second one
			{
				intersect2 = maybeIntersect;
				break; // we can exit the for loop early- we will only ever have at most two intersects. 
			}

		}
	}


	if (intersect1.has_value() && intersect2.has_value())
	{
		// if we have two intersections already, we're done.
		return std::pair<SimpleMath::Vector3, SimpleMath::Vector3>(intersect1.value(), intersect2.value());
	}
	else if (intersect1.has_value() == false && intersect2.has_value() == false)
	{
		// if there were no intersections, the line must be fully inside or fully outside the frustum.
		// return the original line. If it's fully outside then it shouldn't interfere once rendered.. I hope. This might be bad for faces er.
		// Maybe we should be returning std::optional, and nullopt if we have both (no intersections) AND (neither point inside the frustum).
		// How will faces deal with null opt?  uh connect the two adjacent edges i guess. 
		return std::pair<SimpleMath::Vector3, SimpleMath::Vector3>(start, end);
	}
	else
	{
		// we only have one intersection so far. This means the line segment is partially inside and partially outside the frustum.
		// To put it another way, EITHER the start or end point are inside the frustum, but not both, and not neither. 
		// And the line we want to output is a line starting at whichever point is inside the frustum, and ending at the intersection point.
		if (this->frustumViewWorld.Contains(start))
		{
			// output start -> intersection
			return std::pair<SimpleMath::Vector3, SimpleMath::Vector3>(start, intersect1.value());
		}
		else
		{
#ifdef HCM_DEBUG
			if (this->frustumViewWorld.Contains(end) == false)
				PLOG_ERROR << "clipLineSegmentToFrustum: One intersection found but neither point was inside the frustum?! wtf";
#endif
			// output intersection -> end
			return std::pair<SimpleMath::Vector3, SimpleMath::Vector3>(intersect1.value(), end);
		}
	}


}


template<GameState::Value mGame>
void  Renderer3DImpl<mGame>::renderFace(const faceView& face, uint32_t color, bool debugVertices)
{
#ifdef HCM_DEBUG
	LOG_ONCE_CAPTURE(PLOG_DEBUG << "face[0]->worldPosition: " << wp, wp = face[0]->worldPosition);
#endif


	// Test if all of the points are inside the camera frustum
	if (std::all_of(face.begin(), face.end(), [this](const VertexInfo* v) { return this->frustumViewWorld.Contains(v->worldPosition); }))
	{
		// then we don't need to do anything fancy
		// draw it!
		ImGui::GetBackgroundDrawList()->AddQuadFilled(v3tv2(face[0]->screenPosition), v3tv2(face[1]->screenPosition), v3tv2(face[2]->screenPosition), v3tv2(face[3]->screenPosition), color);

		// debug vertices
		if (debugVertices)
		{
			auto blue = ImGui::ColorConvertFloat4ToU32(SimpleMath::Vector4(0.f, 0.f, 1.f, 1.f));
			RenderTextHelper::drawCenteredOutlinedText("0", v3tv2(face[0]->screenPosition), blue, 10.f);
			RenderTextHelper::drawCenteredOutlinedText("1", v3tv2(face[1]->screenPosition), blue, 10.f);
			RenderTextHelper::drawCenteredOutlinedText("2", v3tv2(face[2]->screenPosition), blue, 10.f);
			RenderTextHelper::drawCenteredOutlinedText("3", v3tv2(face[3]->screenPosition), blue, 10.f);
		}

	}
	else
	{
		// uh oh, at least one of the vertexes lies outside the frustum! We'll need to construct a new planar polygon that is the intersection of the original face AND the view frustum.
		// This new polygon may have between 3 and 8 vertices.

		// get the clipped polygon
		auto newPolygon = clipFaceToFrustum(face);

		// calculate the new screen positions of this polygon
		std::array<ImVec2, 8> newPolygonScreenPositions;
		for (int i = 0; i < 8; i++)
		{
			newPolygonScreenPositions[i] = v3tv2(worldPointToScreenPosition(newPolygon[i], false));
		}




		// draw it!
		ImGui::GetBackgroundDrawList()->AddConvexPolyFilled(newPolygonScreenPositions.data(), newPolygonScreenPositions.size(), color);

		// debug vertices
		if (debugVertices)
		{
			auto green = ImGui::ColorConvertFloat4ToU32(SimpleMath::Vector4(0.f, 1.f, 0.f, 1.f));
			RenderTextHelper::drawCenteredOutlinedText("0", newPolygonScreenPositions[0], green, 10.f);
			RenderTextHelper::drawCenteredOutlinedText("1", newPolygonScreenPositions[1], green, 10.f);
			RenderTextHelper::drawCenteredOutlinedText("2", newPolygonScreenPositions[2], green, 10.f);
			RenderTextHelper::drawCenteredOutlinedText("3", newPolygonScreenPositions[3], green, 10.f);
			RenderTextHelper::drawCenteredOutlinedText("4", newPolygonScreenPositions[4], green, 10.f);
			RenderTextHelper::drawCenteredOutlinedText("5", newPolygonScreenPositions[5], green, 10.f);
			RenderTextHelper::drawCenteredOutlinedText("6", newPolygonScreenPositions[6], green, 10.f);
			RenderTextHelper::drawCenteredOutlinedText("7", newPolygonScreenPositions[7], green, 10.f);
		}
	}

}


template<GameState::Value mGame>
void  Renderer3DImpl<mGame>::renderEdge(const edgeView& edge, uint32_t color)
{
	bool startVertexInside = this->frustumViewWorld.Contains(edge[0]->worldPosition);
	bool endVertexInside = this->frustumViewWorld.Contains(edge[1]->worldPosition);

	// Test if all of the points are inside the camera frustum
	if (startVertexInside && endVertexInside)
	{
		// then we don't need to do anything fancy, render the line!
		ImGui::GetBackgroundDrawList()->AddLine(v3tv2(edge[0]->screenPosition), v3tv2(edge[1]->screenPosition), color);
	}
	else
	{
		// uh oh, at least one of the vertexes lies outside the frustum! We'll need to construct a new line that is the intersection of the original edge AND the view frustum.
	// This new line will of course be a segment of the original line.

	// get the clipped line
		auto newLine = clipLineSegmentToFrustum(edge[0]->worldPosition, edge[1]->worldPosition);

		// calulate the new lines screen positions
		std::pair<ImVec2, ImVec2> newLineScreenPositions = {
			v3tv2(worldPointToScreenPosition(newLine.first, false)),
			v3tv2(worldPointToScreenPosition(newLine.second, false)),
		};

		// draw it!
		ImGui::GetBackgroundDrawList()->AddLine(newLineScreenPositions.first, newLineScreenPositions.second, color);
	}



}


template<GameState::Value mGame>
void Renderer3DImpl<mGame>::renderTriggerModel(TriggerModel& model, uint32_t fillColor, uint32_t outlineColor)
{

	// Cull if none of the trigger is visible.
	if (this->frustumViewWorld.Contains(model.box) == false) return;

	// Calculate initial screen positions. Used if face/edge is entirely within frustum.
	for (auto& v : model.vertices)
	{
		v.screenPosition = worldPointToScreenPosition(v.worldPosition, false);
	}

	// Render faces
	for (auto& fv : model.faceViews)
	{
		renderFace(fv, fillColor);
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

	// Calculate initial screen positions. Used if face/edge is entirely within frustum.
	for (auto& v : model.vertices)
	{
		v.screenPosition = worldPointToScreenPosition(v.worldPosition, false);
	}

	if (GetKeyState('8') & 0x8000)
	{
		// only render purple face. Enable debugVertices flag.
		renderFace(model.faceViews[4], ImGui::ColorConvertFloat4ToU32(SimpleMath::Vector4(1.f, 0.f, 1.f, 1.f)), true);

		// Render edges (not sorted)
		for (auto& e : model.edgeViews)
		{
			renderEdge(e, outlineColor);
		}

		return;
	}

	// Order faces by world distance from camera. And assign each one a color.
	typedef std::tuple<faceView, float, uint32_t> sortedFaceView;
	std::array<sortedFaceView, 6> sortedFaceViews = {
		sortedFaceView(model.faceViews[0], 0.f, ImGui::ColorConvertFloat4ToU32(SimpleMath::Vector4(1.f, 0.f, 0.f, 1.f))),
		sortedFaceView(model.faceViews[1], 0.f, ImGui::ColorConvertFloat4ToU32(SimpleMath::Vector4(0.f, 1.f, 0.f, 1.f))),
		sortedFaceView(model.faceViews[2], 0.f, ImGui::ColorConvertFloat4ToU32(SimpleMath::Vector4(0.f, 0.f, 1.f, 1.f))),
		sortedFaceView(model.faceViews[3], 0.f, ImGui::ColorConvertFloat4ToU32(SimpleMath::Vector4(1.f, 1.f, 0.f, 1.f))),
		sortedFaceView(model.faceViews[4], 0.f, ImGui::ColorConvertFloat4ToU32(SimpleMath::Vector4(1.f, 0.f, 1.f, 1.f))),
		sortedFaceView(model.faceViews[5], 0.f, ImGui::ColorConvertFloat4ToU32(SimpleMath::Vector4(0.f, 1.f, 1.f, 1.f))),
	};

	// Measure the world distance from camera:
	for (auto& sfv : sortedFaceViews)
	{
		// Center position of face is just all 4 vertexes added up and div by 4.
		auto& fv = std::get<faceView>(sfv);
		auto centerPositionOfFace = fv[0]->worldPosition + fv[1]->worldPosition + fv[2]->worldPosition + fv[3]->worldPosition;
		centerPositionOfFace = centerPositionOfFace / 4.f;
		std::get<float>(sfv) = SimpleMath::Vector3::DistanceSquared(this->cameraPosition, centerPositionOfFace);
	}

	// Sort by distance
	std::sort(sortedFaceViews.begin(), sortedFaceViews.end(), [](const auto& a, const auto& b) { return std::get<float>(a) > std::get<float>(b); });


	if (GetKeyState('8') & 0x8000)
	{
		// only render purple face
	}


	// Render faces (sorted)
	for (auto& sf : sortedFaceViews)
	{
		renderFace(std::get<faceView>(sf), std::get<uint32_t>(sf));
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