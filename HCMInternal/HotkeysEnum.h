#pragma once
#include "pch.h"

// hotkeys that fire an event when going from unpressed to pressed
#define ALL_EVENTONPRESS_HOTKEYS 	\
toggleGUI,\
togglePause,\
advanceTicks,\
forceCheckpoint,\
forceRevert,\
forceDoubleRevert,\
forceCoreSave,\
forceCoreLoad,\
injectCheckpoint,\
dumpCheckpoint,\
injectCore,\
dumpCore,\
forceMissionRestart,\
speedhack,\
invuln,\
aiFreeze,\
medusa,\
forceTeleport,\
forceLaunch,\
naturalCheckpointDisable,\
infiniteAmmo,\
bottomlessClip,\
display2DInfo,\
freeCamera,\
freeCameraTeleportToCameraHotkey,\
freeCameraGameInputDisable,\
freeCameraCameraInputDisable,\
freeCameraUserInputCameraIncreaseTranslationSpeedHotkey,\
freeCameraUserInputCameraDecreaseTranslationSpeedHotkey,\
freeCameraUserInputCameraSetPosition,\
freeCameraUserInputCameraSetRotation,\
freeCameraUserInputCameraMaintainVelocity, \
freeCameraUserInputCameraSetVelocity,\
freeCameraAnchorPositionToObjectPosition,\
freeCameraAnchorPositionToObjectRotation,\
freeCameraAnchorRotationToObjectPosition,\
freeCameraAnchorRotationToObjectFacing,\
freeCameraAnchorFOVToObjectDistance,\
editPlayerViewAngleSet,\
editPlayerViewAngleAdjustHorizontal,\
editPlayerViewAngleAdjustVertical,\
switchBSP,\
switchBSPSet,\
hideHUDToggle,\
setPlayerHealth,\
toggleWaypoint3D,\
triggerOverlayToggleHotkey,\
commandConsoleHotkey,\
commandConsoleExecuteBuffer,\
disableBarriers,\
softCeilingOverlayToggleHotkey,\
abilityMeterToggleHotkey,\
sensResetCountHotkey,\
sensDriftOverlayToggleHotkey



// skull hotkeys. They don't behave any different to above, but BOOST_PP_TUPLE_SIZE can only handle up to 64 elements 
#define SKULL_HOTKEYS \
skullAngerHotkey, \
skullAssassinsHotkey, \
skullBlackEyeHotkey, \
skullBlindHotkey, \
skullCatchHotkey, \
skullEyePatchHotkey, \
skullFamineHotkey, \
skullFogHotkey, \
skullForeignHotkey, \
skullIronHotkey, \
skullJackedHotkey, \
skullMasterblasterHotkey, \
skullMythicHotkey, \
skullRecessionHotkey, \
skullSoAngryHotkey, \
skullStreakingHotkey, \
skullSwarmHotkey, \
skullThatsJustWrongHotkey, \
skullTheyComeBackHotkey, \
skullThunderstormHotkey, \
skullTiltHotkey, \
skullToughLuckHotkey, \
skullBandannaHotkey, \
skullBondedPairHotkey, \
skullBoomHotkey, \
skullCowbellHotkey, \
skullEnvyHotkey, \
skullFeatherHotkey, \
skullGhostHotkey, \
skullGruntBirthdayPartyHotkey, \
skullGruntFuneralHotkey, \
skullIWHBYDHotkey, \
skullMalfunctionHotkey, \
skullPinataHotkey, \
skullProphetBirthdayPartyHotkey, \
skullScarabHotkey, \
skullSputnikHotkey, \
skullAcrophobiaHotkey


// hotkeys that do not fire an event. Usually used for continous inputs like freecamera
#define NOEVENT_HOTKEYS \
cameraTranslateUpBinding,\
cameraTranslateDownBinding,\
cameraRollLeftBinding,\
cameraRollRightBinding,\
cameraFOVIncreaseBinding,\
cameraFOVDecreaseBinding

// both event and non-event hotkeys are rebindable
#define ALL_REBINDABLE_HOTKEYS ALL_EVENTONPRESS_HOTKEYS, NOEVENT_HOTKEYS, SKULL_HOTKEYS


//enum class RebindableHotkeyEnum : int {
//	ALL_EVENTONPRESS_HOTKEYS
//};


enum class RebindableHotkeyEnum : int {
	ALL_REBINDABLE_HOTKEYS
};
