#pragma once
#include "pch.h"
#include <ranges>

typedef std::pair<SimpleMath::Vector3, SimpleMath::Vector3> Edge;

struct TriggerModelOld
{
	DirectX::BoundingOrientedBox box; // For frustum culling.
	std::array< Edge, 12> edges; // for drawing wireframe lines
	SimpleMath::Matrix transformation; // how to get from a unit cube @ origin to the correct position, rotation, and size
	std::string label;

	TriggerModelOld(std::string triggerName, SimpleMath::Vector3 position, SimpleMath::Vector3 extents, SimpleMath::Vector3 forward, SimpleMath::Vector3 up);
		
};