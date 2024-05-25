#pragma once
#include "BumpControlDefs.h"
#include "BumpSetup.h"
#include "EntityPhysicsSimulator.h"
#include "BumpPlayerStruct.h"






#ifdef SHIELD_BUMPS
#include "BumpShieldStruct.h"
BumpSetup cullISBGroupSetup(const Nook& nook, BumpSetup plainSetup, EntityPhysicsSimulator::AdvancePhysicsTicks* advancePhysics, BumpPlayerObject* playerObject, BumpShieldObject* shieldObject, Datum playerDatum);
#endif

BumpSetup cullISBSetup(const Nook& nook, BumpSetup plainSetup, EntityPhysicsSimulator::AdvancePhysicsTicks* advancePhysics, BumpPlayerObject* playerObject, Datum playerDatum);
