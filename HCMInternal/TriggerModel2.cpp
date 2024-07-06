#include "pch.h"
#include "TriggerModel2.h"
#include <ranges>
#include "earcut.hpp"


// https://stackoverflow.com/questions/217578/how-can-i-determine-whether-a-2d-point-is-within-a-polygon/2922778#2922778
int pnpoly(const std::vector<SimpleMath::Vector2>& verts, SimpleMath::Vector2 testPoint)
{
	bool c = false;
	int i, j;
	for (i = 0, j = verts.size() - 1; i < verts.size(); j = i++) {
		if (((verts[i].y > testPoint.y) != (verts[j].y > testPoint.y)) &&
			(testPoint.x < (verts[j].x - verts[i].x) * (testPoint.y - verts[i].y) / (verts[j].y - verts[i].y) + verts[i].x))
			c = !c;
	}
	return c;
}




bool TriggerModel2::pointInside(const SimpleMath::Vector3& worldPoint)
{
	if (boundingBox.has_value())
		return boundingBox.value().Contains(worldPoint) != DirectX::ContainmentType::DISJOINT;
	else if (sectorHeight.has_value() && sectorPoints2D.has_value())
	{
		// sector type!
		// Sectors are 2.5d shapes ; they consist of a perfectly horizontal n-gon, extruded up by some height
		// Importantly, they cannot be rotated! And their side faces are sheer vertical.

		// So if the worldPoint.z isn't within vertices[0].z to vertices[0].z + sectorHeight, the answer is false

		if (worldPoint.z < vertices[0].position.z || worldPoint.z > vertices[0].position.z + sectorHeight.value())
			return false;



		assert(false && "Turns out my assumption about the bases sharing the same z position was incorrect. They can indeed differ. ");
		// so how the fuck am I gonna perform this test then? 



		// Now our problem is just a 2D one! using an algo I stole from the internet
		return pnpoly(sectorPoints2D.value(), { worldPoint.x, worldPoint.y });

	}
	else
		throw HCMRuntimeException("EH?!");
}




// bounding box constructor
TriggerModel2::TriggerModel2(SimpleMath::Vector3 position, SimpleMath::Vector3 extents, SimpleMath::Vector3 forward, SimpleMath::Vector3 up)
{
	// Note: position is actually the location of the mininum x/y/z, not the center.
	auto center = position + (extents / 2);

	auto resizeTransform = SimpleMath::Matrix::CreateScale(extents);

	auto rotateTransform = SimpleMath::Matrix::CreateTranslation(extents / 2) // offset to pivot point (corner)
		* SimpleMath::Matrix::CreateFromQuaternion(SimpleMath::Quaternion::FromToRotation(SimpleMath::Vector3::UnitX, forward)) // rotate forward
		* SimpleMath::Matrix::CreateFromQuaternion(SimpleMath::Quaternion::FromToRotation(SimpleMath::Vector3::UnitZ, up)) // rotate up
		* SimpleMath::Matrix::CreateTranslation(-1.f * (extents / 2)); // unoffset to pivot point

	auto translateTransform = SimpleMath::Matrix::CreateTranslation(center);


	auto transformation = SimpleMath::Matrix::CreateWorld(SimpleMath::Vector3::Zero, SimpleMath::Vector3::UnitX, SimpleMath::Vector3::UnitZ) // world at origin
		* resizeTransform // resize
		* rotateTransform // rotate
		* translateTransform; // translate



	// begin with unit cube vertices
	std::array<SimpleMath::Vector3, 8> cubeVertices =
	{
		// from DirectX::BoundingOrientedBox constructor
		/*
			{ { { -1.0f, -1.0f,  1.0f, 0.0f } } },
			{ { {  1.0f, -1.0f,  1.0f, 0.0f } } },
			{ { {  1.0f,  1.0f,  1.0f, 0.0f } } },
			{ { { -1.0f,  1.0f,  1.0f, 0.0f } } },
			{ { { -1.0f, -1.0f, -1.0f, 0.0f } } },
			{ { {  1.0f, -1.0f, -1.0f, 0.0f } } },
			{ { {  1.0f,  1.0f, -1.0f, 0.0f } } },
			{ { { -1.0f,  1.0f, -1.0f, 0.0f } } },

		*/
		// above is span -1 .. 1, but unit cube is actually -0.5f to 0.5f

		SimpleMath::Vector3{ -0.5f, -0.5f,  0.5f },
		SimpleMath::Vector3{  0.5f, -0.5f,  0.5f },
		SimpleMath::Vector3{  0.5f,  0.5f,  0.5f },
		SimpleMath::Vector3{ -0.5f,  0.5f,  0.5f },
		SimpleMath::Vector3{ -0.5f, -0.5f, -0.5f },
		SimpleMath::Vector3{  0.5f, -0.5f, -0.5f },
		SimpleMath::Vector3{  0.5f,  0.5f, -0.5f },
		SimpleMath::Vector3{ -0.5f,  0.5f, -0.5f },
	};

	// transform unit cube vertices by transformation
	std::ranges::transform(cubeVertices, cubeVertices.begin(), [transformation](const auto& v) { return SimpleMath::Vector3::Transform(v, transformation); });

	// create bounding box
	boundingBox = DirectX::BoundingOrientedBox(SimpleMath::Vector3::Zero, extents / 2, SimpleMath::Quaternion::Identity);
	boundingBox.value().Transform(boundingBox.value(), rotateTransform);
	boundingBox.value().Transform(boundingBox.value(), translateTransform);

	// copy vertices
	vertices.insert_range(vertices.begin(), cubeVertices);

	// setup edge indices
	edgeIndices = {
		// Near face.
		0, 1,
		1, 2,
		2, 3,
		3, 4,

		// Far face.
		4, 5,
		5, 6,
		6, 7,
		7, 8,

		// Connect 'em together.
		0, 4,
		1, 5,
		2, 6,
		3, 7,
	};


	// setup tri indices (each face is just two tris)
	triangleIndices = {
		  //Top
		  2, 6, 7,
		  2, 3, 7,

		  //Bottom
		  0, 4, 5,
		  0, 1, 5,

		  //Left
		  0, 2, 6,
		  0, 4, 6,

		  //Right
		  1, 3, 7,
		  1, 5, 7,

		  //Front
		  0, 2, 3,
		  0, 1, 3,

		  //Back
		  4, 6, 7,
		  4, 5, 7
	};


}


