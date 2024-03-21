#pragma once
#include "pch.h"
#include "SpriteResource.h"

// interface provides 3d rendering functions
class IRenderer3D {
private:
	virtual bool updateCameraData(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, SimpleMath::Vector2 screenSize, ID3D11RenderTargetView* pMainRenderTargetView) = 0;
	friend class Render3DEventProvider;
public:
	// returns where on the screen a world point should appear
	virtual SimpleMath::Vector3 worldPointToScreenPosition(SimpleMath::Vector3 worldPointPosition) = 0;

	// returns cameras distance to a world point
	virtual float cameraDistanceToWorldPoint(SimpleMath::Vector3 worldPointPosition) = 0;


	// RECTF return is the screen position min and max of the drawn item.
	virtual RECTF drawSprite(int spriteResourceID, SimpleMath::Vector2 screenPosition, float spriteScale = 1.f, SimpleMath::Vector4 spriteColor = {1.f, 0.5f, 0.f, 1.f}) = 0;
	virtual RECTF drawCenteredSprite(int spriteResourceID, SimpleMath::Vector2 screenPosition, float spriteScale = 1.f, SimpleMath::Vector4 spriteColor = { 1.f, 0.5f, 0.f, 1.f }) = 0;

	enum class AppliedClamp
	{
		None,
		Left,
		Right,
		Top,
		Bottom,
	};

	virtual AppliedClamp clampScreenPositionToEdge(SimpleMath::Vector3& screenPositionOut, SimpleMath::Vector3& worldPointPosition, float clampBorderRatio = 0.f) = 0;
};

// impl in Renderer3DImpl.h