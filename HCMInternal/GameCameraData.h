#pragma once
#include "pch.h"
struct GameCameraData
{
	SimpleMath::Vector3* position;
	SimpleMath::Vector3* velocity; 
	float* FOV;
	SimpleMath::Vector3* lookDirForward;
	SimpleMath::Vector3* lookDirUp;        
}; 