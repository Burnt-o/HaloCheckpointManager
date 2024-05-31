#include "pch.h"
#include "TriggerModel.h"

TriggerModel::TriggerModel(std::string triggerName, SimpleMath::Vector3 position, SimpleMath::Vector3 extents, SimpleMath::Vector3 forward, SimpleMath::Vector3 up)
	: label(triggerName)
{
	// Note: position is actually the location of the mininum x/y/z, not the center.
	auto center = position + (extents / 2);


	auto triggerRotation = SimpleMath::Quaternion::LookRotation(forward, up);
	triggerRotation.Normalize();
	box = DirectX::BoundingOrientedBox(center, extents, triggerRotation);

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

	transformation = SimpleMath::Matrix::CreateWorld(SimpleMath::Vector3::Zero, SimpleMath::Vector3::UnitX, SimpleMath::Vector3::UnitZ) // world at origin
		* SimpleMath::Matrix::CreateScale(extents)// resize

		* SimpleMath::Matrix::CreateTranslation(extents / 2) // offset to pivot point (corner)
		* SimpleMath::Matrix::CreateFromQuaternion(SimpleMath::Quaternion::FromToRotation(SimpleMath::Vector3::UnitX, forward)) // rotate forward
		* SimpleMath::Matrix::CreateFromQuaternion(SimpleMath::Quaternion::FromToRotation(SimpleMath::Vector3::UnitZ, up)) // rotate up
		* SimpleMath::Matrix::CreateTranslation(-1.f * (extents / 2)) // unoffset to pivot point

		* SimpleMath::Matrix::CreateTranslation(center); // translate


}