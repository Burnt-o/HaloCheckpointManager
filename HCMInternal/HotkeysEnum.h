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
hideHUDToggle,\
setPlayerHealth

// hotkeys that do not fire an event. Usually used for continous inputs like freecamera
#define NOEVENT_HOTKEYS \
cameraTranslateUpBinding,\
cameraTranslateDownBinding,\
cameraRollLeftBinding,\
cameraRollRightBinding,\
cameraFOVIncreaseBinding,\
cameraFOVDecreaseBinding

// both event and non-event hotkeys are rebindable
#define ALL_REBINDABLE_HOTKEYS ALL_EVENTONPRESS_HOTKEYS, NOEVENT_HOTKEYS


//enum class RebindableHotkeyEnum : int {
//	ALL_EVENTONPRESS_HOTKEYS
//};


enum class RebindableHotkeyEnum : int {
	ALL_REBINDABLE_HOTKEYS
};
