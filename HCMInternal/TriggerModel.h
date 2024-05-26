#pragma once
#include "pch.h"
#include <ranges>


struct TriggerModel
{
	DirectX::BoundingOrientedBox box; // For frustum culling.
	std::array<SimpleMath::Vector3, 8> vertices; // Vertices (the real data for rendering).



	explicit TriggerModel(SimpleMath::Vector3 position, SimpleMath::Vector3 extents, SimpleMath::Vector3 forward, SimpleMath::Vector3 up)
		: box(position, extents, SimpleMath::Quaternion::LookRotation(forward, up))
	{

		box.GetCorners(vertices.data());
	}





	
};