// sector constructor
TriggerModel2::TriggerModel2(std::vector<SimpleMath::Vector3> sectorPointsBottom, float height)
{
	// setup sectorHeight and sectorPoint2D for containment tests
	sectorHeight = height;
	sectorPoints2D = std::vector<SimpleMath::Vector2>();
	std::ranges::transform(sectorPointsBottom, sectorPoints2D.value().begin(), [](const auto& v3D) { return SimpleMath::Vector2{v3D.x, v3D.y}; });


	size_t sectorPointCount = sectorPointsBottom.size();

	// vertices consists of sectorPoints, and then sectorPoints again but with z += height.
	std::vector<SimpleMath::Vector3> sectorPointsTop;
	std::ranges::transform(sectorPointsBottom, sectorPointsTop.begin(), [height](const auto& bot) { return SimpleMath::Vector3{ bot.x, bot.y, bot.z + height }; });
	for (auto& sp : sectorPointsBottom)
	{
		vertices.push_back(sp);
	}
	for (auto& sp : sectorPointsTop)
	{
		vertices.push_back(sp);
	}



	// setup edge indices
	// vertical edges, just connect bottom to top
	for (int i = 0; i < sectorPointCount; i++)
	{
		edgeIndices.push_back(i);
		edgeIndices.push_back(i + sectorPointCount);
	}

	// bottom face edges
	for (int i = 0; i < sectorPointCount; i++)
	{
		edgeIndices.push_back(i);
		edgeIndices.push_back((i + 1) % sectorPointCount);
	}

	// top face edges
	for (int i = 0; i < sectorPointCount; i++)
	{
		edgeIndices.push_back(i + sectorPointCount);
		edgeIndices.push_back(((i + 1) % sectorPointCount) + sectorPointCount);
	}

	// setup triangle indices
	// side faces. easy to do since they're just sheer vertical square faces

	for (int i = 0; i < sectorPointCount; i++)
	{
		triangleIndices.push_back(i);
		triangleIndices.push_back((i + 1) % sectorPointCount);
		triangleIndices.push_back(i + sectorPointCount);

		triangleIndices.push_back(i);
		triangleIndices.push_back((i + 1) % sectorPointCount);
		triangleIndices.push_back(((i + 1) % sectorPointCount) + sectorPointCount);

	}

	// bottom and top faces.. not trivial. Need to nick this neat ear-clipping triangulation library
	std::vector<std::vector<SimpleMath::Vector3>> polygon;
	polygon.push_back(sectorPointsBottom); // doesn't matter if we use bottom or top since this is a 2d calc 

	// Run tessellation
	// Returns array of indices that refer to the vertices of the input polygon.
	// Three subsequent indices form a triangle. Output triangles are clockwise.
	std::vector<uint16_t> triangulatedIndices = mapbox::earcut<uint16_t>(polygon);

	// fill up the bottom face indices
	for (auto& index : triangulatedIndices)
	{
		triangleIndices.push_back(index);
	}

	// fill up the top face indices
	for (auto& index : triangulatedIndices)
	{
		triangleIndices.push_back(index + sectorPointCount);
	}

}



namespace mapbox {
	namespace util {

		template <>
		struct nth<0, SimpleMath::Vector3> {
			inline static auto get(const SimpleMath::Vector3& t) {
				return t.x;
			};
		};
		template <>
		struct nth<1, SimpleMath::Vector3> {
			inline static auto get(const SimpleMath::Vector3& t) {
				return t.y;
			};
		};

	} // namespace util
} // namespace mapbox