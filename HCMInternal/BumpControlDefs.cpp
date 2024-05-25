#include "pch.h"
#include "BumpControlDefs.h"

#define rad(x) DirectX::XMConvertToRadians(x)
typedef std::pair<SimpleMath::Vector3, SimpleMath::Vector3> VV;

// SILENT CARTOGRAPHER. From beach to fling-door
#if WHICH_LEVEL == 3

std::vector<std::pair<SimpleMath::Vector3, SimpleMath::Vector3>> goodTeleportVolumeBounds ={
	VV{{-0.24, -11.35, 2.06 }, {16.01, -28.35, 22.37}},
	VV{{-1.5, -10.05, 6.84 }, {18.46, -29.75, 22.37}},
	VV{{-2.38, -9.07, 12.68 }, {20, -31.70, 22.37}},
};

SimpleMath::Vector3 goodTeleportVolumeLeft = SimpleMath::Vector3{ 5.0733, -9.07, 4.8 };
SimpleMath::Vector3 goodTeleportVolumeRight = SimpleMath::Vector3{ 5.0733, -31.70, 4.8 };
SimpleMath::Vector3 safeStandingSpot = { -47.134167f, 2.2180924f, 3.2526879f };

	#ifdef SHIELD_BUMPS
		#if WHICH_SHIELD == 1
		SimpleMath::Vector3 shieldPosition = { -53.63070679f, 8.817664146f, 4.121509552f };
		SimpleMath::Vector3 shieldRotation = { rad(173.993f), rad(-1.95244f), rad(7.54072f) };
		#elif WHICH_SHIELD == 2
		SimpleMath::Vector3 shieldPosition = { -57.53929138f, 6.269376755f, 3.784127235f };
		SimpleMath::Vector3 shieldRotation = { rad(-132.757f), rad(5.45174f), rad(5.01805f) };
		#endif
	#endif


		std::vector<Nook> nooksToTest = {

	#if WHICH_SHIELD == 1

			// the cool ones
			//Nook("Shield1aab", { -53.35607529f, 8.59325600f, 4.22289753f }), // stand in shield1a, wait for shield to come back on, then full crouch, then destroy it. GOOD 23.78 units remaining (-7,9)
			//Nook("Shield1aba", { -53.51924133f, 8.11715889f, 4.16646099f }), // crouch in shield1a, wait for shield to come back on, then full crouch jump then destroy it. GREAT 19.09 units remaining (45, 32). AWESOME 9 units remain. ah but angle a bit much to the right
			//Nook("Shield1abb", { -53.51942444f, 8.12011623f, 4.16685915f }), // crouch in shield1a, wait for shield to come back on, then 1t crouch jump then destroy it. cool 21.40 units remaining at good angle.


			Nook("Shield1a", { -53.50214385986f, 8.69638347626f, 4.18378925323f }), // the og where I found the cliff tele
			//Nook("Shield1b", { -53.81348038f, 8.59503937f, 4.17600346f }), // shield up crouch left
			//Nook("Shield1ba", {-53.73331833f, 8.25471783f, 4.19185162f }), // crouch jump from 1b with shield up
			//Nook("Shield1bb", { -53.81638336f, 8.62065411f, 4.16750574f }), // stand jump from 1b with shield down
			//Nook("Shield1bc", { -53.73331833f, 8.25471783f, 4.19185162f }), // 1 tick crouch jump from 1b with shield up
			//Nook("Shield1bd", { -53.81872177f, 8.64131451f, 4.16065216f }), // crouch jump from 1b with shield down. 31.93%



			////// crouch in right shield up. jittery nook but stable position from jump.
			//Nook("Shield1ca", { -53.68497086f, 8.12451839f, 4.17313433f }), // full crouch jump
			//Nook("Shield1cb", { -53.68406296f, 8.12887573f, 4.17367983f }), // 1t crouch jump


			//Nook("Shield1aa", { -53.53908920f, 8.65226269f, 4.18880272f }), // stand in shield1a, wait for shield to come back on, then destroy it
			//Nook("Shield1aaa", { -53.66115570f, 8.07860470f, 4.16623783f }), // stand in shield1a, wait for shield to come back on, then full jump, then destroy it.

			//Nook("Shield1aaba", { -53.58399963f, 8.29454231f, 4.19216919f }), // stand in shield1a, wait for shield to come back on, then crouch, then full crouch jump, then destroy it.
			//Nook("Shield1aaba", { -53.63362503f, 8.21169567f, 4.18290854f }), // stand in shield1a, wait for shield to come back on, then crouch, then 1t crouch jump, then destroy it.
			//Nook("Shield1ab", { -53.51926422f, 8.59090233f, 4.20438528f }), // crouch in shield1a, wait for shield to come back on, then destroy it

	#else

			Nook("Shield2a", { -57.5039978f, 5.988418102f, 3.765403986f }), // shield up standing, behind on the left
			Nook("Shield2d", { -57.28651810f, 6.26859570f, 3.80894923f }), // shield up crouch, behind on the right. super precise VA (~2.19213) where it's no-jitter
			Nook("Shield2c", { -57.58158493f, 5.934363365f, 3.754845381f }), // shield up crouch, behind on the left. difficult but consistent to setup
			Nook("Shield2e", { -57.57717514038f, 5.97876930237f, 3.75838088989f }), // shield down crouch, behind on the left. there's a small range of va where jitter avoidable.


			Nook("Shield2db", { -57.17661667f, 6.04207277f, 3.79958868f }), // 2d + full crouch jump. maybe got something? 31.77157021 at -8,14 
			Nook("Shield2da", { -57.17192078f, 6.04637384f, 3.80041575f }), // 2d + 1 tick crouch jump.
			Nook("Shield2db", { -57.17661667f, 6.04207277f, 3.79958868f }), // 2d + full crouch jump


			Nook("Shield2aa", { -57.24642181f, 5.81833982f, 3.77564812f }), // shield up standing, behind on the left. then stand jump
			Nook("Shield2ab", { -57.32780457f, 5.86990643f, 3.77225757f }), // shield up standing, behind on the left. then crouch jump. no good setups?
			Nook("Shield2ac", { -57.32574463f, 5.87159491f, 3.77255583f }), // shield up standing, behind on the left. then 1-t crouch jump



	#endif

};


