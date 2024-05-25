#pragma once
#include "Datum.h"

// Debug shit
	//#define TEST_PHYSICS_SIMULATOR
	//#define TEST_BUMP_GENERATOR  
	//#define TEST_PERFORMANCE
	//#define TEST_DIRECTION_CULL
	#define TEST_TIME_REVERSAL
	#define CACHE_BSPS 



//#define CONSIDER_ANNIVERSARY_GEO




// Level control
#define WHICH_LEVEL 3
#define WHICH_TRICK 0


// primary control
//#define SETUP_BSP_INDEX 12
	#define SHIELD_BUMPS // test shield bumps (outside or inside)
	#define WHICH_SHIELD 1 // shield1 is closest to landing peli, shield2 furtherest
	#define START_SETUP_COUNT 0 // what setup to start at.
	#define ISB_STYLE // new code
	#define EXTRA_TICKS_AFTER_LAST_INPUT 20
	#define ENABLE_CULLING // recommended unless input space is very small
	#define SHIELD_TIME_REVERSAL
#define RANDOM_VA_OFFSET
// control vals for outside-shieldbumps

	#define SHIELD_BUMP_GO_LENGTH 100 // used to use 41
	#define MAX_JUMP_BEFORE_GO -10 // used to be -3


	// override values for SHIELD_BUMP_GO_LENGTH to be in this range
		//#define OVERRIDE_SHIELD_ON_GO_MIN 30
		//#define OVERRIDE_SHIELD_ON_GO_MAX 32
	// override values for MAX_JUMP_BEFORE_GO to be in this range
		//#define OVERRIDE_SHIELD_JUMP_GO_MIN 4
		//#define OVERRIDE_SHIELD_JUMP_GO_MAX 6

	
// control vals for inside-shieldbumps
	#ifdef ISB_STYLE
		#define ISB_INPUT_LENGTH 50
		//#define ISB_PRESETUPS 6
		//#define ISB_PRESETUP_COMBOS 5
		//#define ISB_PRESETUPS_INCLUDE_CROUCH
		#define ISB_IDLE_CROUCH
		//#define BEGIN_JUMP_ONLY
//#define OVERRIDE_SECOND_TICK 11
//#define SPECIAL_OVERRIDE
	#endif


// Culling control
#define CULL_FOR_VELOCITY_DIR



//0.5 / 0.01 for precise runs. 5 and 0.001 for normal

// input space control
constexpr float pixelAngle = 0.00104719761f; // at 2.7 sens
#define DEPTH_ANGLE_1 6.f // multiplies above number - the step-angle each test is conducted at
#define DEPTH_ANGLE_2 0.02f // if potential teleport (is in correct direction) is detected, use this finer step angle instead
#define TEST_EVERY_ANGLE_WHEN_DISTANCE_FRACTION_OVER 0.05f

	//#define ANGLE_DEPTH_1 1000000000 // how many angles to test per setup-nook combo
	//#define ANGLE_DEPTH_2 1000000 // broad phase test every nth angle of above value - if no *potential* teleport found, skip to next nth angle. Otherwise test values in between.
	//#define ANGLE_DEPTH_3 1000 


	#define ONLY_CONSIDER_ADDED_GOINPUTS // pressing a button is easier than releasing it. should just always leave this on.
#define INCLUDE_FORWARD
//#define INCLUDE_LEFT
#define INCLUDE_RIGHT
//#define INCLUDE_BACKWARD
#define INCLUDE_CROUCH
#define INCLUDE_JUMP

#define IGNORE_JITTER // if undefined, jitter during setup phase will cause test to fail
#define LOG_POTENTIAL_TELES



















constexpr int animWaitTime = 15; // how long a crouch <-> uncrouched transition takes. 

// a position that the player can consistently get to for reproducible teleports + a label name.
typedef std::pair<const char*, SimpleMath::Vector3> Nook;

#define advancePhys(x) advancePhysics(playerDatum, x); 
#define setInputs(x) playerObject->setInput(x);

#define MAX_SETUP_TICK 5





static inline std::vector<DirectX::BoundingBox> goodTeleportVolume = {}; // filled in BumpTestingToolImpl by CreateFromPoints(goodTeleportVolumeBounds)
static inline std::vector<DirectX::BoundingBox> goodTeleportVolumeExpanded = {}; // filled in BumpTestingToolImpl by doubling extents of above

// following stuff defined in .cpp, as it depends WHICH_LEVEL (and WHICH_SHIELD) we're doing the tests on
// the destination volume we want to get to! set in bumptestingtoolimpl constructor
extern std::vector<std::pair<SimpleMath::Vector3, SimpleMath::Vector3>> goodTeleportVolumeBounds;
extern  SimpleMath::Vector3 goodTeleportVolumeLeft; // a point along leftmost edge (I don't care about z since this is just used for 2d calculations)
extern  SimpleMath::Vector3 goodTeleportVolumeRight;
extern  std::vector<Nook> nooksToTest;
extern SimpleMath::Vector3 safeStandingSpot; // a spot that is guarenteed the chief can fully stand up.
#ifdef SHIELD_BUMPS
// We need to know the exact position of the shield to look it up in the object table.
// We need to know exact position & rotation to do outside-shield-bump hitbox culling.
extern  SimpleMath::Vector3 shieldPosition;
extern  SimpleMath::Vector3 shieldRotation;
#endif





