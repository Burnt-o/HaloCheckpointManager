#pragma once
#include "pch.h"
#include <boost\preprocessor.hpp>



// game set tuples
#define ALL_SUPPORTED_GAMES Halo1, Halo2, Halo3, Halo3ODST, HaloReach, Halo4
#define ALL_GAMES_AND_MAINMENU Halo1, Halo2, Halo3, Halo3ODST, HaloReach, Halo4, NoGame
#define FREE_CAMERA_SUPPORT Halo1, Halo2, Halo3, Halo3ODST, HaloReach, Halo4
#define THIRD_GEN Halo3, Halo3ODST, HaloReach, Halo4
#define ABILITY_GAMES HaloReach, Halo4

// interpolator macro for freecamera
#define defFreeCameraInterpolator(name)\
((freeCamera##name##Interpolator, (FREE_CAMERA_SUPPORT)))\
((freeCamera##name##InterpolatorLinearFactor, (FREE_CAMERA_SUPPORT)))


// A sequence of pairs, where the first element of a pair is the GUIElementEnum name, and the second element is a tuple of supported games for that guielement
// Indentation is cosmetic but indicates hiearchy of elements
#define RELEASEGUIELEMENTS_ANDSUPPORTEDGAMES1 \
((controlHeadingGUI, (ALL_GAMES_AND_MAINMENU)))\
	((toggleGUIHotkeyGUI, (ALL_GAMES_AND_MAINMENU)))\
	((messagesFontSize, (ALL_GAMES_AND_MAINMENU)))\
	((messagesFontColor, (ALL_GAMES_AND_MAINMENU)))\
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
	((OBSBypassToggleGUI, (ALL_GAMES_AND_MAINMENU)))\
	((HideWatermarkGUI, (ALL_GAMES_AND_MAINMENU)))\
	((HideWatermarkIncludeMessagesGUI, (ALL_GAMES_AND_MAINMENU)))\
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
		((injectCheckpointIgnoresChecksum, (Halo1, Halo2, Halo3, Halo3ODST, HaloReach, Halo4)))\
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
	((forceFutureCheckpointGUI, (ALL_SUPPORTED_GAMES)))\
		((forceFutureCheckpointToggle, (ALL_SUPPORTED_GAMES)))\
		((forceFutureCheckpointTick, (ALL_SUPPORTED_GAMES)))\
		((forceFutureCheckpointFill, (ALL_SUPPORTED_GAMES)))\
	((forceMissionRestartGUI, (ALL_SUPPORTED_GAMES)))\
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
	((switchBSPGUI, (Halo1, Halo2)))\
	((switchBSPSetGUI, (THIRD_GEN)))\
		((switchBSPSetLoadSet, (THIRD_GEN)))\
		((switchBSPSetFillCurrent, (THIRD_GEN)))\
		((switchBSPSetLoadIndex, (THIRD_GEN)))\
		((switchBSPSetUnloadIndex, (THIRD_GEN)))\
	((setPlayerHealthSubheadingGUI, (ALL_SUPPORTED_GAMES)))\
		((setPlayerHealthGUI, (ALL_SUPPORTED_GAMES)))\
		((setPlayerHealthValueGUI, (ALL_SUPPORTED_GAMES)))\
	((skullToggleGUI, (ALL_SUPPORTED_GAMES)))\
	((playerPositionToClipboardGUI, (ALL_SUPPORTED_GAMES)))\
	((consoleCommandGUI, (ALL_SUPPORTED_GAMES)))\
	((consoleCommandSettings, (ALL_SUPPORTED_GAMES)))\
		((consoleCommandOutputEvent, (ALL_SUPPORTED_GAMES)))\
		((consoleCommandPauseGame, (ALL_SUPPORTED_GAMES)))\
		((consoleCommandBlockInput, (ALL_SUPPORTED_GAMES)))\
		((consoleCommandFreeCursor, (ALL_SUPPORTED_GAMES)))\
		((consoleCommandFontSize, (ALL_SUPPORTED_GAMES)))\
		((consoleCommandExecuteBuffer, (ALL_SUPPORTED_GAMES)))\
	((disableBarriersToggle, (THIRD_GEN)))\
	((soundClassGainAdjusterToggle, (Halo1)))\
	((soundClassGainAdjusterSettings, (Halo1)))\
		((soundClassGainDialog, (Halo1)))\
		((soundClassGainAmbience, (Halo1)))\
		((soundClassGainWeapons, (Halo1)))\
		((soundClassGainVehicles, (Halo1)))\
		((soundClassGainMusic, (Halo1)))\
		((soundClassGainOther, (Halo1)))




#define RELEASEGUIELEMENTS_ANDSUPPORTEDGAMES2 \
((overlaysHeadingGUI, (ALL_SUPPORTED_GAMES)))\
	((renderDistance3DGUI, (ALL_SUPPORTED_GAMES)))\
	((display2DInfoToggleGUI, (ALL_SUPPORTED_GAMES)))\
	((display2DInfoSettingsInfoSubheading, (ALL_SUPPORTED_GAMES)))\
			((display2DInfoShowGameTick, (ALL_SUPPORTED_GAMES)))\
			((display2DInfoShowAggro, (Halo1)))\
			((display2DInfoShowRNG, (Halo1)))\
			((display2DInfoShowBSP, (Halo1, Halo2)))\
			((display2DInfoShowBSPSet, (THIRD_GEN)))\
			((display2DInfoShowNextObjectDatum, (Halo2)))\
			((display2DInfoTrackPlayer, (ALL_SUPPORTED_GAMES)))\
				((display2DInfoShowPlayerViewAngle, (ALL_SUPPORTED_GAMES)))\
				((display2DInfoShowPlayerViewAngleID, (ALL_SUPPORTED_GAMES)))\
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
	((waypoint3DGUIToggle, (ALL_SUPPORTED_GAMES)))\
	((waypoint3DGUIList, (ALL_SUPPORTED_GAMES)))\
	((waypoint3DGUISettings, (ALL_SUPPORTED_GAMES)))\
		((waypoint3DGUIClampToggle, (ALL_SUPPORTED_GAMES)))\
		((waypoint3DGUIGlobalSpriteColor, (ALL_SUPPORTED_GAMES)))\
		((waypoint3DGUIGlobalSpriteScale, (ALL_SUPPORTED_GAMES)))\
		((waypoint3DGUIGlobalLabelColor, (ALL_SUPPORTED_GAMES)))\
		((waypoint3DGUIGlobalLabelScale, (ALL_SUPPORTED_GAMES)))\
		((waypoint3DGUIGlobalDistanceColor, (ALL_SUPPORTED_GAMES)))\
		((waypoint3DGUIGlobalDistanceScale, (ALL_SUPPORTED_GAMES)))\
		((waypoint3DGUIGlobalDistancePrecision, (ALL_SUPPORTED_GAMES)))\
	((viewAngleLine3DGUIToggle, (ALL_SUPPORTED_GAMES)))\
	((viewAngleLine3DGUIList, (ALL_SUPPORTED_GAMES)))\
	((viewAngleLine3DGUISettings, (ALL_SUPPORTED_GAMES)))\
		((viewAngleLine3DGUIGlobalSpriteColor, (ALL_SUPPORTED_GAMES)))\
		((viewAngleLine3DGUIGlobalLabelColor, (ALL_SUPPORTED_GAMES)))\
		((viewAngleLine3DGUIGlobalLabelScale, (ALL_SUPPORTED_GAMES)))\
		((viewAngleLine3DGUIGlobalDistanceColor, (ALL_SUPPORTED_GAMES)))\
		((viewAngleLine3DGUIGlobalDistanceScale, (ALL_SUPPORTED_GAMES)))\
		((viewAngleLine3DGUIGlobalDistancePrecision, (ALL_SUPPORTED_GAMES)))\
	((triggerOverlayToggle, (ALL_SUPPORTED_GAMES)))\
	((triggerOverlaySettings, (ALL_SUPPORTED_GAMES)))\
		((triggerOverlayFilterToggle, (ALL_SUPPORTED_GAMES)))\
			((triggerOverlayFilterExactMatch, (ALL_SUPPORTED_GAMES)))\
			((triggerOverlayFilterStringDialog, (ALL_SUPPORTED_GAMES)))\
			((triggerOverlayFilterCountLabel, (ALL_SUPPORTED_GAMES)))\
			((triggerOverlayFilterStringCopy, (ALL_SUPPORTED_GAMES)))\
			((triggerOverlayFilterStringPaste, (ALL_SUPPORTED_GAMES)))\
		((triggerOverlayRenderStyle, (ALL_SUPPORTED_GAMES)))\
		((triggerOverlayInteriorStyle, (ALL_SUPPORTED_GAMES)))\
		((triggerOverlayLabelStyle, (ALL_SUPPORTED_GAMES)))\
		((triggerOverlayLabelScale, (ALL_SUPPORTED_GAMES)))\
		((triggerOverlayNormalColor, (ALL_SUPPORTED_GAMES)))\
		((triggerOverlayBSPColor, (ALL_SUPPORTED_GAMES)))\
		((triggerOverlaySectorColor, (Halo3ODST, HaloReach, Halo4)))\
		((triggerOverlayAlpha, (ALL_SUPPORTED_GAMES)))\
		((triggerOverlayWireframeAlpha, (ALL_SUPPORTED_GAMES)))\
		((triggerOverlayCheckHitToggle, (ALL_SUPPORTED_GAMES)))\
			((triggerOverlayCheckHitFalloff, (ALL_SUPPORTED_GAMES)))\
			((triggerOverlayCheckHitColor, (ALL_SUPPORTED_GAMES)))\
		((triggerOverlayCheckMissToggle, (ALL_SUPPORTED_GAMES)))\
			((triggerOverlayCheckMissFalloff, (ALL_SUPPORTED_GAMES)))\
			((triggerOverlayCheckMissColor, (ALL_SUPPORTED_GAMES)))\
		((triggerOverlayMessageOnEnter, (ALL_SUPPORTED_GAMES)))\
		((triggerOverlayMessageOnExit, (ALL_SUPPORTED_GAMES)))\
		((triggerOverlayMessageOnCheckHit, (ALL_SUPPORTED_GAMES)))\
		((triggerOverlayMessageOnCheckMiss, (ALL_SUPPORTED_GAMES)))\
		((triggerOverlayPositionToggle, (ALL_SUPPORTED_GAMES)))\
			((triggerOverlayPositionColor, (ALL_SUPPORTED_GAMES)))\
			((triggerOverlayPositionScale, (ALL_SUPPORTED_GAMES)))\
			((triggerOverlayPositionWireframe, (ALL_SUPPORTED_GAMES)))\
	((shieldInputPrinterToggle, (Halo1)))\
	((sensDriftOverlayToggle, (Halo1)))\
	((sensDriftOverlaySettings, (Halo1)))\
		((sensOverDotFrameToggle, (Halo1)))\
			((sensMessageOnOverDotToggle, (Halo1)))\
			((sensSoundOnOverDotToggle, (Halo1)))\
		((sensSubpixelDriftToggle, (Halo1)))\
			((sensMessageOnSubpixelDriftToggle, (Halo1)))\
			((sensSoundOnSubpixelDriftToggle, (Halo1)))\
			((sensSubpixelDriftSciNotationToggle, (Halo1)))\
		((sensCountTurnsToggle, (Halo1)))\
		((sensResetCountAction, (Halo1)))\
		((sensResetCountOnRevertToggle, (Halo1)))\
		((sensAnchorCorner, (Halo1)))\
		((sensScreenOffset, (Halo1)))\
		((sensFontSize, (Halo1)))\
		((sensFontColour, (Halo1)))\
	((softCeilingOverlayToggle, (THIRD_GEN)))\
	((softCeilingOverlaySettings, (THIRD_GEN)))\
		((softCeilingOverlayRenderTypes, (THIRD_GEN)))\
		((softCeilingOverlayRenderDirection, (THIRD_GEN)))\
		((softCeilingOverlayColorAccel, (THIRD_GEN)))\
		((softCeilingOverlayColorSlippy, (THIRD_GEN)))\
		((softCeilingOverlayColorKill, (THIRD_GEN)))\
		((softCeilingOverlaySolidTransparency, (THIRD_GEN)))\
		((softCeilingOverlayWireframeTransparency, (THIRD_GEN)))\
	((abilityMeterOverlayToggle, (ABILITY_GAMES)))\
	((abilityMeterOverlaySettings, (ABILITY_GAMES)))\
			((abilityMeterHeroAssistToggle, (Halo4)))\
			((abilityMeterAbilityAnchorCorner, (ABILITY_GAMES)))\
			((abilityMeterAbilityScreenOffset, (ABILITY_GAMES)))\
			((abilityMeterAbilitySize, (ABILITY_GAMES)))\
			((abilityMeterAbilityBackgroundColor, (ABILITY_GAMES)))\
			((abilityMeterAbilityForegroundColor, (ABILITY_GAMES)))\
			((abilityMeterAbilityHighlightColor, (ABILITY_GAMES)))\
			((abilityMeterCooldownToggle, (ABILITY_GAMES)))\
				((abilityMeterCooldownAnchorCorner, (ABILITY_GAMES)))\
				((abilityMeterCooldownScreenOffset, (ABILITY_GAMES)))\
				((abilityMeterCooldownSize, (ABILITY_GAMES)))\
				((abilityMeterCooldownBackgroundColor, (ABILITY_GAMES)))\
				((abilityMeterCooldownForegroundColor, (ABILITY_GAMES)))\
				((abilityMeterCooldownHighlightColor, (ABILITY_GAMES)))\
((cameraHeadingGUI, (ALL_SUPPORTED_GAMES)))\
	((hideHUDToggle, (FREE_CAMERA_SUPPORT)))\
	((editPlayerViewAngleSubheading, (ALL_SUPPORTED_GAMES)))\
		((editPlayerViewAngleSet, (ALL_SUPPORTED_GAMES)))\
		((editPlayerViewAngleVec2, (ALL_SUPPORTED_GAMES)))\
		((editPlayerViewAngleFillCurrent, (ALL_SUPPORTED_GAMES)))\
		((editPlayerViewAngleCopy, (ALL_SUPPORTED_GAMES)))\
		((editPlayerViewAnglePaste, (ALL_SUPPORTED_GAMES)))\
		((editPlayerViewAngleAdjustHorizontal, (ALL_SUPPORTED_GAMES)))\
		((editPlayerViewAngleAdjustVertical, (ALL_SUPPORTED_GAMES)))\
		((editPlayerViewAngleAdjustFactor, (ALL_SUPPORTED_GAMES)))\
	((editPlayerViewAngleIDSubheading, (ALL_SUPPORTED_GAMES)))\
		((editPlayerViewAngleIDSet, (ALL_SUPPORTED_GAMES)))\
		((editPlayerViewAngleIDInt, (ALL_SUPPORTED_GAMES)))\
		((editPlayerViewAngleIDFillCurrent, (ALL_SUPPORTED_GAMES)))\
		((editPlayerViewAngleIDCopy, (ALL_SUPPORTED_GAMES)))\
		((editPlayerViewAngleIDPaste, (ALL_SUPPORTED_GAMES)))\
		((editPlayerViewAngleIDAdjustPositive, (ALL_SUPPORTED_GAMES)))\
		((editPlayerViewAngleIDAdjustNegative, (ALL_SUPPORTED_GAMES)))\
		((editPlayerViewAngleIDAdjustFactor, (ALL_SUPPORTED_GAMES)))\
	((freeCameraToggleGUI, (FREE_CAMERA_SUPPORT)))\
	((freeCameraSettingsSimpleSubheading, (FREE_CAMERA_SUPPORT)))\
	((freeCameraSettingsAdvancedSubheading, (FREE_CAMERA_SUPPORT)))\
		((freeCameraTeleportToCamera, (FREE_CAMERA_SUPPORT)))\
			((freeCameraTeleportToCameraSlightlyBehind, (FREE_CAMERA_SUPPORT)))\
		((freeCameraThirdPersonRendering, (FREE_CAMERA_SUPPORT)))\
		((freeCameraDisableScreenEffects, (Halo1, Halo2, HaloReach)))\
		((freeCameraGameInputDisable, (FREE_CAMERA_SUPPORT)))\
		((freeCameraCameraInputDisable, (FREE_CAMERA_SUPPORT)))\
		((freeCameraUserInputCameraSettings, (FREE_CAMERA_SUPPORT)))\
			((freeCameraUserInputCameraBindingsSubheading, (FREE_CAMERA_SUPPORT)))\
				((freeCameraUserInputTranslateUpBinding, (FREE_CAMERA_SUPPORT)))\
				((freeCameraUserInputTranslateDownBinding, (FREE_CAMERA_SUPPORT)))\
				((freeCameraUserInputRollLeftBinding, (FREE_CAMERA_SUPPORT)))\
				((freeCameraUserInputRollRightBinding, (FREE_CAMERA_SUPPORT)))\
				((freeCameraUserInputFOVIncreaseBinding, (FREE_CAMERA_SUPPORT)))\
				((freeCameraUserInputFOVDecreaseBinding, (FREE_CAMERA_SUPPORT)))\
			((freeCameraUserInputCameraBaseFOV, (FREE_CAMERA_SUPPORT)))\
			((freeCameraUserInputCameraTranslationSpeed, (FREE_CAMERA_SUPPORT)))\
			((freeCameraUserInputCameraIncreaseTranslationSpeedHotkey, (FREE_CAMERA_SUPPORT)))\
			((freeCameraUserInputCameraDecreaseTranslationSpeedHotkey, (FREE_CAMERA_SUPPORT)))\
			((freeCameraUserInputCameraTranslationSpeedChangeFactor, (FREE_CAMERA_SUPPORT)))\
			defFreeCameraInterpolator(UserInputCameraTranslation)\
			((freeCameraUserInputCameraRotationSpeed, (FREE_CAMERA_SUPPORT)))\
			defFreeCameraInterpolator(UserInputCameraRotation)\
			((freeCameraUserInputCameraRotationScalesToFOV, (FREE_CAMERA_SUPPORT)))\
			((freeCameraUserInputCameraFOVSpeed, (FREE_CAMERA_SUPPORT)))\
			defFreeCameraInterpolator(UserInputCameraFOV)\
			((freeCameraUserInputCameraNonLinearFOVAtMinimum, (FREE_CAMERA_SUPPORT)))\
			((freeCameraUserInputCameraNonLinearFOVAtMaximum, (FREE_CAMERA_SUPPORT)))\
			((freeCameraUserInputCameraSetPosition, (FREE_CAMERA_SUPPORT)))\
			((freeCameraUserInputCameraSetPositionChildren, (FREE_CAMERA_SUPPORT)))\
				((freeCameraUserInputCameraSetPositionVec3, (FREE_CAMERA_SUPPORT)))\
				((freeCameraUserInputCameraSetPositionFillCurrent, (FREE_CAMERA_SUPPORT)))\
				((freeCameraUserInputCameraSetPositionCopy, (FREE_CAMERA_SUPPORT)))\
				((freeCameraUserInputCameraSetPositionPaste, (FREE_CAMERA_SUPPORT)))\
			((freeCameraUserInputCameraSetRotation, (FREE_CAMERA_SUPPORT)))\
			((freeCameraUserInputCameraSetRotationChildren, (FREE_CAMERA_SUPPORT)))\
				((freeCameraUserInputCameraSetRotationVec3, (FREE_CAMERA_SUPPORT)))\
				((freeCameraUserInputCameraSetRotationFillCurrent, (FREE_CAMERA_SUPPORT)))\
				((freeCameraUserInputCameraSetRotationCopy, (FREE_CAMERA_SUPPORT)))\
				((freeCameraUserInputCameraSetRotationPaste, (FREE_CAMERA_SUPPORT)))\
	((changeOOBBackgroundToggle, (Halo3)))\
		((changeOOBBackgroundColor, (Halo3)))\
	((disableFogToggle, (ALL_SUPPORTED_GAMES)))\
((theaterHeadingGUI, (Halo3,Halo3ODST,HaloReach,Halo4)))\
((debugHeadingGUI, (ALL_SUPPORTED_GAMES)))\
	((getPlayerDatumGUI, (ALL_SUPPORTED_GAMES)))\
	((getObjectAddressGUI, (ALL_SUPPORTED_GAMES)))\
	((getTagAddressGUI, (ALL_SUPPORTED_GAMES)))\


#define DEBUGGUIELEMENTS_ANDSUPPORTEDGAMES \
((HCMDebugHeadingGUI, (ALL_SUPPORTED_GAMES)))\





//#ifdef HCM_DEBUG
//
//#define ALLGUIELEMENTS_ANDSUPPORTEDGAMES BOOST_PP_CAT(RELEASEGUIELEMENTS_ANDSUPPORTEDGAMES, DEBUGGUIELEMENTS_ANDSUPPORTEDGAMES)
//
//#else 
//
//#define ALLGUIELEMENTS_ANDSUPPORTEDGAMES RELEASEGUIELEMENTS_ANDSUPPORTEDGAMES
//
//#endif




#define MAKE_FIRSTOFPAIR_SET(r, d, seq) BOOST_PP_TUPLE_ELEM(0, seq),
#define MAKE_ALL_FIRSTOFPAIR(seq) BOOST_PP_SEQ_FOR_EACH(MAKE_FIRSTOFPAIR_SET, _, seq)
#define ALLGUIELEMENTS1 MAKE_ALL_FIRSTOFPAIR(RELEASEGUIELEMENTS_ANDSUPPORTEDGAMES1)

#define ALLGUIELEMENTS2 MAKE_ALL_FIRSTOFPAIR(RELEASEGUIELEMENTS_ANDSUPPORTEDGAMES2)
#define ALLGUIELEMENTSDEBUG MAKE_ALL_FIRSTOFPAIR(DEBUGGUIELEMENTS_ANDSUPPORTEDGAMES)

enum class GUIElementEnum {
	ALLGUIELEMENTS1
	ALLGUIELEMENTS2
#ifdef HCM_DEBUG
	ALLGUIELEMENTSDEBUG
#endif
};
