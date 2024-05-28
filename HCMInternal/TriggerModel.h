#pragma once
#include "pch.h"
#include <ranges>

typedef std::pair<SimpleMath::Vector3, SimpleMath::Vector3> Edge;

struct TriggerModel
{
	DirectX::BoundingOrientedBox box; // For frustum culling.
	std::array< Edge, 12> edges; // for drawing wireframe lines
	SimpleMath::Matrix transformation; // how to get from a unit cube @ origin to the correct position, rotation, and size
	std::string label;


	TriggerModel(std::string triggerName, SimpleMath::Vector3 position, SimpleMath::Vector3 extents, SimpleMath::Vector3 forward, SimpleMath::Vector3 up)
		: box(position, extents, SimpleMath::Quaternion::LookRotation(forward, up)),
		label(triggerName)
	{
		std::array<SimpleMath::Vector3, 8> vertices;
		box.GetCorners(vertices.data());

		edges = {
			// Near face.
			Edge{vertices[0], vertices[1]},
			Edge{vertices[1], vertices[2]},
			Edge{vertices[2], vertices[3]},
			Edge{vertices[3], vertices[0]},

			// Far face.
			Edge{vertices[4], vertices[5]},
			Edge{vertices[5], vertices[6]},
			Edge{vertices[6], vertices[7]},
			Edge{vertices[7], vertices[4]},

			// Connect 'em together.
			Edge{vertices[0], vertices[4]},
			Edge{vertices[1], vertices[5]},
			Edge{vertices[2], vertices[6]},
			Edge{vertices[3], vertices[7]},
		};

		transformation = SimpleMath::Matrix::CreateWorld(SimpleMath::Vector3::Zero, SimpleMath::Vector3::UnitX, SimpleMath::Vector3::UnitZ); // world at origin
		transformation = transformation * SimpleMath::Matrix::CreateTranslation(position); // translate
		transformation = transformation * SimpleMath::Matrix::CreateFromQuaternion(SimpleMath::Quaternion::LookRotation(forward, up)); // rotate
		transformation = transformation * SimpleMath::Matrix::CreateScale(extents); // resize

	}





	
};