#endif

// ASSAULT ON THE CONTROL ROOM - from peli start to outside-hallway
#if WHICH_LEVEL == 4
#if WHICH_TRICK == 0
		std::vector<std::pair<SimpleMath::Vector3, SimpleMath::Vector3>>goodTeleportVolumeBounds = {
			VV{{45.75, -24.22, 46.83 }, {43.83, -20.13, 70}},
			VV{{45.75, -21.82, 46.83 }, {40.49, -20.13, 70}},
			VV{{50.75, -25.82, 50 }, {35, -14.00, 70}},
		};

		//SimpleMath::Vector3 goodTeleportVolumeLeft = { 45.75, -20.13, 47.83 };
		//SimpleMath::Vector3 goodTeleportVolumeRight = { 40.49, -20.13, 47.83 };
		SimpleMath::Vector3 goodTeleportVolumeLeft = { 50.75, -20.13, 47.83 };
		SimpleMath::Vector3 goodTeleportVolumeRight = { 35.49, -20.13, 47.83 };
		SimpleMath::Vector3 safeStandingSpot = { 41.46952438354f, -15.34479618073f, 47.99993896484f }; // nice side effect of forcing the correct bsp



#if WHICH_SHIELD == 1
		// the closest one that you can SEE when getting out the peli. easy to create a setup for, but angle prob hard to find
		SimpleMath::Vector3 shieldPosition = { 40.7871f, 30.3646f, 47.951f };
		SimpleMath::Vector3 shieldRotation = { rad(150.f), 0.f, 0.f };

#elif WHICH_SHIELD == 2
		// the one furtherest right when you get out the peli. harder to create setup for, but angle prob easy to find
		SimpleMath::Vector3 shieldPosition = { 36.3029f, 30.4302f, 47.951f };
		SimpleMath::Vector3 shieldRotation = { rad(-150.f), 0.f, 0.f };

#elif WHICH_SHIELD == 3
		SimpleMath::Vector3 shieldPosition = { 48.0704f, 30.2939f, 47.951f };
		SimpleMath::Vector3 shieldRotation = { rad(-150.f), 0.f, 0.f };
