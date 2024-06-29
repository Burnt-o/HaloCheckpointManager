#include "pch.h"
#include "TriggerModel.h"

TriggerModel::TriggerModel(std::string triggerName, SimpleMath::Vector3 position, SimpleMath::Vector3 extents, SimpleMath::Vector3 forward, SimpleMath::Vector3 up)
	: label(triggerName)
{
	// Note: position is actually the location of the mininum x/y/z, not the center.
	auto center = position + (extents / 2);



	auto resizeTransform = SimpleMath::Matrix::CreateScale(extents);

	auto rotateTransform = SimpleMath::Matrix::CreateTranslation(extents / 2) // offset to pivot point (corner)
		* SimpleMath::Matrix::CreateFromQuaternion(SimpleMath::Quaternion::FromToRotation(SimpleMath::Vector3::UnitX, forward)) // rotate forward
		* SimpleMath::Matrix::CreateFromQuaternion(SimpleMath::Quaternion::FromToRotation(SimpleMath::Vector3::UnitZ, up)) // rotate up
		* SimpleMath::Matrix::CreateTranslation(-1.f * (extents / 2)); // unoffset to pivot point

	auto translateTransform = SimpleMath::Matrix::CreateTranslation(center);


	transformation = SimpleMath::Matrix::CreateWorld(SimpleMath::Vector3::Zero, SimpleMath::Vector3::UnitX, SimpleMath::Vector3::UnitZ) // world at origin
		* resizeTransform // resize
		* rotateTransform // rotate
		* translateTransform; // translate



	// begin with unit cube vertices
	std::array<SimpleMath::Vector3, 8> vertices =
	{
		// from DirectX::BoundingOrientedBox constructor
		/*
			{ { { -1.0f, -1.0f,  1.0f, 0.0f } } },
			{ { {  1.0f, -1.0f,  1.0f, 0.0f } } },
			{ { {  1.0f,  1.0f,  1.0f, 0.0f } } },
			{ { { -1.0f,  1.0f,  1.0f, 0.0f } } },
			{ { { -1.0f, -1.0f, -1.0f, 0.0f } } },
			{ { {  1.0f, -1.0f, -1.0f, 0.0f } } },
			{ { {  1.0f,  1.0f, -1.0f, 0.0f } } },
			{ { { -1.0f,  1.0f, -1.0f, 0.0f } } },

		*/
		// above is span -1 .. 1, but unit cube is actually -0.5f to 0.5f

		SimpleMath::Vector3{ -0.5f, -0.5f,  0.5f },
		SimpleMath::Vector3{  0.5f, -0.5f,  0.5f },
		SimpleMath::Vector3{  0.5f,  0.5f,  0.5f },
		SimpleMath::Vector3{ -0.5f,  0.5f,  0.5f },
		SimpleMath::Vector3{ -0.5f, -0.5f, -0.5f },
		SimpleMath::Vector3{  0.5f, -0.5f, -0.5f },
		SimpleMath::Vector3{  0.5f,  0.5f, -0.5f },
		SimpleMath::Vector3{ -0.5f,  0.5f, -0.5f },
	};

	// transform unit cube vertices by transformation
	std::ranges::transform(vertices, vertices.begin(), [this](const auto& v) { return SimpleMath::Vector3::Transform(v, transformation); });

	// create bounding box
	box = DirectX::BoundingOrientedBox(SimpleMath::Vector3::Zero, extents / 2, SimpleMath::Quaternion::Identity);
	box.Transform(box, rotateTransform);
	box.Transform(box, translateTransform);


	//DirectX::BoundingOrientedBox::CreateFromPoints(box, vertices.size(), vertices.data(), sizeof(SimpleMath::Vector3)); // overzealous bounding box! bad

#ifdef HCM_DEBUG
	PLOG_DEBUG << "box.Center" << (SimpleMath::Vector3)box.Center;
#endif

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


}