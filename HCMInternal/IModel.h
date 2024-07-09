#pragma once
#include "pch.h"
#include "directxtk\VertexTypes.h"

using IndexCollection = std::vector<uint16_t>;
using VertexCollection = std::vector<DirectX::VertexPosition>;

class IModelTriangles
{
public:
	virtual const VertexCollection& getTriangleVertices() const = 0;
	virtual const IndexCollection& getTriangleIndices() const = 0;
	virtual ~IModelTriangles() = default;
};

class IModelEdges
{
public:
	virtual const VertexCollection& getEdgeVertices() const = 0;
	virtual const IndexCollection& getEdgeIndices() const = 0;
	virtual ~IModelEdges() = default;
};

