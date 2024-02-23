#pragma once
#include "pch.h"
#include <d3d11.h>
typedef eventpp::CallbackList<void(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, SimpleMath::Vector2 screenSize, ID3D11RenderTargetView* m_pMainRenderTargetView)> DirectXRenderEvent;