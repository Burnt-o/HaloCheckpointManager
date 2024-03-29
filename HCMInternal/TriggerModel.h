#pragma once
#include "pch.h"


struct TriggerModel
{

	DirectX::BoundingOrientedBox box;
	TriggerModel(SimpleMath::Vector3 position, SimpleMath::Vector3 extents, SimpleMath::Vector3 forward, SimpleMath::Vector3 up)
		: box(position, extents, SimpleMath::Quaternion::LookRotation(forward, up)) {}
	TriggerModel() {}
	
	
};