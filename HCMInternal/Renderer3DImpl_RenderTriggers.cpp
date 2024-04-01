#include "pch.h"
#include "Renderer3DImpl.h"
#include "directxtk\SimpleMath.h"
#include "RenderTextHelper.h"

//#include "Mathematics\IntrSegment3Triangle3.h"

bool worldPointAisBetweenBC(const SimpleMath::Vector3& a, const SimpleMath::Vector3& b, const SimpleMath::Vector3& c)
{
	float distanceBC = SimpleMath::Vector3::DistanceSquared(b, c);
	return (SimpleMath::Vector3::DistanceSquared(a, b) <= distanceBC) && (SimpleMath::Vector3::DistanceSquared(a, c) <= distanceBC);
}


#define v3tv2(x) static_cast<SimpleMath::Vector2>(x)

template<GameState::Value mGame>
std::vector<SimpleMath::Vector3> Renderer3DImpl<mGame>::clipFaceToFrustum(const faceView& face)
{
	// Construct 4 line segments from the face, feed them to edge clippedToFrustum
	std::array<std::optional<std::pair<SimpleMath::Vector3, SimpleMath::Vector3>>, 4> lineSegments = {
	 clipLineSegmentToFrustum(face[0]->worldPosition, face[1]->worldPosition),
	 clipLineSegmentToFrustum(face[1]->worldPosition, face[2]->worldPosition),
	 clipLineSegmentToFrustum(face[2]->worldPosition, face[3]->worldPosition),
	 clipLineSegmentToFrustum(face[3]->worldPosition, face[0]->worldPosition)
	};

	std::vector<SimpleMath::Vector3> out;

	for (auto& lineSegment : lineSegments)
	{
		if (lineSegment.has_value())
		{
			out.emplace_back(lineSegment.value().first);
			out.emplace_back(lineSegment.value().second);
		}
	}
	return out;
}

template<GameState::Value mGame>
std::optional<std::pair<SimpleMath::Vector3, SimpleMath::Vector3>> Renderer3DImpl<mGame>::clipLineSegmentToFrustum(const SimpleMath::Vector3& start, const SimpleMath::Vector3& end)
{


	/*
	Clips a line segment to only the portion contained inside the view frustum.
	A line segment may have 0, 1, or 2 intersections with the view frustum. 
	We need to find these intersection points and construct the new line from them.
	*/


	bool startInside = pointOnScreen(start);
	bool endInside = pointOnScreen(end);

	// If both points are inside the frustum, we can just return the original line.
	if (startInside && endInside)
	{
		return std::pair<SimpleMath::Vector3, SimpleMath::Vector3>( start, end );
	}
	

	// Iterate over each face of the view frustum, finding the intersection point if there is one. We can stop iterating once we find 2.

	std::optional<SimpleMath::Vector3> intersectionPoint1;
	std::optional<SimpleMath::Vector3> intersectionPoint2;

	for (auto& frustumFace : this->frustumViewWorldFaces)
	{
		/*
		First, check if the infinite line describes by the line-segment intersects with the infinite plane described by the frustum face.
		If so, check if the intersection point lies on the finite line segment.
		If so, check if the intersection point lies on the finite frustum face.
		*/

		auto frustumPlane = DirectX::XMPlaneFromPoints(frustumFace[0], frustumFace[1], frustumFace[2]);
		auto linePoint1 = DirectX::XMVECTOR{ start.x, start.y, start.z, 0.f };
		auto linePoint2 = DirectX::XMVECTOR{end.x, end.y, end.z, 0.f};


		PLOG_DEBUG << frustumFace[0];
		PLOG_DEBUG << frustumFace[1];
		PLOG_DEBUG << frustumFace[2];

		// XMPlaneIntersectLine sets all components of vector to NAN if there is no intersection (Ie line is pefectly parralel to plane)
		SimpleMath::Vector3 intersectionPoint = DirectX::XMPlaneIntersectLine(frustumPlane, linePoint1, linePoint2);
		if (std::isnan(intersectionPoint.x))
		{
			PLOG_ERROR << "this should rarely if ever happen";
			// BOY this is sure happening a lot
			// like a LOT. wtf.
			continue;
		}


		if (
			worldPointAisBetweenBC(intersectionPoint, start, end) // Does the intersection point lie on the line segment? ps this only works cause the line segment is colinear to the infinite line.
			&& worldPointAisBetweenBC(intersectionPoint, frustumFace[0], frustumFace[2]) // Is the intersection point between both the diagonals of the frustum face?
			&& worldPointAisBetweenBC(intersectionPoint, frustumFace[1], frustumFace[3]) // ps this only works because the face is coplaner to the infinite plane
			)
		{
			PLOG_INFO << "YOOOOOOOOOOOOO";
			// Then we have a real intersection
			if (intersectionPoint1.has_value() == false) // is this the first intersection we've found?
			{
				intersectionPoint1 = intersectionPoint;
			}
			else  // it must be the second intersection point we've found
			{
				intersectionPoint2 = intersectionPoint;
				break; // we can't have more than two intersection points; bail out of the loop.
			}
		}
	}

	int intersectionCount = 0;
	if (intersectionPoint1.has_value())
		intersectionCount++;
	if (intersectionPoint2.has_value())
		intersectionCount++;

	if (intersectionCount == 0)
	{
		// No intersections; the line segment must lie entirely outside the view frustum, so there's no valid line to return.
		//assert(startInside == false && endInside == false);
		if (startInside || endInside)
		{
			//PLOG_ERROR << "no intersectiosn but a point was inside the frustum";
		}
		return std::nullopt;
	}
	else if (intersectionCount == 1)
	{
		// Need to check which of the two line points is inside the frustum (should be only one of them that is), return the line from it to the intersection point.
		if (startInside)
		{
			return std::pair<SimpleMath::Vector3, SimpleMath::Vector3>(start, intersectionPoint1.value());
		}
		else
		{
			//assert(endInside);
			return std::pair<SimpleMath::Vector3, SimpleMath::Vector3>(end, intersectionPoint1.value());
		}
	}
	else // two intersections
	{
		//assert(startInside == false && endInside == false);
		return std::pair<SimpleMath::Vector3, SimpleMath::Vector3>(intersectionPoint1.value(), intersectionPoint2.value());
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
		std::vector<ImVec2> newPolygonScreenPositions;
		for (auto& v : newPolygon)
		{

			newPolygonScreenPositions.emplace_back(v3tv2(worldPointToScreenPosition(v, false)));
		}




		// draw it!
		ImGui::GetBackgroundDrawList()->AddConvexPolyFilled(newPolygonScreenPositions.data(), newPolygonScreenPositions.size(), color);

		// debug vertices
		if (debugVertices)
		{
			auto green = ImGui::ColorConvertFloat4ToU32(SimpleMath::Vector4(0.f, 1.f, 0.f, 1.f));
			for (int i = 0; i < newPolygonScreenPositions.size(); i++)
			{
				RenderTextHelper::drawCenteredOutlinedText(std::format("{}", i).c_str(), newPolygonScreenPositions[i], green, 10.f);
			}
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

		if (newLine.has_value())
		{
			// calulate the new lines screen positions
			std::pair<ImVec2, ImVec2> newLineScreenPositions = {
				v3tv2(worldPointToScreenPosition(newLine.value().first, false)),
				v3tv2(worldPointToScreenPosition(newLine.value().second, false)),
			};

			// draw it!
			ImGui::GetBackgroundDrawList()->AddLine(newLineScreenPositions.first, newLineScreenPositions.second, color);
		}
		
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