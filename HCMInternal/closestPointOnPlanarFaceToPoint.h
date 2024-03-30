#pragma once
#include "pch.h"



SimpleMath::Vector3 closestPointOnPlanarFaceToPoint(const std::array<SimpleMath::Vector3, 4>& face, const SimpleMath::Plane& plane, const SimpleMath::Vector3& worldPoint);