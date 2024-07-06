#pragma once
#include "pch.h"
#include "directxtk\VertexTypes.h"

using IndexCollection = std::vector<uint16_t>;
using VertexCollection = std::vector<DirectX::VertexPosition>;

class IModelTriangles
{
public:
	virtual const VertexCollection& getTriangleVertices() = 0;
	virtual const IndexCollection& getTriangleIndices() = 0;
	virtual ~IModelTriangles() = default;
};

class IModelEdges
{
public:
	virtual const VertexCollection& getEdgeVertices() = 0;
	virtual const IndexCollection& getEdgeIndices() = 0;
	virtual ~IModelEdges() = default;
};

class IModelPointInside
{
public:
	virtual bool pointInside(const SimpleMath::Vector3& worldPoint) = 0;
	virtual ~IModelPointInside() = default;
};

class IModel : public IModelTriangles, public IModelEdges, public IModelPointInside
{
public:
	virtual ~IModel() = default;
};