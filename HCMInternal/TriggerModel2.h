#pragma once
#include "IModel.h"

// an upgrade to trigger model that can handle both bounding box types and sector types.
// intended to be drawn with vertex/indice lists (primitiveBatch)

// not finished yet

class TriggerModel2 : IModel
{
private:
	// only applicable to BoundingBox types, used for pointInside.
	std::optional< DirectX::BoundingOrientedBox> boundingBox;

	// only applicable to Sector types, used for pointInside.
	std::optional<float> sectorHeight;
	std::optional<std::vector<SimpleMath::Vector2>> sectorPoints2D;

	VertexCollection vertices;
	IndexCollection triangleIndices;
	IndexCollection edgeIndices;

public:
	TriggerModel2(SimpleMath::Vector3 position, SimpleMath::Vector3 extents, SimpleMath::Vector3 forward, SimpleMath::Vector3 up); // bounding box constructor
	TriggerModel2(std::vector<SimpleMath::Vector3> sectorPoints, float height); // sector constructor

	virtual const VertexCollection& getTriangleVertices() override { return vertices; }
	virtual const IndexCollection& getTriangleIndices() override { return triangleIndices; }

	virtual const VertexCollection& getEdgeVertices() override { return vertices; }
	virtual const IndexCollection& getEdgeIndices() override { return edgeIndices; }

	virtual bool pointInside(const SimpleMath::Vector3& worldPoint) override;
};