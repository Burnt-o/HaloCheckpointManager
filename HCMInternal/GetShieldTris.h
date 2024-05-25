#pragma once
#include "BumpControlDefs.h"
//#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING
//#define _SILENCE_CXX20_CISO646_REMOVED_WARNING
//#include "fcl\fcl.h"
//#include "fcl\geometry\collision_geometry.h"

#ifdef SHIELD_BUMPS

typedef std::array<SimpleMath::Vector3, 3> SMTriangle;
const std::array< SMTriangle, 24>& getShieldTris();

//fcl::CollisionObjectf* getShieldCollisionModel();


#endif