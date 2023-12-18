#pragma once
#include "pch.h"
#include <d3d11.h>

// singleton 2d d3d11 rendering class
class Renderer2D
{
private:

	static inline Renderer2D* instance = nullptr;
	static inline bool init = false;
	static void inititialise(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
public:

	static void render(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, SimpleMath::Vector2 ss, ID3D11RenderTargetView* m_pMainRenderTargetView);

	static inline bool good = true;
};