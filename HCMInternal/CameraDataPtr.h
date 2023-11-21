#pragma once
#include "pch.h"
struct CameraDataPtr
{
	SimpleMath::Vector3* position = nullptr;
	SimpleMath::Vector3* velocity = nullptr;
	SimpleMath::Vector3* forwardLookDir = nullptr;
	SimpleMath::Vector3* upLookDir = nullptr;
	float* FOV = nullptr;
};