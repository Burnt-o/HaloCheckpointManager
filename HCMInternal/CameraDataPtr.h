#pragma once
#include "pch.h"
struct CameraDataPtr
{
	SimpleMath::Vector3* position;
	SimpleMath::Vector3* velocity;
	float* FOV;
	SimpleMath::Vector3* lookDirForward;
	SimpleMath::Vector3* lookDirUp;
};