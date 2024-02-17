#pragma once
#include "pch.h"

// interface provides 3d rendering functions
class IRenderer3D {
private:
	virtual void updateCameraData(const SimpleMath::Vector2& screensize) = 0;
	friend class Render3DEventProvider;
public:
	virtual void draw3DText(const std::string& text, const SimpleMath::Vector3& targetPosition, const SimpleMath::Vector4& textColor) = 0;
};

// impl in Renderer3DImpl.h