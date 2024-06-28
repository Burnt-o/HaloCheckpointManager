#pragma once
#include "EnumClass.h"

enum class SoftCeilingType
{
	Acceleration,
	SoftKill,
	SlipSurface
};

enum class SoftCeilingIgnoresObjectType
{
	Biped = 0,
	Vehicle = 1,
	Camera = 2,
	HugeVehicle = 3
};
enableEnumClassBitmask(SoftCeilingIgnoresObjectType); // Activate bitmask operators
using SoftCeilingObjectMask = bitmask<SoftCeilingIgnoresObjectType>;

struct SoftCeilingData
{
private:
	const SoftCeilingObjectMask softCeilingObjectMask;
public:
	const SoftCeilingType softCeilingType;
	const std::array<SimpleMath::Vector3, 3> vertices;

	bool appliesToBiped() const { return (SoftCeilingObjectMask(SoftCeilingIgnoresObjectType::Biped) & softCeilingObjectMask).operator bool() == false; }
	bool appliesToVehicle() const { return (SoftCeilingObjectMask(SoftCeilingIgnoresObjectType::Vehicle) & softCeilingObjectMask).operator bool() == false; }
	bool appliesToCamera() const { return (SoftCeilingObjectMask(SoftCeilingIgnoresObjectType::Camera) & softCeilingObjectMask).operator bool() == false; }
	bool appliesToHugeVehicle() const { return (SoftCeilingObjectMask(SoftCeilingIgnoresObjectType::HugeVehicle) & softCeilingObjectMask).operator bool() == false; }

	explicit SoftCeilingData(SoftCeilingObjectMask softCeilingObjectMask, SoftCeilingType softCeilingType, std::array<SimpleMath::Vector3, 3> vertices)
		: softCeilingObjectMask(softCeilingObjectMask), softCeilingType(softCeilingType), vertices(vertices) {}
};