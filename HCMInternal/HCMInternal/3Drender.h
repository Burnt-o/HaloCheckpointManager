#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include "CameraData.h"
#include <vector>


extern void render3D();
std::vector<XMFLOAT4> GetNGonScreenPositions(std::vector<XMFLOAT3> vertices);
XMFLOAT4 WorldToScreen(const XMFLOAT3& worldPos);