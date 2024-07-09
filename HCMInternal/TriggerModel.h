#pragma once
#include "IModel.h"

// an upgrade to trigger model that can handle both bounding box types and sector types.
// intended to be drawn with vertex/indice lists (primitiveBatch)

// not finished yet

class TriggerModel : public IModelTriangles, public IModelEdges
{
private:
	DirectX::BoundingOrientedBox box;


	VertexCollection vertices;
	IndexCollection triangleIndices;
	IndexCollection edgeIndices;

	std::string label;

public:
	TriggerModel(std::string triggerName, SimpleMath::Vector3 position, SimpleMath::Vector3 extents, SimpleMath::Vector3 forward, SimpleMath::Vector3 up); // bounding box constructor
	TriggerModel(std::string triggerName, std::vector<SimpleMath::Vector3> sectorPoints, float height); // sector constructor

	virtual const VertexCollection& getTriangleVertices() const override { return vertices; }
	virtual const IndexCollection& getTriangleIndices() const override { return triangleIndices; }

	virtual const VertexCollection& getEdgeVertices() const override { return vertices; }
	virtual const IndexCollection& getEdgeIndices() const override { return edgeIndices; }

	const bool m_isSectorType;

	const std::string& getLabel() const { return label; }

	// Only accurate for box type triggers
	const DirectX::BoundingOrientedBox& getBoundingBox() const { return box; }
};