#elif WHICH_SHIELD == 4
		SimpleMath::Vector3 shieldPosition = { 52.8322f, 30.2613f, 47.951f };
		SimpleMath::Vector3 shieldRotation = { rad(150.f), 0.f, 0.f };
#endif


	std::vector<Nook> nooksToTest = {

#if WHICH_SHIELD == 1
		Nook("Shield1a", { 40.88221359f, 30.10756111f, 47.94997787f }), // shield down crouch right
		Nook("Shield1b", { 40.5124588f, 30.32104301f, 47.94997787f }), // shield down crouch left
		Nook("Shield1c", { 40.79184341f, 30.66563988f, 47.94997787f }), // shield down, front, crouch right
		Nook("Shield1d", { 41.04094696f, 30.52181816f, 47.94997787f }), // shield down, front, crouch left
#elif WHICH_SHIELD == 2
		Nook("Shield2a", { 36.57307815552f, 30.38402748108f, 47.94997787476f }), // shield down crouch right
		Nook("Shield2b", { 36.20331573486f, 30.17054176331f, 47.94997787476f }), // shield down crouch left
#elif WHICH_SHIELD == 3
		Nook("Shield3a", { 47.97083282471f, 30.03430747986f, 47.94997787476f }), // shield down crouch left
#elif WHICH_SHIELD == 4
		Nook("Shield4a", { 52.55760574341f, 30.21769142151f, 47.94997787476f }), // shield down crouch left. small angle range
		Nook("Shield4b", { 52.92737197876f, 30.00420761108f, 47.94997787476f }), // shield down crouch right. small angle range
#endif

	};




#elif WHICH_TRICK == 1
std::vector<std::pair<SimpleMath::Vector3, SimpleMath::Vector3>>goodTeleportVolumeBounds = {
	VV{{44.086082 + 5, -24.911348 + 5, 37.869949 + 5}, {44.086082 - 5, -24.911348 - 5, 37.869949 - 5}},
	VV{{42, -23, 18 }, {100, -35, 25}},
	VV{{42, -15, -1 }, {200, -100, 25}},
};


SimpleMath::Vector3 goodTeleportVolumeLeft = { 50.75, -20.13, 47.83 };
SimpleMath::Vector3 goodTeleportVolumeRight = { 35.49, -20.13, 47.83 };
SimpleMath::Vector3 safeStandingSpot = { 44.626842, -20.322628, 48.168781 }; // nice side effect of forcing the correct bsp

std::vector<Nook> nooksToTest = {
	Nook("WindowLeft1a", {42.57135772705f, -18.81858634949f, 49.48493194580f}), // left. just crouch, align parallel and walk forward
	Nook("WindowLeft2a", {45.84356689453f, -21.05826377869f, 49.48493576050f}), // on left side. jump twice to ensure you get to same spot
	Nook("WindowLeft3a", {45.84356689453f, -23.30828094482f, 49.48493576050f}), // on left side. small range of angles at (2.39) where crouch-w gives consistent
	Nook("WindowLeft3b", {45.84356689453f, -23.10828018188f, 49.48493576050f}), // on right side. crouch-w turning left til shaking stops. jump once.
	Nook("WindowRight1a", {40.48418807983f, -18.61857986450f, 49.48493194580f}), // left. just crouch, align parallel and walk forward
	Nook("WindowRight2a", {40.48418807983f, -21.05952453613f, 49.48493576050f}), // right. aim (0.76) crouch - w
	Nook("WindowRight3a", {43.75639724731f, -23.30827713013f, 49.48493576050f}), // right. just crouch, align parallel and walk forward
	Nook("WindowTopRight1", {49.94176864624f, -18.71858215332f, 51.06097412109f}), 
	Nook("WindowTopLeft3", {49.38597488403f, -23.20828247070f, 51.06096267700f}),

};
#endif
#endif


// 343 GUULTY SPART
#if WHICH_LEVEL == 5
// reveal skip replacement
#if WHICH_TRICK == 0
std::vector<std::pair<SimpleMath::Vector3, SimpleMath::Vector3>>goodTeleportVolumeBounds = {
	VV{{-21.37672997, 5, 1.700999308}, {-12.5599041, 23.44089127, 20}}
};

