#pragma once
#include "EnumClass.h"
#include "IModel.h"
enum class SoftCeilingType
{
	Acceleration,
	SoftKill,
	SlipSurface
};

enum class SoftCeilingIgnoresObjectType : uint16_t
{
	None = 0,
	Biped = 0x1 << 0,
	Vehicle = 0x1 << 1,
	Camera = 0x1 << 2,
	HugeVehicle = 0x1 << 3
};
enableEnumClassBitmask(SoftCeilingIgnoresObjectType); // Activate bitmask operators
using SoftCeilingObjectMask = bitmask<SoftCeilingIgnoresObjectType>;

static_assert(sizeof(SoftCeilingObjectMask) == 0x2);

class SoftCeilingData : public IModelTriangles, public IModelEdges
{
private:
	const SoftCeilingObjectMask softCeilingObjectMask;
	VertexCollection vertices = {};
	IndexCollection triangleIndices;
	IndexCollection edgeIndices;

public:
	const SoftCeilingType softCeilingType;



	virtual const VertexCollection& getTriangleVertices() const override { return vertices; }
	virtual const IndexCollection& getTriangleIndices() const override { return triangleIndices; }

	virtual const VertexCollection& getEdgeVertices() const override { return vertices; }
	virtual const IndexCollection& getEdgeIndices() const override { return edgeIndices; }


	bool appliesToBiped() const { return bitmaskContains(softCeilingObjectMask, SoftCeilingIgnoresObjectType::Biped) == false; }
	bool appliesToVehicle() const { return bitmaskContains(softCeilingObjectMask, SoftCeilingIgnoresObjectType::Vehicle) == false; }
	bool appliesToCamera() const { return bitmaskContains(softCeilingObjectMask, SoftCeilingIgnoresObjectType::Camera) == false; }
	bool appliesToHugeVehicle() const { return bitmaskContains(softCeilingObjectMask, SoftCeilingIgnoresObjectType::HugeVehicle) == false; }

	explicit SoftCeilingData(SoftCeilingObjectMask softCeilingObjectMask, SoftCeilingType softCeilingType, std::array<SimpleMath::Vector3, 3> verts)
		: softCeilingObjectMask(softCeilingObjectMask), softCeilingType(softCeilingType)
	
	{
		vertices.push_back(verts.at(0));
		vertices.push_back(verts.at(1));
		vertices.push_back(verts.at(2));

		triangleIndices = { 0, 1, 2 };
		edgeIndices = {
			0, 1,
			1, 2,
			2, 0
		};
	}


	// mutable
	SimpleMath::Vector4 colorSolid { 1.f, 0.f, 1.f, 1.f };
	SimpleMath::Vector4 colorWireframe { 0.f, 1.f, 1.f, 1.f };

};