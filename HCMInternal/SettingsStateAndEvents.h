#pragma once
#include "UnarySetting.h"
#include "BinarySetting.h"
#include "ISettingsSerialiser.h"
#include "GUIServiceInfo.h"
#include "WaypointList.h"
#include "SkullEnum.h"
#include "BitBoolPointer.h"
#include "SettingsEnums.h"

class SettingsStateAndEvents
{
private:
	std::shared_ptr<ISettingsSerialiser> mSerialiser;

public:
	SettingsStateAndEvents(std::shared_ptr<ISettingsSerialiser> serialiser)
		: mSerialiser(serialiser)
	{ 
		// deserialise (load) serialisable options
		mSerialiser->deserialise(allSerialisableOptions); 

	}
	~SettingsStateAndEvents() {
		PLOG_DEBUG << "~SettingsStateAndEvents()";
		// serialise (save) serialisable options
		mSerialiser->serialise(allSerialisableOptions); 
	};

	//	hotkeys - see HotkeyEventsLambdas for how they connect to respective toggle (they just flip the value)
	std::shared_ptr<ActionEvent> toggleGUIHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> togglePauseHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> speedhackHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> invulnerabilityHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> aiFreezeHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> medusaHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> naturalCheckpointDisableHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> infiniteAmmoHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> bottomlessClipHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> display2DInfoHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> freeCameraHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> freeCameraGameInputDisableHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> freeCameraCameraInputDisableHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> freeCameraUserInputCameraIncreaseTranslationSpeedHotkey = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> freeCameraUserInputCameraDecreaseTranslationSpeedHotkey = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> freeCameraUserInputCameraMaintainVelocityHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> freeCameraAnchorPositionToObjectPositionHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> freeCameraAnchorPositionToObjectRotationHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> freeCameraAnchorRotationToObjectPositionHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> freeCameraAnchorRotationToObjectFacingHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> freeCameraAnchorFOVToObjectDistanceHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> hideHUDToggleHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> setPlayerHealthEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> toggleWaypoint3DHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> triggerOverlayToggleHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> softCeilingOverlayToggleHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> disableBarriersHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> abilityMeterToggleHotkeyEvent = std::make_shared<ActionEvent>();

	
	// events
	std::shared_ptr<ActionEvent> showGUIFailures = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> advanceTicksEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> forceCheckpointEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> forceRevertEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> forceDoubleRevertEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> forceCoreSaveEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> forceCoreLoadEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> injectCheckpointEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> dumpCheckpointEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> injectCoreEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> dumpCoreEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> commandConsoleHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> commandConsoleExecuteBufferEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> getObjectAddressEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> getTagAddressEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> playerPositionToClipboardEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> forceTeleportEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> forceTeleportAbsoluteFillCurrent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> forceTeleportAbsoluteCopy = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> forceTeleportAbsolutePaste = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> forceLaunchEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> editPlayerViewAngleSet = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> editPlayerViewAngleFillCurrent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> editPlayerViewAngleCopy = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> editPlayerViewAnglePaste = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> editPlayerViewAngleAdjustHorizontal = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> editPlayerViewAngleAdjustVertical = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> freeCameraTeleportToCameraEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> freeCameraUserInputCameraBindingsPopup = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> freeCameraUserInputCameraSetPosition = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> freeCameraUserInputCameraSetPositionFillCurrent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> freeCameraUserInputCameraSetPositionCopy = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> freeCameraUserInputCameraSetPositionPaste = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> freeCameraUserInputCameraSetRotation = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> freeCameraUserInputCameraSetRotationFillCurrent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> freeCameraUserInputCameraSetRotationCopy = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> freeCameraUserInputCameraSetRotationPaste = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> freeCameraUserInputCameraSetVelocity = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> freeCameraUserInputCameraSetVelocityFillCurrent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> freeCameraUserInputCameraSetVelocityCopy = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> freeCameraUserInputCameraSetVelocityPaste = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> switchBSPEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> switchBSPSetLoadSetEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> switchBSPLoadIndexEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> switchBSPUnloadIndexEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> switchBSPSetFillCurrent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> forceFutureCheckpointFillEvent = std::make_shared<ActionEvent>(); 
	std::shared_ptr<ActionEvent> triggerOverlayFilterStringDialogEvent = std::make_shared<ActionEvent>(); 
	std::shared_ptr<ActionEvent> triggerOverlayFilterStringCopyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> triggerOverlayFilterStringPasteEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> getPlayerDatumEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> sensResetCountsEvent = std::make_shared<ActionEvent>();

	

	// waypoint events (delete, edit)
	std::shared_ptr<WaypointAndListEvent> deleteWaypointEvent = std::make_shared<WaypointAndListEvent>();
	std::shared_ptr<WaypointAndListEvent> editWaypointEvent = std::make_shared<WaypointAndListEvent>();
	std::shared_ptr<WaypointListEvent> addWaypointEvent = std::make_shared<WaypointListEvent>();


	// skulls
	std::shared_ptr<eventpp::CallbackList<void(GameState)>> updateSkullBitBoolCollectionEvent = std::make_shared<eventpp::CallbackList<void(GameState)>>(); // called by GUI to tell SkullToggler to update pointer data
	std::map<SkullEnum, BitBoolPointer> skullBitBoolCollection; // pointer data is cached and updated on MCC gamestate change
	std::atomic_bool skullBitBoolCollectionInUse = false; // locked while cache is being updated or in use by gui


	// hotkeys for each skull
	std::shared_ptr<ActionEvent> skullAngerToggleHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> skullAssassinsToggleHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> skullBlackEyeToggleHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> skullBlindToggleHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> skullCatchToggleHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> skullEyePatchToggleHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> skullFamineToggleHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> skullFogToggleHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> skullForeignToggleHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> skullIronToggleHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> skullJackedToggleHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> skullMasterblasterToggleHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> skullMythicToggleHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> skullRecessionToggleHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> skullSoAngryToggleHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> skullStreakingToggleHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> skullSwarmToggleHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> skullThatsJustWrongToggleHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> skullTheyComeBackToggleHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> skullThunderstormToggleHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> skullTiltToggleHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> skullToughLuckToggleHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> skullBandannaToggleHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> skullBondedPairToggleHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> skullBoomToggleHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> skullCowbellToggleHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> skullEnvyToggleHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> skullFeatherToggleHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> skullGhostToggleHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> skullGruntBirthdayPartyToggleHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> skullGruntFuneralToggleHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> skullIWHBYDToggleHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> skullMalfunctionToggleHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> skullPinataToggleHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> skullProphetBirthdayPartyToggleHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> skullScarabToggleHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> skullSputnikToggleHotkeyEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> skullAcrophobiaToggleHotkeyEvent = std::make_shared<ActionEvent>();


	// settings

	std::shared_ptr<BinarySetting<float>> messagesFontSize = std::make_shared<BinarySetting<float>>
		(
			10.f,
			[](float in) { return in > 6.f; }, // must be positive
			nameof(messagesFontSize)
		);

	std::shared_ptr<BinarySetting<SimpleMath::Vector4>> messagesFontColor = std::make_shared<BinarySetting<SimpleMath::Vector4>>
		(
			SimpleMath::Vector4{ 1.f, 0.2f, 0.f, 1.f },
			[](SimpleMath::Vector4 in) { return true; },
			nameof(messagesFontColor)
		);

