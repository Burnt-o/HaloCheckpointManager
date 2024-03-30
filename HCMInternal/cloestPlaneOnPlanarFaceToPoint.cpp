#include "pch.h"
#include "closestPointOnPlanarFaceToPoint.h"

bool PointAIsInbetweenBandC(const SimpleMath::Vector3& a, const SimpleMath::Vector3& b, const SimpleMath::Vector3& c)
{
	float distanceBC = SimpleMath::Vector3::DistanceSquared(b, c);
	return (SimpleMath::Vector3::DistanceSquared(a, b) <= distanceBC) && (SimpleMath::Vector3::DistanceSquared(a, c) <= distanceBC);
}


// for finite lines only
SimpleMath::Vector3 closestPointOnLineSegmentToPoint(const SimpleMath::Vector3& worldPoint, const SimpleMath::Vector3& lineStartPoint, const SimpleMath::Vector3& lineEndPoint)
{
	// need to project worldPoint onto the line segment
	// https://stackoverflow.com/questions/47481774/getting-point-on-line-segment-that-is-closest-to-another-point
	auto AB = lineEndPoint - lineStartPoint;
	auto AP = worldPoint - lineStartPoint;
	float lengthSqrAB = AB.LengthSquared();
	
	// interpolation represents how far along the line segment the point is, where 0 is lineStartPoint, 1 is lineEndPoint. 
	float lineInterpolation = AB.Dot(AP) / lengthSqrAB;
	
	// Since this is a segment, not an infinite line, clamp it to 0..1
	lineInterpolation = std::clamp(lineInterpolation, 0.f, 1.f);

	return lineStartPoint + (lineInterpolation * AB);


}


float distancePointPlane(const SimpleMath::Plane& plane, const SimpleMath::Vector3& worldPoint)
{
	// https://studiofreya.com/3d-math-and-physics/distance-from-point-to-plane-implementation/
	return plane.Normal().Dot(worldPoint) / plane.Normal().Dot(plane.Normal()); 
}

/// <summary>
/// Returns closest point on a 3D finite face to a coplanar point (ie a point that lies on the same infinite plane as the face)
/// </summary>
/// <param name="face"></param>
/// <param name="coplanarPoint">Must lie on the same plane as the face</param>
/// <returns></returns>
SimpleMath::Vector3 closestPointToCoplanarFace(const std::array<SimpleMath::Vector3, 4>& face, const SimpleMath::Vector3& coplanarPoint)
{
	// 	https://forum.unity.com/threads/finding-the-closest-point-from-a-point-to-a-4-points-face.786248/

	// Get closest vertex of face to the coplanar point
	const SimpleMath::Vector3* closestVertex = nullptr;
	int closestVertexIndex = -1;
	{
		float closestDistanceSoFar = std::numeric_limits<float>::max();
		for (int i = 0; i < 4; i++)
		{
			float thisVertexDistance = SimpleMath::Vector3::DistanceSquared(face[i], coplanarPoint);
			if (thisVertexDistance < closestDistanceSoFar)
			{
				closestDistanceSoFar = thisVertexDistance;
				closestVertexIndex = i;

			}

		}
		closestVertex = &face[closestVertexIndex];
	}

	// On the two edges adjacent to the closest vertex, find the closest point to the coplanar point.
	const SimpleMath::Vector3* prevAdjacentVertex = &face[(closestVertexIndex + 1) % 4];
	const SimpleMath::Vector3* nextAdjacentVertex = &face[(closestVertexIndex - 1 + 4) % 4]; // Do not modulo negative numbers for the love of god

	const SimpleMath::Vector3 prevProjectedPoint = closestPointOnLineSegmentToPoint(coplanarPoint , *prevAdjacentVertex, *closestVertex);
	const SimpleMath::Vector3 nextProjectedPoint = closestPointOnLineSegmentToPoint(coplanarPoint , *prevAdjacentVertex, *closestVertex);

	// If the coplanarPoint is in-between the closest-edge-point & the adjacent vertex, for both edges, then the coplanarPoint point is inside the face.
	if (PointAIsInbetweenBandC(coplanarPoint, prevProjectedPoint, *prevAdjacentVertex) && PointAIsInbetweenBandC(coplanarPoint, nextProjectedPoint, *nextAdjacentVertex))
	{
		// the coplanarPoint already lies on the face.
		return coplanarPoint;
	}
	else
	{
		// The coplanarPoint lies outside the face. The closest point therefore must be on one of the edges of the face.
		// In fact it must be on one of the two edges adjacent to the closest vertex. And we already calculated the closest points on those edges. We just have to return whichever is closer.
		return (SimpleMath::Vector3::DistanceSquared(prevProjectedPoint, coplanarPoint) < SimpleMath::Vector3::DistanceSquared(nextProjectedPoint, coplanarPoint))
			? prevProjectedPoint
			: nextProjectedPoint;
	}

}



SimpleMath::Vector3 closestPointOnPlanarFaceToPoint(const std::array<SimpleMath::Vector3, 4>& face, const SimpleMath::Plane& plane, const SimpleMath::Vector3& worldPoint)
{

	// Step 1: Find distance from plane to the worldPoint.
	float distancePlaneToWorldPoint = distancePointPlane(plane, worldPoint);

	// Step 2: Find closest point on the plane to the worldPoint. 	// https://studiofreya.com/3d-math-and-physics/distance-from-point-to-plane-implementation/
	SimpleMath::Vector3 closestPlanePoint = worldPoint - (distancePlaneToWorldPoint * plane.Normal());

	// Step 3: Find closest point on face to the point on the plane.
	return closestPointToCoplanarFace(face, closestPlanePoint);



}