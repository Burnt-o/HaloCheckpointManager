#pragma once
#include "pch.h"
#include <boost\preprocessor.hpp>

//#define ALL_SUPPORTED_GAMES (Halo1)(Halo2)(Halo3)(Halo3ODST)(HaloReach)(Halo4)
//
//#define ALLGUIELEMENTS 	\
//((controlHeadingGUI)ALL_SUPPORTED_GAMES)\
//((saveManagementHeadingGUI)ALL_SUPPORTED_GAMES)\
//	((forceCheckpointGUI)ALL_SUPPORTED_GAMES)\
//	((forceRevertGUI)ALL_SUPPORTED_GAMES)\
//	((forceDoubleRevertGUI)(Halo2)(Halo3)(Halo3ODST)(HaloReach)(Halo4))\
//	((forceCoreSaveGUI)(Halo1))\
//	((forceCoreLoadGUI)(Halo1))\
//	((injectCheckpointGUI)ALL_SUPPORTED_GAMES)\
//	((dumpCheckpointGUI)ALL_SUPPORTED_GAMES)\
//	((injectCoreGUI)(Halo1))\
//	((dumpCoreGUI)(Halo1))\
//((cheatsHeadingGUI)ALL_SUPPORTED_GAMES)\
//	((speedhackGUI)ALL_SUPPORTED_GAMES)\
//	((invulnGUI)ALL_SUPPORTED_GAMES)\
//	((invulnNPCGUI)ALL_SUPPORTED_GAMES)\
//	((aiFreezeGUI)ALL_SUPPORTED_GAMES) \
//	((consoleCommandGUI)(Halo1)) \
//((overlaysHeadingGUI)ALL_SUPPORTED_GAMES)\
//((cameraHeadingGUI)ALL_SUPPORTED_GAMES)\
//((theaterHeadingGUI)(Halo3)(Halo3ODST)(HaloReach)(Halo4))\
//

// game sets
#define ALL_SUPPORTED_GAMES Halo1, Halo2, Halo3, Halo3ODST, HaloReach, Halo4
#define ALL_GAMES_AND_MAINMENU Halo1, Halo2, Halo3, Halo3ODST, HaloReach, Halo4, NoGame


