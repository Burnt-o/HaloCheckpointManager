#pragma once
#include "EnumClass.h"

enum class SoftCeilingType
{
	Acceleration,
	SoftKill,
	SlipSurface
};

enum class SoftCeilingIgnoresObjectType : uint16_t
{
	None = 0,
	Biped = 0x1 << 0,
	Vehicle = 0x1 << 1,
	Camera = 0x1 << 2,
	HugeVehicle = 0x1 << 3
};
enableEnumClassBitmask(SoftCeilingIgnoresObjectType); // Activate bitmask operators
using SoftCeilingObjectMask = bitmask<SoftCeilingIgnoresObjectType>;

static_assert(sizeof(SoftCeilingObjectMask) == 0x2);

struct SoftCeilingData
{
public:
	const SoftCeilingObjectMask softCeilingObjectMask;
public:
	const SoftCeilingType softCeilingType;
	const std::array<SimpleMath::Vector3, 3> vertices;

	bool appliesToBiped() const { return bitmaskContains(softCeilingObjectMask, SoftCeilingIgnoresObjectType::Biped) == false; }
	bool appliesToVehicle() const { return bitmaskContains(softCeilingObjectMask, SoftCeilingIgnoresObjectType::Vehicle) == false; }
	bool appliesToCamera() const { return bitmaskContains(softCeilingObjectMask, SoftCeilingIgnoresObjectType::Camera) == false; }
	bool appliesToHugeVehicle() const { return bitmaskContains(softCeilingObjectMask, SoftCeilingIgnoresObjectType::HugeVehicle) == false; }

	explicit SoftCeilingData(SoftCeilingObjectMask softCeilingObjectMask, SoftCeilingType softCeilingType, std::array<SimpleMath::Vector3, 3> vertices)
		: softCeilingObjectMask(softCeilingObjectMask), softCeilingType(softCeilingType), vertices(vertices) {}


	// mutable
	SimpleMath::Vector4 colorSolid { 1.f, 0.f, 1.f, 1.f };
	SimpleMath::Vector4 colorWireframe { 0.f, 1.f, 1.f, 1.f };

};