	std::shared_ptr<BinarySetting<bool>> GUIWindowOpen = std::make_shared<BinarySetting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(GUIWindowOpen)
		);

	std::shared_ptr<BinarySetting<bool>> GUIShowingFreesCursor = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(GUIShowingFreesCursor)
		);


	std::shared_ptr<BinarySetting<bool>> GUIShowingBlocksInput = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(GUIShowingBlocksInput)
		);

	std::shared_ptr<BinarySetting<bool>> GUIShowingPausesGame = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(GUIShowingPausesGame)
		);

	std::shared_ptr<BinarySetting<bool>> togglePause = std::make_shared<BinarySetting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(togglePause)
		);

	std::shared_ptr<BinarySetting<bool>> pauseAlsoBlocksInput = std::make_shared<BinarySetting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(pauseAlsoBlocksInput)
		);

	std::shared_ptr<BinarySetting<bool>> pauseAlsoFreesCursor = std::make_shared<BinarySetting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(pauseAlsoFreesCursor)
		);

	std::shared_ptr<BinarySetting<int>> advanceTicksCount = std::make_shared<BinarySetting<int>>
		(
			1,
			[](int in) { return in > 0; }, // must be positive
			nameof(advanceTicksCount)
		);


	std::shared_ptr<BinarySetting<bool>> hideWatermark = std::make_shared<BinarySetting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(hideWatermark)
		);

	std::shared_ptr<BinarySetting<bool>> hideWatermarkHideMessages = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(hideWatermarkHideMessages)
		);

	std::shared_ptr<BinarySetting<bool>> injectionIgnoresChecksum = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(injectionIgnoresChecksum)
		);

	std::shared_ptr<BinarySetting<bool>> injectCheckpointForcesRevert = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(injectCheckpointForcesRevert)
		);

	std::shared_ptr<BinarySetting<bool>> injectCheckpointLevelCheck = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(injectCheckpointLevelCheck)
		);

	std::shared_ptr<BinarySetting<bool>> injectCheckpointVersionCheck = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(injectCheckpointVersionCheck)
		);

	std::shared_ptr<BinarySetting<bool>> injectCheckpointDifficultyCheck = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(injectCheckpointDifficultyCheck)
		);

	std::shared_ptr<BinarySetting<bool>> autonameCheckpoints = std::make_shared<BinarySetting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(autonameCheckpoints)
		);

	std::shared_ptr<BinarySetting<bool>> dumpCheckpointForcesSave = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(dumpCheckpointForcesSave)
		);

	std::shared_ptr<BinarySetting<bool>> injectCoreForcesRevert = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(injectCoreForcesRevert)
		);

	std::shared_ptr<BinarySetting<bool>> injectCoreLevelCheck = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(injectCoreLevelCheck)
		);

	std::shared_ptr<BinarySetting<bool>> injectCoreVersionCheck = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(injectCoreVersionCheck)
		);

	std::shared_ptr<BinarySetting<bool>> injectCoreDifficultyCheck = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(injectCoreDifficultyCheck)
		);

	std::shared_ptr<BinarySetting<bool>> dumpCoreForcesSave = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(dumpCoreForcesSave)
		);

	std::shared_ptr<BinarySetting<bool>> autonameCoresaves = std::make_shared<BinarySetting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(autonameCoresaves)
		);

	std::shared_ptr<BinarySetting<bool>> forceFutureCheckpointToggle = std::make_shared<BinarySetting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(forceFutureCheckpointToggle)
		);

	std::shared_ptr<BinarySetting<int>> forceFutureCheckpointTick = std::make_shared<BinarySetting<int>>
		(
			1000,
			[](int in) { return in >= 0; },
			nameof(forceFutureCheckpointTick)
		);

	std::shared_ptr<BinarySetting<bool>> speedhackToggle = std::make_shared<BinarySetting<bool>>
	(
		false,
		[](bool in) { return true; },
		nameof(speedhackToggle)
	);

	std::shared_ptr<BinarySetting<double>> speedhackSetting = std::make_shared<BinarySetting<double>>
	(
		10.f,
		[](double in) { return in > 0; }, // must be positive
		nameof(speedhackSetting)
	);


	std::shared_ptr<BinarySetting<bool>> invulnerabilityToggle = std::make_shared<BinarySetting<bool>>
	(
		false,
		[](bool in) { return true; },
		nameof(invulnerabilityToggle)
	);

	std::shared_ptr<BinarySetting<bool>> invulnerabilityNPCToggle = std::make_shared<BinarySetting<bool>>
	(
		false,
		[](bool in) { return true; },
		nameof(invulnerabilityNPCToggle)
	);

	std::shared_ptr<BinarySetting<bool>> aiFreezeToggle = std::make_shared<BinarySetting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(aiFreezeToggle)
		);

	std::shared_ptr<BinarySetting<bool>> medusaToggle = std::make_shared<BinarySetting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(medusaToggle)
		);



	std::shared_ptr<BinarySetting<bool>> consoleCommandPauseGame = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(consoleCommandPauseGame)
		);

	std::shared_ptr<BinarySetting<bool>> consoleCommandBlockInput = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(consoleCommandBlockInput)
		);

	std::shared_ptr<BinarySetting<bool>> consoleCommandFreeCursor = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(consoleCommandFreeCursor)
		);

	std::shared_ptr<BinarySetting<float>> consoleCommandFontSize = std::make_shared<BinarySetting<float>>
		(
			16.f,
			[](int in) { return in > 6.f; },
			nameof(consoleCommandFontSize)
		);


	

	std::shared_ptr<BinarySetting<uint32_t>> getObjectAddressDWORD = std::make_shared<BinarySetting<uint32_t>>
		(
			0xDEADBEEF,
			[](uint32_t in) { return true; },
			nameof(getObjectAddressDWORD)
		);

	std::shared_ptr<BinarySetting<uint32_t>> getTagAddressDWORD = std::make_shared<BinarySetting<uint32_t>>
		(
			0xDEADBEEF,
			[](uint32_t in) { return true; },
			nameof(getTagAddressDWORD)
		);

	std::shared_ptr<BinarySetting<bool>> forceTeleportApplyToPlayer = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(forceTeleportApplyToPlayer)
		);

	std::shared_ptr<BinarySetting<uint32_t>> forceTeleportCustomObject = std::make_shared<BinarySetting<uint32_t>>
		(
			0xDEADBEEF,
			[](uint32_t in) { return true; },
			nameof(forceTeleportCustomObject)
		);


	std::shared_ptr<BinarySetting<SimpleMath::Vector3>> forceTeleportAbsoluteVec3 = std::make_shared<BinarySetting<SimpleMath::Vector3>>
		(
			SimpleMath::Vector3{ 0.f, 0.f, 0.f },
			[](SimpleMath::Vector3 in) { return true; },
			nameof(forceTeleportAbsoluteVec3)
		);

	std::shared_ptr<BinarySetting<SimpleMath::Vector3>> forceTeleportRelativeVec3 = std::make_shared<BinarySetting<SimpleMath::Vector3>>
		(
			SimpleMath::Vector3{ 5.f, 0.f, 0.f },
			[](SimpleMath::Vector3 in) { return true; },
			nameof(forceTeleportRelativeVec3)
		);

	std::shared_ptr<BinarySetting<bool>> forceTeleportForward = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(forceTeleportForward)
		);



	std::shared_ptr<BinarySetting<bool>> forceTeleportForwardIgnoreZ = std::make_shared<BinarySetting<bool>> // ignore vertical component of players look angle
		(
			false,
			[](bool in) { return true; },
			nameof(forceTeleportForwardIgnoreZ)
		);



	std::shared_ptr<BinarySetting<bool>> forceTeleportManual = std::make_shared<BinarySetting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(forceTeleportManual)
		);

	std::shared_ptr<BinarySetting<bool>> forceLaunchApplyToPlayer = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(forceLaunchApplyToPlayer)
		);

	std::shared_ptr<BinarySetting<uint32_t>> forceLaunchCustomObject = std::make_shared<BinarySetting<uint32_t>>
		(
			0xDEADB33F,
			[](uint32_t in) { return true; },
			nameof(forceLaunchCustomObject)
		);

	std::shared_ptr<BinarySetting<SimpleMath::Vector3>> forceLaunchAbsoluteVec3 = std::make_shared<BinarySetting<SimpleMath::Vector3>>
		(
			SimpleMath::Vector3{ 0.f, 0.f, 5.f },
			[](SimpleMath::Vector3 in) { return true; },
			nameof(forceLaunchAbsoluteVec3)
		);

	std::shared_ptr<BinarySetting<SimpleMath::Vector3>> forceLaunchRelativeVec3 = std::make_shared<BinarySetting<SimpleMath::Vector3>>
		(
			SimpleMath::Vector3{ 0.5f, 0.f, 0.f },
			[](SimpleMath::Vector3 in) { return true; },
			nameof(forceLaunchRelativeVec3)
		);


	std::shared_ptr<BinarySetting<bool>> forceLaunchForward = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(forceLaunchForward)
		);

	std::shared_ptr<BinarySetting<bool>> forceLaunchForwardIgnoreZ = std::make_shared<BinarySetting<bool>> // ignore vertical component of players look angle
		(
			false,
			[](bool in) { return true; },
			nameof(forceLaunchForwardIgnoreZ)
		);

	std::shared_ptr<BinarySetting<bool>> forceLaunchManual = std::make_shared<BinarySetting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(forceLaunchManual)
		);



	std::shared_ptr<BinarySetting<bool>> naturalCheckpointDisable = std::make_shared<BinarySetting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(naturalCheckpointDisable)
		);

	std::shared_ptr<BinarySetting<bool>> infiniteAmmoToggle = std::make_shared<BinarySetting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(infiniteAmmoToggle)
		);

	std::shared_ptr<BinarySetting<bool>> bottomlessClipToggle = std::make_shared<BinarySetting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(bottomlessClipToggle)
		);

	std::shared_ptr<BinarySetting<bool>> display2DInfoToggle = std::make_shared<BinarySetting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(display2DInfoToggle)
		);

	std::shared_ptr<BinarySetting<bool>> display2DInfoShowGameTick = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(display2DInfoShowGameTick)
		);

	std::shared_ptr<BinarySetting<bool>> display2DInfoShowNextObjectDatum = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(display2DInfoShowNextObjectDatum)
		);

	std::shared_ptr<BinarySetting<bool>> display2DInfoShowAggro = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(display2DInfoShowAggro)
		);

	std::shared_ptr<BinarySetting<bool>> display2DInfoShowRNG = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(display2DInfoShowRNG)
		);

	std::shared_ptr<BinarySetting<bool>> display2DInfoShowBSP = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(display2DInfoShowBSP)
		);

	std::shared_ptr<BinarySetting<bool>> display2DInfoShowBSPSet = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(display2DInfoShowBSPSet)
		);

	std::shared_ptr<BinarySetting<bool>> display2DInfoTrackPlayer = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(display2DInfoTrackPlayer)
		);

	std::shared_ptr<BinarySetting<bool>> display2DInfoShowPlayerViewAngle= std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(display2DInfoShowPlayerViewAngle)
		);





	std::shared_ptr<BinarySetting<bool>> display2DInfoShowPlayerPosition = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(display2DInfoShowPlayerPosition)
		);

	std::shared_ptr<BinarySetting<bool>> display2DInfoShowPlayerVelocity = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(display2DInfoShowPlayerVelocity)
		);

	std::shared_ptr<BinarySetting<bool>> display2DInfoShowPlayerVelocityAbs = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(display2DInfoShowPlayerVelocityAbs)
		);

	std::shared_ptr<BinarySetting<bool>> display2DInfoShowPlayerVelocityXY = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(display2DInfoShowPlayerVelocityXY)
		);

	std::shared_ptr<BinarySetting<bool>> display2DInfoShowPlayerVelocityXYZ = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(display2DInfoShowPlayerVelocityXYZ)
		);

	std::shared_ptr<BinarySetting<bool>> display2DInfoShowPlayerHealth = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(display2DInfoShowPlayerHealth)
		);


	std::shared_ptr<BinarySetting<bool>> display2DInfoShowPlayerRechargeCooldown = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(display2DInfoShowPlayerRechargeCooldown)
		);

	std::shared_ptr<BinarySetting<bool>> display2DInfoShowPlayerVehicleHealth = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(display2DInfoShowPlayerVehicleHealth)
		);



	std::shared_ptr<BinarySetting<bool>> display2DInfoTrackCustomObject = std::make_shared<BinarySetting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(display2DInfoTrackCustomObject)
		);

	std::shared_ptr<BinarySetting<uint32_t>> display2DInfoCustomObjectDatum = std::make_shared<BinarySetting<uint32_t>>
		(
			0xDEADB33F,
			[](uint32_t in) { return true; },
			nameof(display2DInfoCustomObjectDatum)
		);

	std::shared_ptr<BinarySetting<bool>> display2DInfoShowEntityObjectType = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(display2DInfoShowEntityObjectType)
		);

	std::shared_ptr<BinarySetting<bool>> display2DInfoShowEntityTagName = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(display2DInfoShowEntityTagName)
		);


	std::shared_ptr<BinarySetting<bool>> display2DInfoShowEntityPosition = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(display2DInfoShowEntityPosition)
		);

	std::shared_ptr<BinarySetting<bool>> display2DInfoShowEntityVelocity = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(display2DInfoShowEntityVelocity)
		);

	std::shared_ptr<BinarySetting<bool>> display2DInfoShowEntityVelocityAbs = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(display2DInfoShowEntityVelocityAbs)
		);

	std::shared_ptr<BinarySetting<bool>> display2DInfoShowEntityVelocityXY = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(display2DInfoShowEntityVelocityXY)
		);

	std::shared_ptr<BinarySetting<bool>> display2DInfoShowEntityVelocityXYZ = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(display2DInfoShowEntityVelocityXYZ)
		);

	std::shared_ptr<BinarySetting<bool>> display2DInfoShowEntityHealth = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(display2DInfoShowEntityHealth)
		);

	std::shared_ptr<BinarySetting<bool>> display2DInfoShowEntityRechargeCooldown = std::make_shared<BinarySetting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(display2DInfoShowEntityRechargeCooldown)
		);

	std::shared_ptr<BinarySetting<bool>> display2DInfoShowEntityVehicleHealth = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(display2DInfoShowEntityVehicleHealth)
		);


	std::shared_ptr<BinarySetting<SettingsEnums::ScreenAnchorEnum>> display2DInfoAnchorCorner = std::make_shared<BinarySetting<SettingsEnums::ScreenAnchorEnum>>
		(
			SettingsEnums::ScreenAnchorEnum::BottomRight,
			[](SettingsEnums::ScreenAnchorEnum in) { return true; },
			nameof(display2DInfoAnchorCorner)
		);

	std::shared_ptr<BinarySetting<SimpleMath::Vector2>> display2DInfoScreenOffset = std::make_shared<BinarySetting<SimpleMath::Vector2>>
		(
			SimpleMath::Vector2{400, 400},
			[](SimpleMath::Vector2 in) { return in.x >= 0 && in.y >= 0; }, // no negative offsets
			nameof(display2DInfoScreenOffset)
		);

	std::shared_ptr<BinarySetting<float>> display2DInfoFontSize = std::make_shared<BinarySetting<float>>
		(
			16.f, 
			[](int in) { return in > 6.f ; },
			nameof(display2DInfoFontSize)
		);

	std::shared_ptr<BinarySetting<SimpleMath::Vector4>> display2DInfoFontColour = std::make_shared<BinarySetting<SimpleMath::Vector4>>
		(
			SimpleMath::Vector4{1.00f, 0.60f, 0.25f, 1.00f}, // a nice orange colour that matches the rest of the gui
			[](SimpleMath::Vector4 in) { return in.x >= 0 && in.y >= 0 && in.z >= 0 && in.w >= 0 && in.x <= 1 && in.y <= 1 && in.z <= 1 && in.w <= 1; }, // range 0.f ... 1.f 
			nameof(display2DInfoFontColour)
		);

	std::shared_ptr<BinarySetting<int>> display2DInfoFloatPrecision = std::make_shared<BinarySetting<int>>
		(
			6,
			[](int in) { return in >= 0; },
			nameof(display2DInfoFloatPrecision)
		);

	std::shared_ptr<BinarySetting<bool>> display2DInfoOutline = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(display2DInfoOutline)
		);

	std::shared_ptr<BinarySetting<bool>> hideHUDToggle = std::make_shared<BinarySetting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(hideHUDToggle)
		);

	std::shared_ptr<BinarySetting<SimpleMath::Vector2>> editPlayerViewAngleVec2 = std::make_shared<BinarySetting<SimpleMath::Vector2>>
		(
			SimpleMath::Vector2(4.20, 0.69), // gottem
			[](SimpleMath::Vector2 in) { return (in.x >= 0.f) && (in.x < (std::numbers::pi_v<float> * 2.f)) && (in.y < (std::numbers::pi_v<float> / 2.f)) && (in.y > ((std::numbers::pi_v<float> / 2.f) * -1.f)); }, // x (yaw) must be from 0 to 6.14, y (pitch) must be from -1.57 to 1.57
			nameof(editPlayerViewAngleVec2)
		);

	std::shared_ptr<BinarySetting<float>> editPlayerViewAngleAdjustFactor = std::make_shared<BinarySetting<float>>
		(
			0.1f,
			[](float in) { return true; },
			nameof(editPlayerViewAngleAdjustFactor)
		);





	std::shared_ptr<BinarySetting<bool>> freeCameraToggle = std::make_shared<BinarySetting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(freeCameraToggle)
		);


	std::shared_ptr<BinarySetting<bool>> freeCameraThirdPersonRendering = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(freeCameraThirdPersonRendering)
		);

	std::shared_ptr<BinarySetting<bool>> freeCameraDisableScreenEffects = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(freeCameraDisableScreenEffects)
		);




	std::shared_ptr<BinarySetting<bool>> freeCameraGameInputDisable = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(freeCameraGameInputDisable)
		);

	std::shared_ptr<BinarySetting<bool>> freeCameraCameraInputDisable = std::make_shared<BinarySetting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(freeCameraCameraInputDisable)
		);


	std::shared_ptr<BinarySetting<bool>> freeCameraTeleportToCameraSlightlyBehind = std::make_shared<BinarySetting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(freeCameraTeleportToCameraSlightlyBehind)
		);

	

	std::shared_ptr<BinarySetting<float>> freeCameraUserInputCameraTranslationSpeedChangeFactor = std::make_shared<BinarySetting<float>>
		(
			1.5f,
			[](float in) { return in > 1.f; }, // 
			nameof(freeCameraUserInputCameraTranslationSpeedChangeFactor)
		);

	std::shared_ptr<BinarySetting<SimpleMath::Vector3>> freeCameraUserInputCameraSetPositionVec3 = std::make_shared<BinarySetting<SimpleMath::Vector3>>
		(
			SimpleMath::Vector3{ 5.f, 0.f, 0.f },
			[](SimpleMath::Vector3 in) { return true; },
			nameof(freeCameraUserInputCameraSetPositionVec3)
		);

	std::shared_ptr<BinarySetting<SimpleMath::Vector3>> freeCameraUserInputCameraSetRotationVec3 = std::make_shared<BinarySetting<SimpleMath::Vector3>>
		(
			SimpleMath::Vector3{ 0.f, 0.f, 0.f },
			[](SimpleMath::Vector3 in) { return true; },
			nameof(freeCameraUserInputCameraSetRotationVec3)
		);

	std::shared_ptr<BinarySetting<bool>> freeCameraUserInputCameraMaintainVelocity = std::make_shared<BinarySetting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(freeCameraUserInputCameraMaintainVelocity)
		);

	std::shared_ptr<BinarySetting<SimpleMath::Vector3>> freeCameraUserInputCameraSetVelocityVec3 = std::make_shared<BinarySetting<SimpleMath::Vector3>>
		(
			SimpleMath::Vector3{ 5.f, 0.f, 0.f },
			[](SimpleMath::Vector3 in) { return true; },
			nameof(freeCameraUserInputCameraSetVelocityVec3)
		);

	std::shared_ptr<BinarySetting<float>> freeCameraUserInputCameraBaseFOV = std::make_shared<BinarySetting<float>>
		(
			90.f,
			[](float in) { return in > 0.f && in < 180.f; }, 
			nameof(freeCameraUserInputCameraBaseFOV)
		);


	std::shared_ptr<BinarySetting<float>> freeCameraUserInputCameraTranslationSpeed = std::make_shared<BinarySetting<float>>
		(
			3.f,
			[](float in) { return true; }, // if the user wants to have a negative speed that's their perogative
			nameof(freeCameraUserInputCameraTranslationSpeed)
		); 
	
	std::shared_ptr<BinarySetting<SettingsEnums::FreeCameraInterpolationTypesEnum>> freeCameraUserInputCameraTranslationInterpolator = std::make_shared<BinarySetting<SettingsEnums::FreeCameraInterpolationTypesEnum>>
		(
			SettingsEnums::FreeCameraInterpolationTypesEnum::Linear,
			[](SettingsEnums::FreeCameraInterpolationTypesEnum in) { return true; },
			nameof(freeCameraUserInputCameraTranslationInterpolator)
		);

	std::shared_ptr<BinarySetting<float>> freeCameraUserInputCameraTranslationInterpolatorLinearFactor = std::make_shared<BinarySetting<float>>
		(
			0.06f,
			[](float in) { return in > 0.f && in <= 1.f; },
			nameof(freeCameraUserInputCameraTranslationInterpolatorLinearFactor)
		);


	std::shared_ptr<BinarySetting<float>> freeCameraUserInputCameraRotationSpeed = std::make_shared<BinarySetting<float>>
		(
			3.f,
			[](float in) { return true; }, // if the user wants to have a negative speed that's their perogative
			nameof(freeCameraUserInputCameraRotationSpeed)
		);

	// actually an enum but stored as int
	std::shared_ptr<BinarySetting<SettingsEnums::FreeCameraInterpolationTypesEnum>> freeCameraUserInputCameraRotationInterpolator = std::make_shared<BinarySetting<SettingsEnums::FreeCameraInterpolationTypesEnum>>
		(
			SettingsEnums::FreeCameraInterpolationTypesEnum::Linear,
			[](SettingsEnums::FreeCameraInterpolationTypesEnum in) { return true; },
			nameof(freeCameraUserInputCameraRotationInterpolator)
		);

	std::shared_ptr<BinarySetting<float>> freeCameraUserInputCameraRotationInterpolatorLinearFactor = std::make_shared<BinarySetting<float>>
		(
			0.06f,
			[](float in) { return in > 0.f && in <= 1.f; },
			nameof(freeCameraUserInputCameraRotationInterpolatorLinearFactor)
		);



	std::shared_ptr<BinarySetting<float>> freeCameraUserInputCameraFOVSpeed = std::make_shared<BinarySetting<float>>
		(
			1.f,
			[](float in) { return true; }, // if the user wants to have a negative speed that's their perogative
			nameof(freeCameraUserInputCameraFOVSpeed)
		);


	std::shared_ptr<BinarySetting<SettingsEnums::FreeCameraInterpolationTypesEnum>> freeCameraUserInputCameraFOVInterpolator = std::make_shared<BinarySetting<SettingsEnums::FreeCameraInterpolationTypesEnum>>
		(
			SettingsEnums::FreeCameraInterpolationTypesEnum::Linear,
			[](SettingsEnums::FreeCameraInterpolationTypesEnum in) { return true; },
			nameof(freeCameraUserInputCameraFOVInterpolator)
		);

	std::shared_ptr<BinarySetting<float>> freeCameraUserInputCameraFOVInterpolatorLinearFactor = std::make_shared<BinarySetting<float>>
		(
			0.06f,
			[](float in) { return in > 0.f && in <= 1.f; },
			nameof(freeCameraUserInputCameraFOVInterpolatorLinearFactor)
		);

	std::shared_ptr<BinarySetting<bool>> freeCameraAnchorPositionToObjectPosition = std::make_shared<BinarySetting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(freeCameraAnchorPositionToObjectPosition)
		);


		std::shared_ptr<BinarySetting<SettingsEnums::FreeCameraObjectTrackEnum>> freeCameraAnchorPositionToObjectPositionObjectToTrackComboGroup = std::make_shared<BinarySetting<SettingsEnums::FreeCameraObjectTrackEnum>>
			(
				SettingsEnums::FreeCameraObjectTrackEnum::Player,
				[](SettingsEnums::FreeCameraObjectTrackEnum in) { return true; },
				nameof(freeCameraAnchorPositionToObjectPositionObjectToTrackComboGroup)
			);

	std::shared_ptr<BinarySetting<uint32_t>> freeCameraAnchorPositionToObjectPositionObjectToTrackCustomObjectDatum = std::make_shared<BinarySetting<uint32_t>>
		(
			0xDEADBEEF,
			[](uint32_t in) { return true; },
			nameof(freeCameraAnchorPositionToObjectPositionObjectToTrackCustomObjectDatum)
		);

	std::shared_ptr<BinarySetting<SettingsEnums::FreeCameraInterpolationTypesEnum>> freeCameraAnchorPositionToObjectPositionTranslationInterpolator = std::make_shared<BinarySetting<SettingsEnums::FreeCameraInterpolationTypesEnum>>
		(
			SettingsEnums::FreeCameraInterpolationTypesEnum::Linear,
			[](SettingsEnums::FreeCameraInterpolationTypesEnum in) { return true; },
			nameof(freeCameraAnchorPositionToObjectPositionTranslationInterpolator)
		);

	std::shared_ptr<BinarySetting<float>> freeCameraAnchorPositionToObjectPositionTranslationInterpolatorLinearFactor = std::make_shared<BinarySetting<float>>
		(
			0.06f,
			[](float in) { return in > 0.f && in <= 1.f; },
			nameof(freeCameraAnchorPositionToObjectPositionTranslationInterpolatorLinearFactor)
		);


	std::shared_ptr<BinarySetting<bool>> freeCameraAnchorPositionToObjectRotation = std::make_shared<BinarySetting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(freeCameraAnchorPositionToObjectRotation)
		);

	std::shared_ptr<BinarySetting<bool>> freeCameraAnchorRotationToObjectPosition = std::make_shared<BinarySetting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(freeCameraAnchorRotationToObjectPosition)
		);

	std::shared_ptr<BinarySetting<SettingsEnums::FreeCameraObjectTrackEnum>> freeCameraAnchorRotationToObjectPositionObjectToTrackComboGroup = std::make_shared<BinarySetting<SettingsEnums::FreeCameraObjectTrackEnum>>
		(
			SettingsEnums::FreeCameraObjectTrackEnum::Player,
			[](SettingsEnums::FreeCameraObjectTrackEnum in) { return true; },
			nameof(freeCameraAnchorRotationToObjectPositionObjectToTrackComboGroup)
		);

	std::shared_ptr<BinarySetting<uint32_t>> freeCameraAnchorRotationToObjectPositionObjectToTrackCustomObjectDatum = std::make_shared<BinarySetting<uint32_t>>
		(
			0xDEADBEEF,
			[](uint32_t in) { return true; },
			nameof(freeCameraAnchorRotationToObjectPositionObjectToTrackCustomObjectDatum)
		);

	std::shared_ptr<BinarySetting<SimpleMath::Vector3>> freeCameraAnchorRotationToObjectPositionObjectToTrackManualPositionVec3 = std::make_shared<BinarySetting<SimpleMath::Vector3>>
		(
			SimpleMath::Vector3{ 0.f, 0.f, 0.f },
			[](SimpleMath::Vector3 in) { return true; },
			nameof(freeCameraAnchorRotationToObjectPositionObjectToTrackManualPositionVec3)
		);
	std::shared_ptr<BinarySetting<SettingsEnums::FreeCameraInterpolationTypesEnum>> freeCameraAnchorRotationToObjectPositionRotationInterpolator = std::make_shared<BinarySetting<SettingsEnums::FreeCameraInterpolationTypesEnum>>
		(
			SettingsEnums::FreeCameraInterpolationTypesEnum::Linear,
			[](SettingsEnums::FreeCameraInterpolationTypesEnum in) { return true; },
			nameof(freeCameraAnchorRotationToObjectPositionRotationInterpolator)
		);

	std::shared_ptr<BinarySetting<float>> freeCameraAnchorRotationToObjectPositionRotationInterpolatorLinearFactor = std::make_shared<BinarySetting<float>>
		(
			0.06f,
			[](float in) { return in > 0.f && in <= 1.f; },
			nameof(freeCameraAnchorRotationToObjectPositionRotationInterpolatorLinearFactor)
		);

	std::shared_ptr<BinarySetting<bool>> freeCameraAnchorRotationToObjectFacing = std::make_shared<BinarySetting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(freeCameraAnchorRotationToObjectFacing)
		);

	std::shared_ptr<BinarySetting<SettingsEnums::FreeCameraObjectTrackEnum>> freeCameraAnchorRotationToObjectFacingObjectToTrackComboGroup = std::make_shared<BinarySetting<SettingsEnums::FreeCameraObjectTrackEnum>>
		(
			SettingsEnums::FreeCameraObjectTrackEnum::Player,
			[](SettingsEnums::FreeCameraObjectTrackEnum in) { return true; },
			nameof(freeCameraAnchorRotationToObjectFacingObjectToTrackComboGroup)
		);

	std::shared_ptr<BinarySetting<uint32_t>> freeCameraAnchorRotationToObjectFacingObjectToTrackCustomObjectDatum = std::make_shared<BinarySetting<uint32_t>>
		(
			0xDEADBEEF,
			[](uint32_t in) { return true; },
			nameof(freeCameraAnchorRotationToObjectFacingObjectToTrackCustomObjectDatum)
		);

	std::shared_ptr<BinarySetting<SettingsEnums::FreeCameraInterpolationTypesEnum>> freeCameraAnchorRotationToObjectFacingRotationInterpolator = std::make_shared<BinarySetting<SettingsEnums::FreeCameraInterpolationTypesEnum>>
		(
			SettingsEnums::FreeCameraInterpolationTypesEnum::Linear,
			[](SettingsEnums::FreeCameraInterpolationTypesEnum in) { return true; },
			nameof(freeCameraAnchorRotationToObjectFacingRotationInterpolator)
		);

	std::shared_ptr<BinarySetting<float>> freeCameraAnchorRotationToObjectFacingRotationInterpolatorLinearFactor = std::make_shared<BinarySetting<float>>
		(
			0.06f,
			[](float in) { return in > 0.f && in <= 1.f; },
			nameof(freeCameraAnchorRotationToObjectFacingRotationInterpolatorLinearFactor)
		);

	std::shared_ptr<BinarySetting<bool>> freeCameraAnchorFOVToObjectDistance = std::make_shared<BinarySetting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(freeCameraAnchorFOVToObjectDistance)
		);


	std::shared_ptr<BinarySetting<SettingsEnums::FreeCameraObjectTrackEnumPlusAbsolute>> freeCameraAnchorFOVToObjectDistanceObjectToTrackComboGroup = std::make_shared<BinarySetting<SettingsEnums::FreeCameraObjectTrackEnumPlusAbsolute>>
		(
			SettingsEnums::FreeCameraObjectTrackEnumPlusAbsolute::Player,
			[](SettingsEnums::FreeCameraObjectTrackEnumPlusAbsolute in) { return true; },
			nameof(freeCameraAnchorFOVToObjectDistanceObjectToTrackComboGroup)
		);

	std::shared_ptr<BinarySetting<uint32_t>> freeCameraAnchorFOVToObjectDistanceObjectToTrackCustomObjectDatum = std::make_shared<BinarySetting<uint32_t>>
		(
			0xDEADBEEF,
			[](uint32_t in) { return true; },
			nameof(freeCameraAnchorFOVToObjectDistanceObjectToTrackCustomObjectDatum)
		);

	std::shared_ptr<BinarySetting<SimpleMath::Vector3>> freeCameraAnchorFOVToObjectDistanceObjectToTrackManualPositionVec3 = std::make_shared<BinarySetting<SimpleMath::Vector3>>
		(
			SimpleMath::Vector3{ 0.f, 0.f, 0.f },
			[](SimpleMath::Vector3 in) { return true; },
			nameof(freeCameraAnchorFOVToObjectDistanceObjectToTrackManualPositionVec3)
		);

	std::shared_ptr<BinarySetting<SettingsEnums::FreeCameraInterpolationTypesEnum>> freeCameraAnchorFOVToObjectDistanceFOVInterpolator = std::make_shared<BinarySetting<SettingsEnums::FreeCameraInterpolationTypesEnum>>
		(
			SettingsEnums::FreeCameraInterpolationTypesEnum::Linear,
			[](SettingsEnums::FreeCameraInterpolationTypesEnum in) { return true; },
			nameof(freeCameraAnchorFOVToObjectDistanceFOVInterpolator)
		);

	std::shared_ptr<BinarySetting<float>> freeCameraAnchorFOVToObjectDistanceFOVInterpolatorLinearFactor = std::make_shared<BinarySetting<float>>
		(
			0.06f,
			[](float in) { return in > 0.f && in <= 1.f; },
			nameof(freeCameraAnchorFOVToObjectDistanceFOVInterpolatorLinearFactor)
		);

	std::shared_ptr<BinarySetting<bool>> freeCameraUserInputCameraRotationScalesToFOV = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(freeCameraRotationSensivitiyScalesToFOV)
		);

	std::shared_ptr<BinarySetting<bool>> freeCameraUserInputCameraNonLinearFOVAtMinimum = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(freeCameraUserInputCameraNonLinearFOVAtMinimum)
		);

	std::shared_ptr<BinarySetting<bool>> freeCameraUserInputCameraNonLinearFOVAtMaximum = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(freeCameraUserInputCameraNonLinearFOVAtMaximum)
		);

	std::shared_ptr<BinarySetting<int>> switchBSPIndex = std::make_shared<BinarySetting<int>>
		(
			0,
			[](int in) { return true; },
			nameof(switchBSPIndex)
		);

	std::shared_ptr<BinarySetting<int>> switchBSPSetLoadSet = std::make_shared<BinarySetting<int>>
		(
			0,
			[](int in) { return true; },
			nameof(switchBSPSetLoadSet)
		);

	std::shared_ptr<BinarySetting<int>> switchBSPSetLoadIndex = std::make_shared<BinarySetting<int>>
		(
			0,
			[](int in) { return true; },
			nameof(switchBSPSetLoadIndex)
		);

	std::shared_ptr<BinarySetting<int>> switchBSPSetUnloadIndex = std::make_shared<BinarySetting<int>>
		(
			0,
			[](int in) { return true; },
			nameof(switchBSPSetUnloadIndex)
		);


	std::shared_ptr<BinarySetting<bool>> OBSBypassToggle = std::make_shared<BinarySetting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(OBSBypassToggle)
		);

	std::shared_ptr<BinarySetting<SimpleMath::Vector2>> setPlayerHealthVec2 = std::make_shared<BinarySetting<SimpleMath::Vector2>>
		(
			SimpleMath::Vector2(1.f, 1.f), // full health, full shields
			[](SimpleMath::Vector2 in) { return (in.x >= 0.f) && (in.x <= 1.f) && (in.y >= 0.f) && (in.y <= 1.f); }, // 0 to 1 inclusive
			nameof(setPlayerHealthVec2)
		);

	std::shared_ptr<BinarySetting<bool>> carrierBumpAnalyserToggle = std::make_shared<BinarySetting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(carrierBumpAnalyserToggle)
		);

	std::shared_ptr<BinarySetting<bool>> waypoint3DToggle = std::make_shared<BinarySetting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(waypoint3DToggle)
		);

	std::shared_ptr<BinarySetting<bool>> waypoint3DClampToggle = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(waypoint3DClampToggle)
		);


	std::shared_ptr<BinarySetting<float>> waypoint3DGlobalSpriteScale = std::make_shared<BinarySetting<float>>
		(
			1.f,
			[](float in) { return in > 0.f; },
			nameof(waypoint3DGlobalSpriteScale)
		);

	std::shared_ptr<BinarySetting<float>> waypoint3DGlobalLabelScale = std::make_shared<BinarySetting<float>>
		(
			16.f,
			[](float in) { return in > 6.f; },
			nameof(waypoint3DGlobalLabelScale)
		);

	std::shared_ptr<BinarySetting<float>> waypoint3DGlobalDistanceScale = std::make_shared<BinarySetting<float>>
		(
			16.f,
			[](float in) { return in > 6.f; },
			nameof(waypoint3DGlobalDistanceScale)
		);

	std::shared_ptr<BinarySetting<int>> waypoint3DGlobalDistancePrecision = std::make_shared<BinarySetting<int>>
		(
			3,
			[](int in) { return in > 0; },
			nameof(waypoint3DGlobalDistancePrecision)
		);


	std::shared_ptr<BinarySetting<SimpleMath::Vector4>> waypoint3DGlobalSpriteColor = std::make_shared<BinarySetting<SimpleMath::Vector4>>
		(
			SimpleMath::Vector4{ 0.f, 1.f, 0.f, 1.f }, // green
			[](SimpleMath::Vector4 in) { return true; },
			nameof(waypoint3DGlobalSpriteColor)
		);

	std::shared_ptr<BinarySetting<SimpleMath::Vector4>> waypoint3DGlobalLabelColor = std::make_shared<BinarySetting<SimpleMath::Vector4>>
		(
			SimpleMath::Vector4{ 0.f, 1.f, 0.f, 1.f }, // green
			[](SimpleMath::Vector4 in) { return true; },
			nameof(waypoint3DGlobalLabelColor)
		);

	std::shared_ptr<BinarySetting<SimpleMath::Vector4>> waypoint3DGlobalDistanceColor = std::make_shared<BinarySetting<SimpleMath::Vector4>>
		(
			SimpleMath::Vector4{ 0.f, 1.f, 0.f, 1.f }, // green
			[](SimpleMath::Vector4 in) { return true; },
			nameof(waypoint3DGlobalDistanceColor)
		);

	std::shared_ptr<UnarySetting<WaypointList>> waypoint3DList = std::make_shared<UnarySetting<WaypointList>>
		(
			WaypointList(), // default constructed
			nameof(waypoint3DList)
		);

	std::shared_ptr<BinarySetting<bool>> triggerOverlayToggle = std::make_shared<BinarySetting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(triggerOverlayToggle)
		);

	std::shared_ptr<BinarySetting<bool>> triggerOverlayFilterToggle = std::make_shared<BinarySetting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(triggerOverlayFilterToggle)
		);

	std::shared_ptr<UnarySetting<int>> triggerOverlayFilterCountLabel = std::make_shared<UnarySetting<int>>
		(
			0,
			nameof(triggerOverlayFilterCountLabel)
		);

	std::shared_ptr<BinarySetting<bool>> triggerOverlayFilterExactMatch = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(triggerOverlayFilterExactMatch)
		);

	std::shared_ptr<BinarySetting<std::string>> triggerOverlayFilterString = std::make_shared<BinarySetting<std::string>>
		(
			"",
			[](std::string in) { return true; },
			nameof(triggerOverlayFilterString)
		);




	std::shared_ptr<BinarySetting<SettingsEnums::TriggerRenderStyle>> triggerOverlayRenderStyle = std::make_shared<BinarySetting<SettingsEnums::TriggerRenderStyle>>
		(
			SettingsEnums::TriggerRenderStyle::SolidAndWireframe,
			[](SettingsEnums::TriggerRenderStyle in) { return true; },
			nameof(triggerOverlayRenderStyle)
		);



	std::shared_ptr<BinarySetting<SettingsEnums::TriggerInteriorStyle>> triggerOverlayInteriorStyle = std::make_shared<BinarySetting<SettingsEnums::TriggerInteriorStyle>>
		(
			SettingsEnums::TriggerInteriorStyle::Normal,
			[](SettingsEnums::TriggerInteriorStyle in) { return true; },
			nameof(triggerOverlayInteriorStyle)
		);

	std::shared_ptr<BinarySetting<SettingsEnums::TriggerLabelStyle>> triggerOverlayLabelStyle = std::make_shared<BinarySetting<SettingsEnums::TriggerLabelStyle>>
		(
			SettingsEnums::TriggerLabelStyle::Center,
			[](SettingsEnums::TriggerLabelStyle in) { return true; },
			nameof(triggerOverlayLabelStyle)
		);



	std::shared_ptr<BinarySetting<float>> triggerOverlayLabelScale = std::make_shared<BinarySetting<float>>
		(
			16.f,
			[](float in) { return in > 6.f; },
			nameof(triggerOverlayLabelScale)
		);

	std::shared_ptr<BinarySetting<SimpleMath::Vector4>> triggerOverlayNormalColor = std::make_shared<BinarySetting<SimpleMath::Vector4>>
		(
			SimpleMath::Vector4{1.0f, 0.0f, 0.0f, 0.25f}, // red
			[](SimpleMath::Vector4 in) { return  in.w >= 0.05f; },
			nameof(triggerOverlayNormalColor)
		);

	std::shared_ptr<BinarySetting<SimpleMath::Vector4>> triggerOverlayBSPColor = std::make_shared<BinarySetting<SimpleMath::Vector4>>
		(
			SimpleMath::Vector4{0.0f, 0.0f, 1.0f, 0.25f}, // blue
			[](SimpleMath::Vector4 in) { return  in.w >= 0.05f; },
			nameof(triggerOverlayBSPColor)
		);

	std::shared_ptr<BinarySetting<SimpleMath::Vector4>> triggerOverlaySectorColor = std::make_shared<BinarySetting<SimpleMath::Vector4>>
		(
			SimpleMath::Vector4{1.0f, 0.0f, 1.0f, 0.25f}, // purple
			[](SimpleMath::Vector4 in) { return  in.w >= 0.05f; },
			nameof(triggerOverlaySectorColor)
		);

	std::shared_ptr<BinarySetting<float>> triggerOverlayAlpha = std::make_shared<BinarySetting<float>>
		(
			0.4f,
			[](float in) { return in >= 0.05f && in <= 1.f; },
			nameof(triggerOverlayAlpha)
		);

	std::shared_ptr<BinarySetting<float>> triggerOverlayWireframeAlpha = std::make_shared<BinarySetting<float>>
		(
			1.0f,
			[](float in) { return in >= 0.05f && in <= 1.f; },
			nameof(triggerOverlayWireframeAlpha)
		);

	std::shared_ptr<BinarySetting<bool>> triggerOverlayCheckHitToggle = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(triggerOverlayCheckHitToggle)
		);

	std::shared_ptr<BinarySetting<bool>> triggerOverlayCheckMissToggle = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(triggerOverlayCheckMissToggle)
		);

	std::shared_ptr<BinarySetting<int>> triggerOverlayCheckHitFalloff = std::make_shared<BinarySetting<int>>
		(
			60,
			[](int in) { return in >= 0; },
			nameof(triggerOverlayCheckHitFalloff)
		);

	std::shared_ptr<BinarySetting<int>> triggerOverlayCheckMissFalloff = std::make_shared<BinarySetting<int>>
		(
			15,
			[](int in) { return in >= 0; },
			nameof(triggerOverlayCheckMissFalloff)
		);



	std::shared_ptr<BinarySetting<SimpleMath::Vector4>> triggerOverlayCheckHitColor = std::make_shared<BinarySetting<SimpleMath::Vector4>>
		(
			SimpleMath::Vector4{0.0f, 1.0f, 0.0f, 1.f}, // green
			[](SimpleMath::Vector4 in) { return  in.w >= 0.05f; },
			nameof(triggerOverlayCheckHitColor)
		);

	std::shared_ptr<BinarySetting<SimpleMath::Vector4>> triggerOverlayCheckMissColor = std::make_shared<BinarySetting<SimpleMath::Vector4>>
		(
			SimpleMath::Vector4{1.0f, 1.0f, 0.0f, 1.0f}, // yellow
			[](SimpleMath::Vector4 in) { return  in.w >= 0.05f; }, 
			nameof(triggerOverlayCheckMissColor)
		);

	std::shared_ptr<BinarySetting<bool>> triggerOverlayMessageOnEnter = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(triggerOverlayMessageOnEnter)
		);

	std::shared_ptr<BinarySetting<bool>> triggerOverlayMessageOnExit = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(triggerOverlayMessageOnExit)
		);

	std::shared_ptr<BinarySetting<bool>> triggerOverlayMessageOnCheckHit = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(triggerOverlayMessageOnCheckHit)
		);

	std::shared_ptr<BinarySetting<bool>> triggerOverlayMessageOnCheckMiss = std::make_shared<BinarySetting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(triggerOverlayMessageOnCheckMiss)
		);

	std::shared_ptr<BinarySetting<bool>> triggerOverlayPositionToggle = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(triggerOverlayPositionToggle)
		);



	std::shared_ptr<BinarySetting<SimpleMath::Vector4>> triggerOverlayPositionColor = std::make_shared<BinarySetting<SimpleMath::Vector4>>
		(
			SimpleMath::Vector4{0.0f, 1.0f, 0.0f, 1.f}, // green
			[](SimpleMath::Vector4 in) { return in.w == 1.f; },
			nameof(triggerOverlayPositionColor)
		);

	std::shared_ptr<BinarySetting<float>> triggerOverlayPositionScale = std::make_shared<BinarySetting<float>>
		(
			0.1f,
			[](float in) { return in > 0.f; },
			nameof(triggerOverlayPositionScale)
		);

	std::shared_ptr<BinarySetting<bool>> triggerOverlayPositionWireframe = std::make_shared<BinarySetting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(triggerOverlayPositionWireframe)
		);

	std::shared_ptr<BinarySetting<bool>> shieldInputPrinterToggle = std::make_shared<BinarySetting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(shieldInputPrinterToggle)
		);


	std::shared_ptr<BinarySetting<bool>> softCeilingOverlayToggle = std::make_shared<BinarySetting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(softCeilingOverlayToggle)
		);

	std::shared_ptr<BinarySetting<SettingsEnums::SoftCeilingRenderTypes>> softCeilingOverlayRenderTypes = std::make_shared<BinarySetting<SettingsEnums::SoftCeilingRenderTypes>>
		(
			SettingsEnums::SoftCeilingRenderTypes::BipedsOrVehicles,
			[](SettingsEnums::SoftCeilingRenderTypes in) { return true; },
			nameof(softCeilingOverlayRenderTypes)
		);

	std::shared_ptr<BinarySetting<SettingsEnums::SoftCeilingRenderDirection>> softCeilingOverlayRenderDirection = std::make_shared<BinarySetting<SettingsEnums::SoftCeilingRenderDirection>>
		(
			SettingsEnums::SoftCeilingRenderDirection::Both,
			[](SettingsEnums::SoftCeilingRenderDirection in) { return true; },
			nameof(softCeilingOverlayRenderDirection)
		);

	std::shared_ptr<BinarySetting<SimpleMath::Vector4>> softCeilingOverlayColorAccel = std::make_shared<BinarySetting<SimpleMath::Vector4>>
		(
			SimpleMath::Vector4{0.0f, 1.0f, 0.0f, 1.f}, // green
			[](SimpleMath::Vector4 in) { return in.w == 1.f; },
			nameof(softCeilingOverlayColorAccel)
		);

	std::shared_ptr<BinarySetting<SimpleMath::Vector4>> softCeilingOverlayColorSlippy = std::make_shared<BinarySetting<SimpleMath::Vector4>>
		(
			SimpleMath::Vector4{1.0f, 1.0f, 0.0f, 1.f}, // yellow
			[](SimpleMath::Vector4 in) { return in.w == 1.f; },
			nameof(softCeilingOverlayColorSlippy)
		);

	std::shared_ptr<BinarySetting<SimpleMath::Vector4>> softCeilingOverlayColorKill = std::make_shared<BinarySetting<SimpleMath::Vector4>>
		(
			SimpleMath::Vector4{1.0f, 0.0f, 0.0f, 1.f}, // red
			[](SimpleMath::Vector4 in) { return in.w == 1.f; },
			nameof(softCeilingOverlayColorKill)
		);

	std::shared_ptr<BinarySetting<float>> softCeilingOverlaySolidTransparency = std::make_shared<BinarySetting<float>>
		(
			0.3f,
			[](float in) { return true; },
			nameof(softCeilingOverlaySolidTransparency)
		);

	std::shared_ptr<BinarySetting<float>> softCeilingOverlayWireframeTransparency = std::make_shared<BinarySetting<float>>
		(
			0.6f,
			[](float in) { return true; },
			nameof(softCeilingOverlayWireframeTransparency)
		);

	std::shared_ptr<BinarySetting<bool>> disableBarriersToggle = std::make_shared<BinarySetting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(disableBarriersToggle)
		);

	std::shared_ptr<BinarySetting<float>> renderDistance3D = std::make_shared<BinarySetting<float>>
		(
			1000.f,
			[](float in) { return in >= 0.1f; }, 
			nameof(renderDistance3D)
		); 

	std::shared_ptr<BinarySetting<bool>> abilityMeterOverlayToggle = std::make_shared<BinarySetting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(abilityMeterOverlayToggle)
		);

	
		std::shared_ptr<BinarySetting<bool>> abilityMeterHeroAssistToggle = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(abilityMeterHeroAssistToggle)
		);

	std::shared_ptr<BinarySetting<SettingsEnums::ScreenAnchorEnum>> abilityMeterAbilityAnchorCorner = std::make_shared<BinarySetting<SettingsEnums::ScreenAnchorEnum>>
		(
			SettingsEnums::ScreenAnchorEnum::BottomRight,
			[](SettingsEnums::ScreenAnchorEnum in) { return true; },
			nameof(abilityMeterAbilityAnchorCorner)
		);

	std::shared_ptr<BinarySetting<SimpleMath::Vector2>> abilityMeterAbilityScreenOffset = std::make_shared<BinarySetting<SimpleMath::Vector2>>
		(
			SimpleMath::Vector2{800, 800},
			[](SimpleMath::Vector2 in) { return in.x >= 0 && in.y >= 0; }, // no negative offsets
			nameof(abilityMeterAbilityScreenOffset)
		);

	std::shared_ptr<BinarySetting<SimpleMath::Vector2>> abilityMeterAbilitySize = std::make_shared<BinarySetting<SimpleMath::Vector2>>
		(
			SimpleMath::Vector2{400, 40},
			[](SimpleMath::Vector2 in) { return in.x >= 0 && in.y >= 0; }, // no negative offsets
			nameof(abilityMeterAbilitySize)
		);

	std::shared_ptr<BinarySetting<SimpleMath::Vector4>> abilityMeterAbilityBackgroundColor = std::make_shared<BinarySetting<SimpleMath::Vector4>>
		(
			SimpleMath::Vector4{0.0f, 0.0f, 0.0f, 0.f}, // transparent
			[](SimpleMath::Vector4 in) { return in.x >= 0 && in.y >= 0 && in.z >= 0 && in.w >= 0 && in.x <= 1 && in.y <= 1 && in.z <= 1 && in.w <= 1; }, // range 0.f ... 1.f 
			nameof(abilityMeterAbilityBackgroundColor)
		);

	std::shared_ptr<BinarySetting<SimpleMath::Vector4>> abilityMeterAbilityForegroundColor = std::make_shared<BinarySetting<SimpleMath::Vector4>>
		(
			SimpleMath::Vector4{0.3f, 0.3f, 1.0f, 0.7f}, // blue 
			[](SimpleMath::Vector4 in) { return in.x >= 0 && in.y >= 0 && in.z >= 0 && in.w >= 0 && in.x <= 1 && in.y <= 1 && in.z <= 1 && in.w <= 1; }, // range 0.f ... 1.f 
			nameof(abilityMeterAbilityForegroundColor)
		);

	std::shared_ptr<BinarySetting<SimpleMath::Vector4>> abilityMeterAbilityHighlightColor = std::make_shared<BinarySetting<SimpleMath::Vector4>>
		(
			SimpleMath::Vector4{0.4f, 0.7f, 1.0f, 1.f}, // blue but more opaque
			[](SimpleMath::Vector4 in) { return in.x >= 0 && in.y >= 0 && in.z >= 0 && in.w >= 0 && in.x <= 1 && in.y <= 1 && in.z <= 1 && in.w <= 1; }, // range 0.f ... 1.f 
			nameof(abilityMeterAbilityHighlightColor)
		);

	std::shared_ptr<BinarySetting<bool>> abilityMeterCooldownToggle = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(abilityMeterCooldownToggle)
		);

	std::shared_ptr<BinarySetting<SettingsEnums::ScreenAnchorEnum>> abilityMeterCooldownAnchorCorner = std::make_shared<BinarySetting<SettingsEnums::ScreenAnchorEnum>>
		(
			SettingsEnums::ScreenAnchorEnum::BottomRight,
			[](SettingsEnums::ScreenAnchorEnum in) { return true; },
			nameof(abilityMeterCooldownAnchorCorner)
		);

	std::shared_ptr<BinarySetting<SimpleMath::Vector2>> abilityMeterCooldownScreenOffset = std::make_shared<BinarySetting<SimpleMath::Vector2>>
		(
			SimpleMath::Vector2{800, 750},
			[](SimpleMath::Vector2 in) { return in.x >= 0 && in.y >= 0; }, // no negative offsets
			nameof(abilityMeterCooldownScreenOffset)
		);

	std::shared_ptr<BinarySetting<SimpleMath::Vector2>> abilityMeterCooldownSize = std::make_shared<BinarySetting<SimpleMath::Vector2>>
		(
			SimpleMath::Vector2{400, 20},
			[](SimpleMath::Vector2 in) { return in.x >= 0 && in.y >= 0; }, // no negative offsets
			nameof(abilityMeterCooldownSize)
		);


	std::shared_ptr<BinarySetting<SimpleMath::Vector4>> abilityMeterCooldownBackgroundColor = std::make_shared<BinarySetting<SimpleMath::Vector4>>
		(
			SimpleMath::Vector4{0.0f, 0.0f, 0.0f, 0.f}, // transparent
			[](SimpleMath::Vector4 in) { return in.x >= 0 && in.y >= 0 && in.z >= 0 && in.w >= 0 && in.x <= 1 && in.y <= 1 && in.z <= 1 && in.w <= 1; }, // range 0.f ... 1.f 
			nameof(abilityMeterCooldownBackgroundColor)
		);

	std::shared_ptr<BinarySetting<SimpleMath::Vector4>> abilityMeterCooldownForegroundColor = std::make_shared<BinarySetting<SimpleMath::Vector4>>
		(
			SimpleMath::Vector4{0.3f, 0.3f, 1.0f, 0.7f}, // blue 
			[](SimpleMath::Vector4 in) { return in.x >= 0 && in.y >= 0 && in.z >= 0 && in.w >= 0 && in.x <= 1 && in.y <= 1 && in.z <= 1 && in.w <= 1; }, // range 0.f ... 1.f 
			nameof(abilityMeterCooldownForegroundColor)
		);

	std::shared_ptr<BinarySetting<SimpleMath::Vector4>> abilityMeterCooldownHighlightColor = std::make_shared<BinarySetting<SimpleMath::Vector4>>
		(
			SimpleMath::Vector4{0.4f, 0.7f, 1.0f, 1.f}, // blue but lighter and more opaque
			[](SimpleMath::Vector4 in) { return in.x >= 0 && in.y >= 0 && in.z >= 0 && in.w >= 0 && in.x <= 1 && in.y <= 1 && in.z <= 1 && in.w <= 1; }, // range 0.f ... 1.f 
			nameof(abilityMeterCooldownHighlightColor)
		);



	std::shared_ptr<BinarySetting<float>> soundClassGainDialog = std::make_shared<BinarySetting<float>>
		(
			1.f,
			[](float in) { return in >= 0.f && in <= 1.f; },
			nameof(soundClassGainDialog)
		);

	std::shared_ptr<BinarySetting<float>> soundClassGainAmbience = std::make_shared<BinarySetting<float>>
		(
			1.f,
			[](float in) { return in >= 0.f && in <= 1.f; },
			nameof(soundClassGainAmbience)
		);

	std::shared_ptr<BinarySetting<float>> soundClassGainWeapons = std::make_shared<BinarySetting<float>>
		(
			1.f,
			[](float in) { return in >= 0.f && in <= 1.f; },
			nameof(soundClassGainWeapons)
		);

	std::shared_ptr<BinarySetting<float>> soundClassGainVehicles = std::make_shared<BinarySetting<float>>
		(
			1.f,
			[](float in) { return in >= 0.f && in <= 1.f; },
			nameof(soundClassGainVehicles)
		);

	std::shared_ptr<BinarySetting<float>> soundClassGainMusic = std::make_shared<BinarySetting<float>>
		(
			1.f,
			[](float in) { return in >= 0.f && in <= 1.f; },
			nameof(soundClassGainMusic)
		);

	std::shared_ptr<BinarySetting<float>> soundClassGainOther = std::make_shared<BinarySetting<float>>
		(
			1.f,
			[](float in) { return in >= 0.f && in <= 1.f; },
			nameof(soundClassGainOther)
		);

	std::shared_ptr<BinarySetting<bool>> soundClassGainAdjusterToggle = std::make_shared<BinarySetting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(soundClassGainAdjusterToggle)
		);

	std::shared_ptr<BinarySetting<bool>> changeOOBBackgroundToggle = std::make_shared<BinarySetting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(changeOOBBackgroundToggle)
		);

	std::shared_ptr<BinarySetting<SimpleMath::Vector4>> changeOOBBackgroundColor = std::make_shared<BinarySetting<SimpleMath::Vector4>>
		(
			SimpleMath::Vector4{0.235f, 0.106f, 0.553f, 1.f}, // default value used by halo 3
			[](SimpleMath::Vector4 in) { return in.x >= 0 && in.y >= 0 && in.z >= 0 && in.w >= 0 && in.x <= 1 && in.y <= 1 && in.z <= 1 && in.w <= 1; }, // range 0.f ... 1.f 
			nameof(changeOOBBackgroundColor)
		);


	std::shared_ptr<BinarySetting<bool>> disableFogToggle = std::make_shared<BinarySetting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(disableFogToggle)
		);

	

		std::shared_ptr<BinarySetting<bool>> sensDriftOverlayToggle = std::make_shared<BinarySetting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(sensDriftOverlayToggle)
		);

	std::shared_ptr<BinarySetting<bool>> sensOverDotFrameToggle = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(sensOverDotFrameToggle)
		);

	std::shared_ptr<BinarySetting<bool>> sensMessageOnOverDotToggle = std::make_shared<BinarySetting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(sensMessageOnOverDotToggle)
		);

	std::shared_ptr<BinarySetting<bool>> sensSoundOnOverDotToggle = std::make_shared<BinarySetting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(sensSoundOnOverDotToggle)
		);

	std::shared_ptr<BinarySetting<bool>> sensSubpixelDriftToggle = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(sensSubpixelDriftFrameToggle)
		);

	std::shared_ptr<BinarySetting<bool>> sensMessageOnSubpixelDriftToggle = std::make_shared<BinarySetting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(sensMessageOnSubpixelDriftToggle)
		);

	std::shared_ptr<BinarySetting<bool>> sensSoundOnSubpixelDriftToggle = std::make_shared<BinarySetting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(sensSoundOnSubpixelDriftToggle)
		);

	std::shared_ptr<BinarySetting<bool>> sensResetCountOnRevertToggle = std::make_shared<BinarySetting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(sensResetCountOnRevertToggle)
		);


	std::shared_ptr<BinarySetting<SettingsEnums::ScreenAnchorEnum>> sensAnchorCorner = std::make_shared<BinarySetting<SettingsEnums::ScreenAnchorEnum>>
		(
			SettingsEnums::ScreenAnchorEnum::BottomRight,
			[](SettingsEnums::ScreenAnchorEnum in) { return true; },
			nameof(sensAnchorCorner)
		);

	std::shared_ptr<BinarySetting<SimpleMath::Vector2>> sensScreenOffset = std::make_shared<BinarySetting<SimpleMath::Vector2>>
		(
			SimpleMath::Vector2{400, 400},
			[](SimpleMath::Vector2 in) { return in.x >= 0 && in.y >= 0; }, // no negative offsets
			nameof(sensScreenOffset)
		);

	std::shared_ptr<BinarySetting<float>> sensFontSize = std::make_shared<BinarySetting<float>>
		(
			16.f,
			[](int in) { return in > 6.f; },
			nameof(sensFontSize)
		);

	std::shared_ptr<BinarySetting<SimpleMath::Vector4>> sensFontColour = std::make_shared<BinarySetting<SimpleMath::Vector4>>
		(
			SimpleMath::Vector4{1.00f, 0.60f, 0.25f, 1.00f}, // a nice orange colour that matches the rest of the gui
			[](SimpleMath::Vector4 in) { return in.x >= 0 && in.y >= 0 && in.z >= 0 && in.w >= 0 && in.x <= 1 && in.y <= 1 && in.z <= 1 && in.w <= 1; }, // range 0.f ... 1.f 
			nameof(sensFontColour)
		);



	// settings that ought to be serialised/deserialised between HCM runs
	std::vector<std::shared_ptr<SerialisableSetting>> allSerialisableOptions
	{
		changeOOBBackgroundColor,
		soundClassGainDialog,
			soundClassGainAmbience,
			soundClassGainWeapons,
			soundClassGainVehicles,
			soundClassGainMusic,
			soundClassGainOther,



			sensOverDotFrameToggle,
			sensMessageOnOverDotToggle,
			sensSoundOnOverDotToggle,
			sensSubpixelDriftToggle,
			sensMessageOnSubpixelDriftToggle,
			sensSoundOnSubpixelDriftToggle,
			sensResetCountOnRevertToggle,
			sensAnchorCorner,
			sensScreenOffset,
			sensFontSize,
			sensFontColour,


			abilityMeterAbilityAnchorCorner,
			abilityMeterAbilityScreenOffset,
			abilityMeterAbilitySize,
			abilityMeterAbilityBackgroundColor,
			abilityMeterAbilityForegroundColor,
			abilityMeterAbilityHighlightColor,
			abilityMeterCooldownToggle,
			abilityMeterCooldownAnchorCorner,
			abilityMeterCooldownScreenOffset,
			abilityMeterCooldownSize,
			abilityMeterCooldownBackgroundColor,
			abilityMeterCooldownForegroundColor,
			abilityMeterCooldownHighlightColor,
		renderDistance3D,
		messagesFontSize,
			messagesFontColor,
		triggerOverlayFilterToggle,
		triggerOverlayFilterString,
		triggerOverlayRenderStyle,
		triggerOverlayInteriorStyle,
			triggerOverlayLabelStyle,
			triggerOverlayLabelScale,
		triggerOverlayNormalColor,
		triggerOverlayBSPColor,
			triggerOverlaySectorColor,
		triggerOverlayAlpha,
			triggerOverlayWireframeAlpha,
		triggerOverlayCheckHitToggle,
		triggerOverlayCheckMissToggle,
		triggerOverlayCheckHitFalloff,
		triggerOverlayCheckMissFalloff,
		triggerOverlayCheckHitColor,
		triggerOverlayCheckMissColor,
		triggerOverlayMessageOnEnter,
		triggerOverlayMessageOnExit,
		triggerOverlayMessageOnCheckHit,
		triggerOverlayMessageOnCheckMiss,
			triggerOverlayPositionToggle,
			triggerOverlayPositionColor,
			triggerOverlayPositionScale,
			triggerOverlayPositionWireframe,
			softCeilingOverlayRenderTypes,
			softCeilingOverlayColorAccel,
			softCeilingOverlayColorSlippy,
			softCeilingOverlayColorKill,
			softCeilingOverlaySolidTransparency,
			softCeilingOverlayWireframeTransparency,
		hideWatermarkHideMessages,
		advanceTicksCount,
		injectionIgnoresChecksum,
		injectCheckpointForcesRevert,
		injectCheckpointLevelCheck,
		injectCheckpointVersionCheck,
			injectCheckpointDifficultyCheck,
		autonameCheckpoints,
		dumpCheckpointForcesSave,
			injectCoreForcesRevert,
		injectCoreLevelCheck,
		injectCoreVersionCheck,
			injectCoreDifficultyCheck,
		autonameCoresaves,
		dumpCoreForcesSave,
		forceFutureCheckpointTick,
		invulnerabilityNPCToggle, 
		speedhackSetting, 
		GUIShowingFreesCursor, 
		GUIShowingBlocksInput, 
		GUIShowingPausesGame, 
		pauseAlsoBlocksInput,
		pauseAlsoFreesCursor,
		getObjectAddressDWORD,
				getTagAddressDWORD,
		forceTeleportApplyToPlayer,
		forceTeleportCustomObject,
		forceTeleportAbsoluteVec3,
		forceTeleportRelativeVec3,
		forceTeleportForward,
		forceTeleportForwardIgnoreZ,
		forceTeleportManual,
		forceLaunchApplyToPlayer,
		forceLaunchCustomObject,
		forceLaunchAbsoluteVec3,
		forceLaunchRelativeVec3,
		forceLaunchForward,
		forceLaunchForwardIgnoreZ,
		forceLaunchManual,
		display2DInfoShowGameTick,
		display2DInfoShowAggro,
		display2DInfoShowRNG,
		display2DInfoShowBSP,
			display2DInfoShowBSPSet,
		display2DInfoShowNextObjectDatum,
		display2DInfoTrackPlayer,
		display2DInfoShowPlayerViewAngle,
		display2DInfoShowPlayerPosition,
		display2DInfoShowPlayerVelocity,
		display2DInfoShowPlayerVelocityAbs,
		display2DInfoShowPlayerVelocityXY,
		display2DInfoShowPlayerVelocityXYZ,
		display2DInfoShowPlayerHealth,
		display2DInfoShowPlayerRechargeCooldown,
		display2DInfoShowPlayerVehicleHealth,
		display2DInfoTrackCustomObject,
		display2DInfoCustomObjectDatum,
		display2DInfoShowEntityObjectType,
		display2DInfoShowEntityTagName,
		display2DInfoShowEntityPosition,
		display2DInfoShowEntityVelocity,
		display2DInfoShowEntityVelocityAbs,
		display2DInfoShowEntityVelocityXY,
		display2DInfoShowEntityVelocityXYZ,
		display2DInfoShowEntityHealth,
		display2DInfoShowEntityRechargeCooldown,
		display2DInfoShowEntityVehicleHealth,
		display2DInfoAnchorCorner,
		display2DInfoScreenOffset,
		display2DInfoFontSize,
		display2DInfoFontColour,
		display2DInfoFloatPrecision,
		display2DInfoOutline,
		editPlayerViewAngleVec2,
		editPlayerViewAngleAdjustFactor,
		freeCameraThirdPersonRendering,
		freeCameraGameInputDisable,
		//freeCameraCameraInputDisable,
		freeCameraUserInputCameraTranslationSpeedChangeFactor,
		freeCameraUserInputCameraSetPositionVec3,
		freeCameraUserInputCameraSetRotationVec3,
		freeCameraUserInputCameraMaintainVelocity,
		freeCameraUserInputCameraSetVelocityVec3,
		freeCameraUserInputCameraBaseFOV,
		freeCameraUserInputCameraTranslationSpeed,
		freeCameraUserInputCameraTranslationInterpolator,
		freeCameraUserInputCameraTranslationInterpolatorLinearFactor,
		freeCameraUserInputCameraRotationSpeed,
		freeCameraUserInputCameraRotationInterpolator,
		freeCameraUserInputCameraRotationInterpolatorLinearFactor,
		freeCameraUserInputCameraFOVSpeed,
		freeCameraUserInputCameraFOVInterpolator,
		freeCameraUserInputCameraFOVInterpolatorLinearFactor,
		freeCameraTeleportToCameraSlightlyBehind,
		//freeCameraAnchorPositionToObjectPosition,
		freeCameraAnchorPositionToObjectPositionObjectToTrackComboGroup,
		freeCameraAnchorPositionToObjectPositionObjectToTrackCustomObjectDatum,
		freeCameraAnchorPositionToObjectPositionTranslationInterpolator,
		freeCameraAnchorPositionToObjectPositionTranslationInterpolatorLinearFactor,
		//freeCameraAnchorPositionToObjectRotation,
		//freeCameraAnchorRotationToObjectPosition,
		freeCameraAnchorRotationToObjectPositionObjectToTrackComboGroup,
		freeCameraAnchorRotationToObjectPositionObjectToTrackCustomObjectDatum,
		freeCameraAnchorRotationToObjectPositionObjectToTrackManualPositionVec3,
		freeCameraAnchorRotationToObjectPositionRotationInterpolator,
		freeCameraAnchorRotationToObjectPositionRotationInterpolatorLinearFactor,
		//freeCameraAnchorRotationToObjectFacing,
		freeCameraAnchorRotationToObjectFacingObjectToTrackComboGroup,
		freeCameraAnchorRotationToObjectFacingObjectToTrackCustomObjectDatum,
		freeCameraAnchorRotationToObjectFacingRotationInterpolator,
		freeCameraAnchorRotationToObjectFacingRotationInterpolatorLinearFactor,
		//freeCameraAnchorFOVToObjectDistance,
		freeCameraAnchorFOVToObjectDistanceObjectToTrackComboGroup,
		freeCameraAnchorFOVToObjectDistanceObjectToTrackCustomObjectDatum,
		freeCameraAnchorFOVToObjectDistanceObjectToTrackManualPositionVec3,
		freeCameraAnchorFOVToObjectDistanceFOVInterpolator,
		freeCameraAnchorFOVToObjectDistanceFOVInterpolatorLinearFactor,
		freeCameraUserInputCameraRotationScalesToFOV,
		freeCameraUserInputCameraNonLinearFOVAtMinimum,
		freeCameraUserInputCameraNonLinearFOVAtMaximum,
		switchBSPIndex,
		switchBSPSetLoadSet,
		switchBSPSetLoadIndex,
		switchBSPSetUnloadIndex,
		setPlayerHealthVec2,
		waypoint3DClampToggle,
		waypoint3DGlobalSpriteScale,
		waypoint3DGlobalLabelScale,
		waypoint3DGlobalDistanceScale,
		waypoint3DGlobalDistancePrecision,
		waypoint3DGlobalSpriteColor,
		waypoint3DGlobalLabelColor,
		waypoint3DGlobalDistanceColor,
		waypoint3DList,
		consoleCommandFontSize,
		consoleCommandFreeCursor,
		consoleCommandBlockInput,
		consoleCommandPauseGame,

	};


};

