#include "pch.h"
#include "TriggerModel.h"
#include <ranges>
#include "earcut.hpp"





// bounding box constructor
TriggerModel::TriggerModel(std::string triggerName, SimpleMath::Vector3 position, SimpleMath::Vector3 extents, SimpleMath::Vector3 forward, SimpleMath::Vector3 up)
	: label(triggerName),
	m_isSectorType(false)
{
	// Note: position is actually the location of the mininum x/y/z, not the center.
	auto center = position + (extents / 2);

	auto resizeTransform = SimpleMath::Matrix::CreateScale(extents);

	// NOTE! Rotations occur around the "pivot point" of a trigger, not it's center!
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
		// above is span -1 .. 1, but a unit cube is actually -0.5f to 0.5f (side lengths of 1)

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

	// Make box
	box = DirectX::BoundingOrientedBox(SimpleMath::Vector3::Zero, extents / 2, SimpleMath::Quaternion::Identity);
	box.Transform(box, rotateTransform);
	box.Transform(box, translateTransform);



	// copy vertices
	vertices.insert_range(vertices.begin(), cubeVertices);

	// setup edge indices
	edgeIndices = {
		// Near face.
		0, 1,
		1, 2,
		2, 3,
		3, 0,

		// Far face.
		4, 5,
		5, 6,
		6, 7,
		7, 4,

		// Connect 'em together.
		0, 4,
		1, 5,
		2, 6,
		3, 7,
	};		
	
		//     Near    Far
		//    0----1  4----5
		//    |    |  |    |
		//    |    |  |    |
		//    3----2  7----6

	// setup tri indices (each face is just two tris)
	triangleIndices = {
		// Top face
		0, 1, 4,
		1, 4, 5,

		// Bottom face
		3, 2, 7,
		2, 7, 6,

		// Left face
		3, 0, 7,
		0, 7, 4,

		// Right face
		2, 1, 6,
		1, 6, 5,

		// Front face ttt
		0, 1, 2,
		0, 2, 3,

		// Back face
		4, 5, 6,
		4, 6, 7
	};


}


// sector constructor
TriggerModel::TriggerModel(std::string triggerName, std::vector<SimpleMath::Vector3> sectorPointsBottom, float height)
	: label(triggerName),
	m_isSectorType(true)
{

	size_t sectorPointCount = sectorPointsBottom.size();

	// vertices consists of sectorPoints, and then sectorPoints again but with z += height.
	std::vector<SimpleMath::Vector3> sectorPointsTop = sectorPointsBottom; // copy
	std::ranges::transform(sectorPointsTop, sectorPointsTop.begin(), [height](const auto& v) { return SimpleMath::Vector3{ v.x, v.y, v.z + height }; }); // add height

	for (auto& sp : sectorPointsBottom)
	{
		vertices.push_back(sp);
	}
	for (auto& sp : sectorPointsTop)
	{
		vertices.push_back(sp);
	}
	// The order we did that matters. We can access the bottom vertices with index 0 through (sectorPointCount - 1).
	// We can access the top vertices with index sectorPointCount through (2 * sectorPointCount - 1).
	// and we can just add sectorPointCount to a bottom vertex index to get the top vertex index directly above it.


	// setup edge indices
	// vertical edges, just connect bottom vert to top vert directly above it.
	for (int i = 0; i < sectorPointCount; i++)
	{
		edgeIndices.push_back(i); // bottom
		edgeIndices.push_back(i + sectorPointCount); // top
	}

	// bottom face edges. just connect the initial polyline
	for (int i = 0; i < sectorPointCount; i++)
	{
		edgeIndices.push_back(i);
		edgeIndices.push_back((i + 1) % sectorPointCount); // modulo so the last vertex connects back to the first.
	}

	// top face edges. as above but add sectorPointCount to everything, connecting the polyline on top.
	for (int i = 0; i < sectorPointCount; i++)
	{
		edgeIndices.push_back(i + sectorPointCount);
		edgeIndices.push_back(((i + 1) % sectorPointCount) + sectorPointCount);
	}

	// setup triangle indices
	// side faces. easy to do since they're just sheer vertical square faces
	// here we do bottom pair and the vertex above the first of the pair, then second of the bottom pair and the top pair
	// remember: "above" means adding sectorPointCount
	for (int i = 0; i < sectorPointCount; i++)
	{
		triangleIndices.push_back(i); // bottom first
		triangleIndices.push_back((i + 1) % sectorPointCount); // bottom second
		triangleIndices.push_back(i + sectorPointCount); // top first

		triangleIndices.push_back((i + 1) % sectorPointCount); // bottom second
		triangleIndices.push_back(i + sectorPointCount); // top first
		triangleIndices.push_back(((i + 1) % sectorPointCount) + sectorPointCount); // top second

	}


	// bottom and top faces are.. not trivial. So I'm using this handy ear-clipping triangulation library.
	// This is to find constituent triangles of the 2d polygon formed by the sector points.

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

	// fill up the top face indices (same as bottom, but yknow, above).
	for (auto& index : triangulatedIndices)
	{
		triangleIndices.push_back(index + sectorPointCount);
	}
	

	// Create bounding box (not an accurate representation of sector trigs)
	DirectX::BoundingOrientedBox::CreateFromPoints(box, vertices.size(), &vertices.data()->position, (size_t)sizeof(VertexPosition));


}

// let's us input our SimpleMath Vec3's directly into the 2d earcut algo
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







/*
Notes for if I ever wanna comeback and revist the texture tiling problem:
We would need to use VertexPositionNormalTexture instead of VertexPosition.
We would also need independent vertices for each tri instead of shared ones (so texture uv's are independent).
Calculating the normal is pretty easy, just the normal of the tri (pointing outward).
Calculating the UVs is a lot harder. For each triangle, would need to project from 3d to 2d (in the plane of the normal of the tri). 
	Those 2d coords would be our uvs, *I think*.
IModelTriangles + IModelTexturedTriangles, with IRenderer3D having overloads for drawTriangle/drawTriangleCollection.
*/