#pragma once
#include "pch.h"

// interface provides 3d rendering functions
class IRenderer3D {
private:
	virtual void updateCameraData(const SimpleMath::Vector2& screensize) = 0;
	friend class Render3DEventProvider;
public:
	virtual SimpleMath::Vector3 worldPointToScreenPosition(SimpleMath::Vector3 worldPointPosition) = 0;
	virtual float cameraDistanceToWorldPoint(SimpleMath::Vector3 worldPointPosition) = 0;
};

// impl in Renderer3DImpl.h