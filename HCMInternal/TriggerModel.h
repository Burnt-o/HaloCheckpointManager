#pragma once
#include "pch.h"


struct TriggerModel
{

	DirectX::BoundingOrientedBox box;
	std::array<SimpleMath::Vector3, 8> corners;
	TriggerModel(SimpleMath::Vector3 position, SimpleMath::Vector3 extents, SimpleMath::Vector3 forward, SimpleMath::Vector3 up)
		: box(position, extents, SimpleMath::Quaternion::LookRotation(forward, up))
	{
	box.GetCorners(corners.data());
	}
	TriggerModel() {}


	// so the renderer doesn't have to reallocate every time it renders a trigger
	std::array<SimpleMath::Vector3, 8> cornersScreenPosition;

	
	
};