SimpleMath::Vector3 goodTeleportVolumeLeft = { -21.37672997, 23.44089127,2 };
SimpleMath::Vector3 goodTeleportVolumeRight = { -12.5599041, 5, 2 };
SimpleMath::Vector3 safeStandingSpot = { -59.22397614f, -7.499589443f, 1.899986267f };



#if WHICH_SHIELD == 1
// left shield
SimpleMath::Vector3 shieldPosition = { -56.4106f, -2.74725f, 1.90099f };
SimpleMath::Vector3 shieldRotation = { rad(15.4048f), 0.f, 0.f };

#elif WHICH_SHIELD == 2
// right shield
SimpleMath::Vector3 shieldPosition = { -56.0796f, -9.80218f, 1.90099f };
SimpleMath::Vector3 shieldRotation = { rad(169.297f), 0.f, 0.f };

#endif


std::vector<Nook> nooksToTest = {

#if WHICH_SHIELD == 1 
		Nook("Shield1a", { -56.66043854f, -2.634532213f, 1.899986267f }), // shield down crouch right
		Nook("Shield1b", { -56.24880981f, -2.521116495f, 1.899986267f }), // shield down crouch left
		Nook("Shield1c", { -56.19958496f, -2.962007284f, 1.899986267f }), // shield down, front, crouch right. narrow range of angles
		Nook("Shield1d", { -56.47689438f, -3.038416862f, 1.899986267f }), // shield down, front, crouch left

		Nook("Shield1e", { -56.65443802f, -2.622735023f, 1.899986267f }), // shield up crouch right
		Nook("Shield1ea", { -56.63525772f, -2.365744114f, 1.899986267f }), // ^ full crouch jump
		Nook("Shield1f", { -56.3483696f, -2.496065617f, 1.899986267f }), // shield up stand left. narrow range of angles
		Nook("Shield1fa", { -56.47221375f, -2.187439203f, 1.899986267f }), // ^ stand jump
		Nook("Shield1fa", { -56.47813416f, -2.286340714f, 1.899986267f }), // ^ full crouch jump

#elif WHICH_SHIELD == 2
	Nook("Shield2a", { -55.90481567f, -10.01335144f, 1.899986267f }), // shield down crouch right. narrow range of angles
	Nook("Shield2b", { -56.32435226f, -9.934053421f, 1.899986267f }), // shield down crouch left
	Nook("Shield2c", { -55.89189148f, -9.569907188f, 1.899986267f }), // shield down, front, crouch left

	Nook("Shield2d", { -56.00287628f, -10.04650307f, 1.899986267f }), // shield up stand right.
	Nook("Shield2da", { -56.09814835f, -10.3706646f, 1.899986267f }), // ^ stand jump
	Nook("Shield2db", { -56.1158905f, -10.27173424f, 1.899986267f }), // ^ full crouch jump
#endif

};




#elif WHICH_TRICK == 1
// teleport from camo jumo room to next bsp load
#elif WHICH_TRICK == 2
// teleport from shield in valley room to final lift


std::vector<std::pair<SimpleMath::Vector3, SimpleMath::Vector3>>goodTeleportVolumeBounds = {
	VV{{-29.19, 77.10, -38.20}, {-44.27, 91.98, -30.71}}
};


SimpleMath::Vector3 goodTeleportVolumeLeft = { -44.27, 77.10, -38.20 };
SimpleMath::Vector3 goodTeleportVolumeRight = { -29.19, 91.98, -38.20 };
SimpleMath::Vector3 safeStandingSpot = { -0.5735104084f, 69.15385437f, -38.09896851f };




SimpleMath::Vector3 shieldPosition = { 0.397154f, 68.9684f, -38.098f };
SimpleMath::Vector3 shieldRotation = { rad(49.8228f), 0.f, 0.f };




std::vector<Nook> nooksToTest = {

		Nook("Shield1a", { 0.4918161035f, 69.20636749f, -38.09896851f }), // shield up stand right. narrow range of angles
		Nook("Shield1ab", { 0.7702553272f, 69.39775085f, -38.09896851f }), // ^ full jump from above
		Nook("Shield1ac", { 0.7215973139f, 69.30980682f, -38.09896851f }), // ^ crouch jump from above


#endif
#endif