// interpolator macro for freecamera
#define defFreeCameraInterpolator(name)\
((freeCamera##name##Interpolator, (ALL_SUPPORTED_GAMES)))\
((freeCamera##name##InterpolatorLinearFactor, (ALL_SUPPORTED_GAMES)))


// A sequence of pairs, where the first element of a pair is the GUIElementEnum name, and the second element is a tuple of supported games for that guielement
#define RELEASEGUIELEMENTS_ANDSUPPORTEDGAMES \
((controlHeadingGUI, (ALL_GAMES_AND_MAINMENU)))\
	((toggleGUIHotkeyGUI, (ALL_GAMES_AND_MAINMENU)))\
	((GUISettingsSubheading, (ALL_GAMES_AND_MAINMENU)))\
		((GUIShowingFreesCursor, (ALL_GAMES_AND_MAINMENU)))\
		((GUIShowingBlocksInput, (ALL_GAMES_AND_MAINMENU)))\
		((GUIShowingPausesGame, (ALL_GAMES_AND_MAINMENU)))\
	((togglePauseGUI, (ALL_GAMES_AND_MAINMENU)))\
	((togglePauseSettingsSubheading, (ALL_GAMES_AND_MAINMENU)))\
		((advanceTicksGUI, (ALL_SUPPORTED_GAMES)))\
		((pauseAlsoFreesCursorGUI, (ALL_GAMES_AND_MAINMENU)))\
		((pauseAlsoBlocksInputGUI, (ALL_GAMES_AND_MAINMENU)))\
	((showGUIFailuresGUI, (ALL_GAMES_AND_MAINMENU)))\
((saveManagementHeadingGUI, (ALL_SUPPORTED_GAMES)))\
	((forceCheckpointGUI, (ALL_SUPPORTED_GAMES)))\
	((forceRevertGUI, (ALL_SUPPORTED_GAMES)))\
	((forceDoubleRevertGUI, (Halo2, Halo3, Halo3ODST, HaloReach, Halo4)))\
	((forceCoreSaveGUI, (Halo1)))\
	((forceCoreLoadGUI, (Halo1)))\
	((injectCheckpointGUI, (ALL_SUPPORTED_GAMES)))\
	((injectCheckpointSettingsSubheading, (ALL_SUPPORTED_GAMES)))\
		((injectCheckpointForcesRevert, (ALL_SUPPORTED_GAMES)))\
		((injectCheckpointLevelCheck, (ALL_SUPPORTED_GAMES)))\
		((injectCheckpointVersionCheck, (ALL_SUPPORTED_GAMES)))\
		((injectCheckpointDifficultyCheck, (ALL_SUPPORTED_GAMES)))\
		((injectCheckpointIgnoresChecksum, (Halo3, Halo3ODST, HaloReach, Halo4)))\
	((dumpCheckpointGUI, (ALL_SUPPORTED_GAMES)))\
	((dumpCheckpointSettingsSubheading, (ALL_SUPPORTED_GAMES)))\
		((dumpCheckpointAutonameGUI, (ALL_SUPPORTED_GAMES)))\
		((dumpCheckpointForcesSave, (ALL_SUPPORTED_GAMES)))\
	((injectCoreGUI, (Halo1)))\
	((injectCoreSettingsSubheading, (Halo1)))\
		((injectCoreForcesRevert, (Halo1)))\
		((injectCoreLevelCheck, (Halo1)))\
		((injectCoreVersionCheck, (Halo1)))\
		((injectCoreDifficultyCheck, (Halo1)))\
	((dumpCoreSettingsSubheading, (Halo1)))\
		((dumpCoreAutonameGUI, (Halo1)))\
		((dumpCoreForcesSave, (Halo1)))\
	((dumpCoreGUI, (Halo1)))\
	((naturalCheckpointDisableGUI, (ALL_SUPPORTED_GAMES)))\
((cheatsHeadingGUI, (ALL_SUPPORTED_GAMES)))\
	((speedhackGUI, (ALL_SUPPORTED_GAMES)))\
	((invulnGUI, (ALL_SUPPORTED_GAMES)))\
	((invulnerabilitySettingsSubheading, (ALL_SUPPORTED_GAMES)))\
	((invulnNPCGUI, (ALL_SUPPORTED_GAMES)))\
	((infiniteAmmoGUI, (Halo1, Halo2)))\
	((bottomlessClipGUI, (Halo1, Halo2)))\
	((aiFreezeGUI, (ALL_SUPPORTED_GAMES)))\
	((medusaGUI, (Halo1, Halo2)))\
	((forceTeleportGUI, (ALL_SUPPORTED_GAMES)))\
	((forceTeleportSettingsSubheading, (ALL_SUPPORTED_GAMES)))\
		((forceTeleportApplyToPlayer, (ALL_SUPPORTED_GAMES)))\
			((forceTeleportCustomObject, (ALL_SUPPORTED_GAMES)))\
		((forceTeleportSettingsRadioGroup, (ALL_SUPPORTED_GAMES)))\
		((forceTeleportForward, (ALL_SUPPORTED_GAMES)))\
			((forceTeleportRelativeVec3, (ALL_SUPPORTED_GAMES)))\
			((forceTeleportForwardIgnoreZ, (ALL_SUPPORTED_GAMES)))\
		((forceTeleportManual, (ALL_SUPPORTED_GAMES)))\
			((forceTeleportAbsoluteVec3, (ALL_SUPPORTED_GAMES)))\
			((forceTeleportAbsoluteFillCurrent, (ALL_SUPPORTED_GAMES)))\
			((forceTeleportAbsoluteCopy, (ALL_SUPPORTED_GAMES)))\
			((forceTeleportAbsolutePaste, (ALL_SUPPORTED_GAMES)))\
	((forceLaunchGUI, (ALL_SUPPORTED_GAMES)))\
	((forceLaunchSettingsSubheading, (ALL_SUPPORTED_GAMES)))\
		((forceLaunchApplyToPlayer, (ALL_SUPPORTED_GAMES)))\
			((forceLaunchCustomObject, (ALL_SUPPORTED_GAMES)))\
		((forceLaunchSettingsRadioGroup, (ALL_SUPPORTED_GAMES)))\
		((forceLaunchForward, (ALL_SUPPORTED_GAMES)))\
			((forceLaunchRelativeVec3, (ALL_SUPPORTED_GAMES)))\
			((forceLaunchForwardIgnoreZ, (ALL_SUPPORTED_GAMES)))\
		((forceLaunchManual, (ALL_SUPPORTED_GAMES)))\
			((forceLaunchAbsoluteVec3, (ALL_SUPPORTED_GAMES)))\
	((switchBSPGUI, (ALL_SUPPORTED_GAMES)))\
((overlaysHeadingGUI, (ALL_SUPPORTED_GAMES)))\
	((display2DInfoToggleGUI, (ALL_SUPPORTED_GAMES)))\
	((display2DInfoSettingsInfoSubheading, (ALL_SUPPORTED_GAMES)))\
			((display2DInfoShowGameTick, (ALL_SUPPORTED_GAMES)))\
			((display2DInfoShowAggro, (Halo1)))\
			((display2DInfoShowRNG, (Halo1)))\
			((display2DInfoShowBSP, (ALL_SUPPORTED_GAMES)))\
			((display2DInfoShowNextObjectDatum, (Halo2)))\
			((display2DInfoTrackPlayer, (ALL_SUPPORTED_GAMES)))\
				((display2DInfoShowPlayerViewAngle, (ALL_SUPPORTED_GAMES)))\
				((display2DInfoShowPlayerPosition, (ALL_SUPPORTED_GAMES)))\
				((display2DInfoShowPlayerVelocity, (ALL_SUPPORTED_GAMES)))\
					((display2DInfoShowPlayerVelocityAbs, (ALL_SUPPORTED_GAMES)))\
					((display2DInfoShowPlayerVelocityXY, (ALL_SUPPORTED_GAMES)))\
					((display2DInfoShowPlayerVelocityXYZ, (ALL_SUPPORTED_GAMES)))\
				((display2DInfoShowPlayerHealth, (ALL_SUPPORTED_GAMES)))\
					((display2DInfoShowPlayerRechargeCooldown, (ALL_SUPPORTED_GAMES)))\
					((display2DInfoShowPlayerVehicleHealth, (ALL_SUPPORTED_GAMES)))\
			((display2DInfoTrackCustomObject, (ALL_SUPPORTED_GAMES)))\
				((display2DInfoCustomObjectDatum, (ALL_SUPPORTED_GAMES)))\
				((display2DInfoShowEntityObjectType, (ALL_SUPPORTED_GAMES)))\
				((display2DInfoShowEntityTagName, (ALL_SUPPORTED_GAMES)))\
				((display2DInfoShowEntityPosition, (ALL_SUPPORTED_GAMES)))\
				((display2DInfoShowEntityVelocity, (ALL_SUPPORTED_GAMES)))\
					((display2DInfoShowEntityVelocityAbs, (ALL_SUPPORTED_GAMES)))\
					((display2DInfoShowEntityVelocityXY, (ALL_SUPPORTED_GAMES)))\
					((display2DInfoShowEntityVelocityXYZ, (ALL_SUPPORTED_GAMES)))\
				((display2DInfoShowEntityHealth, (ALL_SUPPORTED_GAMES)))\
					((display2DInfoShowEntityRechargeCooldown, (ALL_SUPPORTED_GAMES)))\
					((display2DInfoShowEntityVehicleHealth, (ALL_SUPPORTED_GAMES)))\
	((display2DInfoSettingsVisualSubheading, (ALL_SUPPORTED_GAMES)))\
			((display2DInfoAnchorCorner, (ALL_SUPPORTED_GAMES)))\
			((display2DInfoScreenOffset, (ALL_SUPPORTED_GAMES)))\
			((display2DInfoFontSize, (ALL_SUPPORTED_GAMES)))\
			((display2DInfoFontColour, (ALL_SUPPORTED_GAMES)))\
			((display2DInfoFloatPrecision, (ALL_SUPPORTED_GAMES)))\
			((display2DInfoOutline, (ALL_SUPPORTED_GAMES)))\
((cameraHeadingGUI, (ALL_SUPPORTED_GAMES)))\
	((editPlayerViewAngleSubheading, (ALL_SUPPORTED_GAMES)))\
		((editPlayerViewAngleSet, (ALL_SUPPORTED_GAMES)))\
		((editPlayerViewAngleVec2, (ALL_SUPPORTED_GAMES)))\
		((editPlayerViewAngleFillCurrent, (ALL_SUPPORTED_GAMES)))\
		((editPlayerViewAngleCopy, (ALL_SUPPORTED_GAMES)))\
		((editPlayerViewAnglePaste, (ALL_SUPPORTED_GAMES)))\
		((editPlayerViewAngleAdjustHorizontal, (ALL_SUPPORTED_GAMES)))\
		((editPlayerViewAngleAdjustVertical, (ALL_SUPPORTED_GAMES)))\
		((editPlayerViewAngleAdjustFactor, (ALL_SUPPORTED_GAMES)))\
	((freeCameraToggleGUI, (ALL_SUPPORTED_GAMES)))\
	((freeCameraSettingsSimpleSubheading, (ALL_SUPPORTED_GAMES)))\
	((freeCameraSettingsAdvancedSubheading, (ALL_SUPPORTED_GAMES)))\
		((freeCameraHideWatermark, (ALL_SUPPORTED_GAMES)))\
		((freeCameraHideMessages, (ALL_SUPPORTED_GAMES)))\
		((freeCameraThirdPersonRendering, (ALL_SUPPORTED_GAMES)))\
		((freeCameraDisableScreenEffects, (ALL_SUPPORTED_GAMES)))\
		((freeCameraGameInputDisable, (ALL_SUPPORTED_GAMES)))\
		((freeCameraCameraInputDisable, (ALL_SUPPORTED_GAMES)))\
		((freeCameraUserInputCameraSettings, (ALL_SUPPORTED_GAMES)))\
			((freeCameraUserInputCameraBindingsPopup, (ALL_SUPPORTED_GAMES)))\
			((freeCameraUserInputCameraBaseFOV, (ALL_SUPPORTED_GAMES)))\
			((freeCameraUserInputCameraTranslationSpeed, (ALL_SUPPORTED_GAMES)))\
			((freeCameraUserInputCameraIncreaseTranslationSpeedHotkey, (ALL_SUPPORTED_GAMES)))\
			((freeCameraUserInputCameraDecreaseTranslationSpeedHotkey, (ALL_SUPPORTED_GAMES)))\
			((freeCameraUserInputCameraTranslationSpeedChangeFactor, (ALL_SUPPORTED_GAMES)))\
			defFreeCameraInterpolator(UserInputCameraTranslation)\
			((freeCameraUserInputCameraRotationSpeed, (ALL_SUPPORTED_GAMES)))\
			defFreeCameraInterpolator(UserInputCameraRotation)\
			((freeCameraUserInputCameraFOVSpeed, (ALL_SUPPORTED_GAMES)))\
			defFreeCameraInterpolator(UserInputCameraFOV)\
			((freeCameraUserInputCameraSetPosition, (ALL_SUPPORTED_GAMES)))\
			((freeCameraUserInputCameraSetPositionChildren, (ALL_SUPPORTED_GAMES)))\
				((freeCameraUserInputCameraSetPositionVec3, (ALL_SUPPORTED_GAMES)))\
				((freeCameraUserInputCameraSetPositionFillCurrent, (ALL_SUPPORTED_GAMES)))\
				((freeCameraUserInputCameraSetPositionCopy, (ALL_SUPPORTED_GAMES)))\
				((freeCameraUserInputCameraSetPositionPaste, (ALL_SUPPORTED_GAMES)))\
			((freeCameraUserInputCameraSetRotation, (ALL_SUPPORTED_GAMES)))\
			((freeCameraUserInputCameraSetRotationChildren, (ALL_SUPPORTED_GAMES)))\
				((freeCameraUserInputCameraSetRotationVec3, (ALL_SUPPORTED_GAMES)))\
				((freeCameraUserInputCameraSetRotationFillCurrent, (ALL_SUPPORTED_GAMES)))\
				((freeCameraUserInputCameraSetRotationCopy, (ALL_SUPPORTED_GAMES)))\
				((freeCameraUserInputCameraSetRotationPaste, (ALL_SUPPORTED_GAMES)))\
			((freeCameraUserInputCameraMaintainVelocity, (ALL_SUPPORTED_GAMES)))\
				((freeCameraUserInputCameraSetVelocity, (ALL_SUPPORTED_GAMES)))\
				((freeCameraUserInputCameraSetVelocityChildren, (ALL_SUPPORTED_GAMES)))\
					((freeCameraUserInputCameraSetVelocityVec3, (ALL_SUPPORTED_GAMES)))\
					((freeCameraUserInputCameraSetVelocityFillCurrent, (ALL_SUPPORTED_GAMES)))\
					((freeCameraUserInputCameraSetVelocityCopy, (ALL_SUPPORTED_GAMES)))\
					((freeCameraUserInputCameraSetVelocityPaste, (ALL_SUPPORTED_GAMES)))\
		((freeCameraAnchorPositionToObjectPosition, (ALL_SUPPORTED_GAMES)))\
			((freeCameraAnchorPositionToObjectPositionObjectToTrackComboGroup, (ALL_SUPPORTED_GAMES)))\
					((freeCameraAnchorPositionToObjectPositionObjectToTrackCustomObjectDatum, (ALL_SUPPORTED_GAMES)))\
			defFreeCameraInterpolator(AnchorPositionToObjectPositionTranslation)\
			((freeCameraAnchorPositionToObjectRotation, (ALL_SUPPORTED_GAMES)))\
		((freeCameraAnchorRotationToObjectPosition, (ALL_SUPPORTED_GAMES)))\
			((freeCameraAnchorRotationToObjectPositionObjectToTrackComboGroup, (ALL_SUPPORTED_GAMES)))\
					((freeCameraAnchorRotationToObjectPositionObjectToTrackCustomObjectDatum, (ALL_SUPPORTED_GAMES)))\
					((freeCameraAnchorRotationToObjectPositionObjectToTrackManualPositionVec3, (ALL_SUPPORTED_GAMES)))\
			defFreeCameraInterpolator(AnchorRotationToObjectPositionRotation)\
		((freeCameraAnchorRotationToObjectFacing, (ALL_SUPPORTED_GAMES)))\
			((freeCameraAnchorRotationToObjectFacingObjectToTrackComboGroup, (ALL_SUPPORTED_GAMES)))\
					((freeCameraAnchorRotationToObjectFacingObjectToTrackCustomObjectDatum, (ALL_SUPPORTED_GAMES)))\
			defFreeCameraInterpolator(AnchorRotationToObjectFacingRotation)\
		((freeCameraAnchorFOVToObjectDistance, (ALL_SUPPORTED_GAMES)))\
			((freeCameraAnchorFOVToObjectDistanceObjectToTrackComboGroup, (ALL_SUPPORTED_GAMES)))\
					((freeCameraAnchorFOVToObjectDistanceObjectToTrackCustomObjectDatum, (ALL_SUPPORTED_GAMES)))\
					((freeCameraAnchorFOVToObjectDistanceObjectToTrackManualPositionVec3, (ALL_SUPPORTED_GAMES)))\
			defFreeCameraInterpolator(AnchorFOVToObjectDistanceFOV)\
((theaterHeadingGUI, (Halo3,Halo3ODST,HaloReach,Halo4)))


#define DEBUGGUIELEMENTS_ANDSUPPORTEDGAMES \
((debugHeadingGUI, (ALL_SUPPORTED_GAMES)))\
	((consoleCommandGUI, (Halo1)))\
	((getObjectAddressGUI, (ALL_SUPPORTED_GAMES)))


#ifdef HCM_DEBUG

#define ALLGUIELEMENTS_ANDSUPPORTEDGAMES BOOST_PP_CAT(RELEASEGUIELEMENTS_ANDSUPPORTEDGAMES, DEBUGGUIELEMENTS_ANDSUPPORTEDGAMES)

#else 

#define ALLGUIELEMENTS_ANDSUPPORTEDGAMES RELEASEGUIELEMENTS_ANDSUPPORTEDGAMES

#endif




#define MAKE_FIRSTOFPAIR_SET(r, d, seq) BOOST_PP_TUPLE_ELEM(0, seq),
#define MAKE_ALL_FIRSTOFPAIR(seq) BOOST_PP_SEQ_FOR_EACH(MAKE_FIRSTOFPAIR_SET, _, seq)
#define ALLGUIELEMENTS MAKE_ALL_FIRSTOFPAIR(ALLGUIELEMENTS_ANDSUPPORTEDGAMES)


enum class GUIElementEnum {
	ALLGUIELEMENTS
};
