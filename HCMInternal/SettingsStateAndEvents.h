#pragma once
#include "Setting.h"
#include "ISettingsSerialiser.h"
#include "GUIServiceInfo.h"

class SettingsStateAndEvents
{
private:
	std::shared_ptr<ISettingsSerialiser> mSerialiser;

public:
	SettingsStateAndEvents(std::shared_ptr<ISettingsSerialiser> serialiser)
		: mSerialiser(serialiser)
	{ 
		mSerialiser->deserialise(allSerialisableOptions); 
	
	}
	~SettingsStateAndEvents() {
		PLOG_DEBUG << "~SettingsStateAndEvents()";
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
	std::shared_ptr<ActionEvent> consoleCommandEvent = std::make_shared<ActionEvent>();
	std::shared_ptr<ActionEvent> getObjectAddressEvent = std::make_shared<ActionEvent>();
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


	// settings
	std::shared_ptr<Setting<bool>> GUIWindowOpen = std::make_shared<Setting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(GUIWindowOpen)
		);

	std::shared_ptr<Setting<bool>> GUIShowingFreesCursor = std::make_shared<Setting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(GUIShowingFreesCursor)
		);


	std::shared_ptr<Setting<bool>> GUIShowingBlocksInput = std::make_shared<Setting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(GUIShowingBlocksInput)
		);

	std::shared_ptr<Setting<bool>> GUIShowingPausesGame = std::make_shared<Setting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(GUIShowingPausesGame)
		);

	std::shared_ptr<Setting<bool>> togglePause = std::make_shared<Setting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(togglePause)
		);

	std::shared_ptr<Setting<bool>> pauseAlsoBlocksInput = std::make_shared<Setting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(pauseAlsoBlocksInput)
		);

	std::shared_ptr<Setting<bool>> pauseAlsoFreesCursor = std::make_shared<Setting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(pauseAlsoFreesCursor)
		);

	std::shared_ptr<Setting<int>> advanceTicksCount = std::make_shared<Setting<int>>
		(
			1,
			[](int in) { return in > 0; }, // must be positive
			nameof(advanceTicksCount)
		);

	std::shared_ptr<Setting<bool>> injectionIgnoresChecksum = std::make_shared<Setting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(injectionIgnoresChecksum)
		);

	std::shared_ptr<Setting<bool>> injectCheckpointForcesRevert = std::make_shared<Setting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(injectCheckpointForcesRevert)
		);

	std::shared_ptr<Setting<bool>> injectCheckpointLevelCheck = std::make_shared<Setting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(injectCheckpointLevelCheck)
		);

	std::shared_ptr<Setting<bool>> injectCheckpointVersionCheck = std::make_shared<Setting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(injectCheckpointVersionCheck)
		);

	std::shared_ptr<Setting<bool>> injectCheckpointDifficultyCheck = std::make_shared<Setting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(injectCheckpointDifficultyCheck)
		);

	std::shared_ptr<Setting<bool>> autonameCheckpoints = std::make_shared<Setting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(autonameCheckpoints)
		);

	std::shared_ptr<Setting<bool>> dumpCheckpointForcesSave = std::make_shared<Setting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(dumpCheckpointForcesSave)
		);

	std::shared_ptr<Setting<bool>> injectCoreForcesRevert = std::make_shared<Setting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(injectCoreForcesRevert)
		);

	std::shared_ptr<Setting<bool>> injectCoreLevelCheck = std::make_shared<Setting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(injectCoreLevelCheck)
		);

	std::shared_ptr<Setting<bool>> injectCoreVersionCheck = std::make_shared<Setting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(injectCoreVersionCheck)
		);

	std::shared_ptr<Setting<bool>> injectCoreDifficultyCheck = std::make_shared<Setting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(injectCoreDifficultyCheck)
		);

	std::shared_ptr<Setting<bool>> dumpCoreForcesSave = std::make_shared<Setting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(dumpCoreForcesSave)
		);

	std::shared_ptr<Setting<bool>> autonameCoresaves = std::make_shared<Setting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(autonameCoresaves)
		);

	std::shared_ptr<Setting<bool>> speedhackToggle = std::make_shared<Setting<bool>>
	(
		false,
		[](bool in) { return true; },
		nameof(speedhackToggle)
	);

	std::shared_ptr<Setting<double>> speedhackSetting = std::make_shared<Setting<double>>
	(
		10.f,
		[](float in) { return in > 0; }, // must be positive
		nameof(speedhackSetting)
	);


	std::shared_ptr<Setting<bool>> invulnerabilityToggle = std::make_shared<Setting<bool>>
	(
		false,
		[](bool in) { return true; },
		nameof(invulnerabilityToggle)
	);

	std::shared_ptr<Setting<bool>> invulnerabilityNPCToggle = std::make_shared<Setting<bool>>
	(
		false,
		[](bool in) { return true; },
		nameof(invulnerabilityNPCToggle)
	);

	std::shared_ptr<Setting<bool>> aiFreezeToggle = std::make_shared<Setting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(aiFreezeToggle)
		);

	std::shared_ptr<Setting<bool>> medusaToggle = std::make_shared<Setting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(medusaToggle)
		);

	std::shared_ptr<Setting<std::string>> consoleCommandString = std::make_shared<Setting<std::string>>
		(
			"game_revert",
			[](std::string in) { return true; },
			nameof(consoleCommandString)
		);


	std::shared_ptr<Setting<uint32_t>> getObjectAddressDWORD = std::make_shared<Setting<uint32_t>>
		(
			0xDEADBEEF,
			[](uint32_t in) { return true; },
			nameof(getObjectAddressDWORD)
		);

	std::shared_ptr<Setting<bool>> forceTeleportApplyToPlayer = std::make_shared<Setting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(forceTeleportApplyToPlayer)
		);

	std::shared_ptr<Setting<uint32_t>> forceTeleportCustomObject = std::make_shared<Setting<uint32_t>>
		(
			0xDEADBEEF,
			[](uint32_t in) { return true; },
			nameof(forceTeleportCustomObject)
		);


	std::shared_ptr<Setting<SimpleMath::Vector3>> forceTeleportAbsoluteVec3 = std::make_shared<Setting<SimpleMath::Vector3>>
		(
			SimpleMath::Vector3{ 0.f, 0.f, 0.f },
			[](SimpleMath::Vector3 in) { return true; },
			nameof(forceTeleportAbsoluteVec3)
		);

	std::shared_ptr<Setting<SimpleMath::Vector3>> forceTeleportRelativeVec3 = std::make_shared<Setting<SimpleMath::Vector3>>
		(
			SimpleMath::Vector3{ 5.f, 0.f, 0.f },
			[](SimpleMath::Vector3 in) { return true; },
			nameof(forceTeleportRelativeVec3)
		);

	std::shared_ptr<Setting<bool>> forceTeleportForward = std::make_shared<Setting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(forceTeleportForward)
		);



	std::shared_ptr<Setting<bool>> forceTeleportForwardIgnoreZ = std::make_shared<Setting<bool>> // ignore vertical component of players look angle
		(
			false,
			[](bool in) { return true; },
			nameof(forceTeleportForwardIgnoreZ)
		);



	std::shared_ptr<Setting<bool>> forceTeleportManual = std::make_shared<Setting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(forceTeleportManual)
		);

	std::shared_ptr<Setting<bool>> forceLaunchApplyToPlayer = std::make_shared<Setting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(forceLaunchApplyToPlayer)
		);

	std::shared_ptr<Setting<uint32_t>> forceLaunchCustomObject = std::make_shared<Setting<uint32_t>>
		(
			0xDEADB33F,
			[](uint32_t in) { return true; },
			nameof(forceLaunchCustomObject)
		);

	std::shared_ptr<Setting<SimpleMath::Vector3>> forceLaunchAbsoluteVec3 = std::make_shared<Setting<SimpleMath::Vector3>>
		(
			SimpleMath::Vector3{ 0.f, 0.f, 5.f },
			[](SimpleMath::Vector3 in) { return true; },
			nameof(forceLaunchAbsoluteVec3)
		);

	std::shared_ptr<Setting<SimpleMath::Vector3>> forceLaunchRelativeVec3 = std::make_shared<Setting<SimpleMath::Vector3>>
		(
			SimpleMath::Vector3{ 0.5f, 0.f, 0.f },
			[](SimpleMath::Vector3 in) { return true; },
			nameof(forceLaunchRelativeVec3)
		);


	std::shared_ptr<Setting<bool>> forceLaunchForward = std::make_shared<Setting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(forceLaunchForward)
		);

	std::shared_ptr<Setting<bool>> forceLaunchForwardIgnoreZ = std::make_shared<Setting<bool>> // ignore vertical component of players look angle
		(
			false,
			[](bool in) { return true; },
			nameof(forceLaunchForwardIgnoreZ)
		);

	std::shared_ptr<Setting<bool>> forceLaunchManual = std::make_shared<Setting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(forceLaunchManual)
		);



	std::shared_ptr<Setting<bool>> naturalCheckpointDisable = std::make_shared<Setting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(naturalCheckpointDisable)
		);

	std::shared_ptr<Setting<bool>> infiniteAmmoToggle = std::make_shared<Setting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(infiniteAmmoToggle)
		);

	std::shared_ptr<Setting<bool>> bottomlessClipToggle = std::make_shared<Setting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(bottomlessClipToggle)
		);

	std::shared_ptr<Setting<bool>> display2DInfoToggle = std::make_shared<Setting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(display2DInfoToggle)
		);

	std::shared_ptr<Setting<bool>> display2DInfoShowGameTick = std::make_shared<Setting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(display2DInfoShowGameTick)
		);

	std::shared_ptr<Setting<bool>> display2DInfoShowNextObjectDatum = std::make_shared<Setting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(display2DInfoShowNextObjectDatum)
		);

	std::shared_ptr<Setting<bool>> display2DInfoShowAggro = std::make_shared<Setting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(display2DInfoShowAggro)
		);

	std::shared_ptr<Setting<bool>> display2DInfoShowRNG = std::make_shared<Setting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(display2DInfoShowRNG)
		);

	std::shared_ptr<Setting<bool>> display2DInfoShowBSP = std::make_shared<Setting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(display2DInfoShowBSP)
		);

	std::shared_ptr<Setting<bool>> display2DInfoTrackPlayer = std::make_shared<Setting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(display2DInfoTrackPlayer)
		);

	std::shared_ptr<Setting<bool>> display2DInfoShowPlayerViewAngle= std::make_shared<Setting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(display2DInfoShowPlayerViewAngle)
		);





	std::shared_ptr<Setting<bool>> display2DInfoShowPlayerPosition = std::make_shared<Setting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(display2DInfoShowPlayerPosition)
		);

	std::shared_ptr<Setting<bool>> display2DInfoShowPlayerVelocity = std::make_shared<Setting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(display2DInfoShowPlayerVelocity)
		);

	std::shared_ptr<Setting<bool>> display2DInfoShowPlayerVelocityAbs = std::make_shared<Setting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(display2DInfoShowPlayerVelocityAbs)
		);

	std::shared_ptr<Setting<bool>> display2DInfoShowPlayerVelocityXY = std::make_shared<Setting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(display2DInfoShowPlayerVelocityXY)
		);

	std::shared_ptr<Setting<bool>> display2DInfoShowPlayerVelocityXYZ = std::make_shared<Setting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(display2DInfoShowPlayerVelocityXYZ)
		);

	std::shared_ptr<Setting<bool>> display2DInfoShowPlayerHealth = std::make_shared<Setting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(display2DInfoShowPlayerHealth)
		);


	std::shared_ptr<Setting<bool>> display2DInfoShowPlayerRechargeCooldown = std::make_shared<Setting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(display2DInfoShowPlayerRechargeCooldown)
		);

	std::shared_ptr<Setting<bool>> display2DInfoShowPlayerVehicleHealth = std::make_shared<Setting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(display2DInfoShowPlayerVehicleHealth)
		);



	std::shared_ptr<Setting<bool>> display2DInfoTrackCustomObject = std::make_shared<Setting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(display2DInfoTrackCustomObject)
		);

	std::shared_ptr<Setting<uint32_t>> display2DInfoCustomObjectDatum = std::make_shared<Setting<uint32_t>>
		(
			0xDEADB33F,
			[](uint32_t in) { return true; },
			nameof(display2DInfoCustomObjectDatum)
		);

	std::shared_ptr<Setting<bool>> display2DInfoShowEntityObjectType = std::make_shared<Setting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(display2DInfoShowEntityObjectType)
		);

	std::shared_ptr<Setting<bool>> display2DInfoShowEntityTagName = std::make_shared<Setting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(display2DInfoShowEntityTagName)
		);


	std::shared_ptr<Setting<bool>> display2DInfoShowEntityPosition = std::make_shared<Setting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(display2DInfoShowEntityPosition)
		);

	std::shared_ptr<Setting<bool>> display2DInfoShowEntityVelocity = std::make_shared<Setting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(display2DInfoShowEntityVelocity)
		);

	std::shared_ptr<Setting<bool>> display2DInfoShowEntityVelocityAbs = std::make_shared<Setting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(display2DInfoShowEntityVelocityAbs)
		);

	std::shared_ptr<Setting<bool>> display2DInfoShowEntityVelocityXY = std::make_shared<Setting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(display2DInfoShowEntityVelocityXY)
		);

	std::shared_ptr<Setting<bool>> display2DInfoShowEntityVelocityXYZ = std::make_shared<Setting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(display2DInfoShowEntityVelocityXYZ)
		);

	std::shared_ptr<Setting<bool>> display2DInfoShowEntityHealth = std::make_shared<Setting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(display2DInfoShowEntityHealth)
		);

	std::shared_ptr<Setting<bool>> display2DInfoShowEntityRechargeCooldown = std::make_shared<Setting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(display2DInfoShowEntityRechargeCooldown)
		);

	std::shared_ptr<Setting<bool>> display2DInfoShowEntityVehicleHealth = std::make_shared<Setting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(display2DInfoShowEntityVehicleHealth)
		);

	enum class Display2DInfoAnchorEnum
	{
		TopLeft,
		TopRight,
		BottomRight,
		BottomLeft
	};
	// actually an enum but stored as int
	std::shared_ptr<Setting<int>> display2DInfoAnchorCorner = std::make_shared<Setting<int>>
		(
			2, // 0 is top left, 1 is top right, 2 is bottom right, 3 is bottom left
			[](int in) { return in >= 0 && in <= 3; }, // within enum range
			nameof(display2DInfoAnchorCorner)
		);

	std::shared_ptr<Setting<SimpleMath::Vector2>> display2DInfoScreenOffset = std::make_shared<Setting<SimpleMath::Vector2>>
		(
			SimpleMath::Vector2{400, 400},
			[](SimpleMath::Vector2 in) { return in.x >= 0 && in.y >= 0; }, // no negative offsets
			nameof(display2DInfoScreenOffset)
		);

	std::shared_ptr<Setting<int>> display2DInfoFontSize = std::make_shared<Setting<int>>
		(
			16, 
			[](int in) { return in > 0 ; },
			nameof(display2DInfoFontSize)
		);

	std::shared_ptr<Setting<SimpleMath::Vector4>> display2DInfoFontColour = std::make_shared<Setting<SimpleMath::Vector4>>
		(
			SimpleMath::Vector4{1.00f, 0.60f, 0.25f, 1.00f}, // a nice orange colour that matches the rest of the gui
			[](SimpleMath::Vector4 in) { return in.x >= 0 && in.y >= 0 && in.z >= 0 && in.w >= 0 && in.x <= 1 && in.y <= 1 && in.z <= 1 && in.w <= 1; }, // range 0.f ... 1.f 
			nameof(display2DInfoFontColour)
		);

	std::shared_ptr<Setting<int>> display2DInfoFloatPrecision = std::make_shared<Setting<int>>
		(
			6,
			[](int in) { return in >= 0; },
			nameof(display2DInfoFloatPrecision)
		);

	std::shared_ptr<Setting<bool>> display2DInfoOutline = std::make_shared<Setting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(display2DInfoOutline)
		);

	std::shared_ptr<Setting<bool>> hideHUDToggle = std::make_shared<Setting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(hideHUDToggle)
		);

	std::shared_ptr<Setting<SimpleMath::Vector2>> editPlayerViewAngleVec2 = std::make_shared<Setting<SimpleMath::Vector2>>
		(
			SimpleMath::Vector2(4.20, 0.69), // gottem
			[](SimpleMath::Vector2 in) { return (in.x >= 0.f) && (in.x < DirectX::XM_2PI) && (in.y < DirectX::XM_PIDIV2) && (in.y > (DirectX::XM_PIDIV2 * -1.f)); }, // x (yaw) must be from 0 to 6.14, y (pitch) must be from -1.57 to 1.57
			nameof(editPlayerViewAngleVec2)
		);

	std::shared_ptr<Setting<float>> editPlayerViewAngleAdjustFactor = std::make_shared<Setting<float>>
		(
			0.1f,
			[](float in) { return true; },
			nameof(editPlayerViewAngleAdjustFactor)
		);


	enum class FreeCameraInterpolationTypesEnum
	{
			None,
			Linear
	};


	std::shared_ptr<Setting<bool>> freeCameraToggle = std::make_shared<Setting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(freeCameraToggle)
		);

	std::shared_ptr<Setting<bool>> freeCameraHideWatermark = std::make_shared<Setting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(freeCameraHideWatermark)
		);

	std::shared_ptr<Setting<bool>> freeCameraThirdPersonRendering = std::make_shared<Setting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(freeCameraThirdPersonRendering)
		);

	std::shared_ptr<Setting<bool>> freeCameraDisableScreenEffects = std::make_shared<Setting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(freeCameraDisableScreenEffects)
		);


	std::shared_ptr<Setting<bool>> freeCameraHideMessages = std::make_shared<Setting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(freeCameraHideMessages)
		);

	std::shared_ptr<Setting<bool>> freeCameraGameInputDisable = std::make_shared<Setting<bool>>
		(
			true,
			[](bool in) { return true; },
			nameof(freeCameraGameInputDisable)
		);

	std::shared_ptr<Setting<bool>> freeCameraCameraInputDisable = std::make_shared<Setting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(freeCameraCameraInputDisable)
		);

	std::shared_ptr<Setting<float>> freeCameraUserInputCameraTranslationSpeedChangeFactor = std::make_shared<Setting<float>>
		(
			1.5f,
			[](float in) { return in > 1.f; }, // 
			nameof(freeCameraUserInputCameraTranslationSpeedChangeFactor)
		);

	std::shared_ptr<Setting<SimpleMath::Vector3>> freeCameraUserInputCameraSetPositionVec3 = std::make_shared<Setting<SimpleMath::Vector3>>
		(
			SimpleMath::Vector3{ 5.f, 0.f, 0.f },
			[](SimpleMath::Vector3 in) { return true; },
			nameof(freeCameraUserInputCameraSetPositionVec3)
		);

	std::shared_ptr<Setting<SimpleMath::Vector3>> freeCameraUserInputCameraSetRotationVec3 = std::make_shared<Setting<SimpleMath::Vector3>>
		(
			SimpleMath::Vector3{ 0.f, 0.f, 0.f },
			[](SimpleMath::Vector3 in) { return true; },
			nameof(freeCameraUserInputCameraSetRotationVec3)
		);

	std::shared_ptr<Setting<bool>> freeCameraUserInputCameraMaintainVelocity = std::make_shared<Setting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(freeCameraUserInputCameraMaintainVelocity)
		);

	std::shared_ptr<Setting<SimpleMath::Vector3>> freeCameraUserInputCameraSetVelocityVec3 = std::make_shared<Setting<SimpleMath::Vector3>>
		(
			SimpleMath::Vector3{ 5.f, 0.f, 0.f },
			[](SimpleMath::Vector3 in) { return true; },
			nameof(freeCameraUserInputCameraSetVelocityVec3)
		);

	std::shared_ptr<Setting<float>> freeCameraUserInputCameraBaseFOV = std::make_shared<Setting<float>>
		(
			90.f,
			[](float in) { return in > 0.f && in <= 120.f; }, 
			nameof(freeCameraUserInputCameraBaseFOV)
		);


	std::shared_ptr<Setting<float>> freeCameraUserInputCameraTranslationSpeed = std::make_shared<Setting<float>>
		(
			3.f,
			[](float in) { return true; }, // if the user wants to have a negative speed that's their perogative
			nameof(freeCameraUserInputCameraTranslationSpeed)
		);
	
	// actually an enum but stored as int
	std::shared_ptr<Setting<int>> freeCameraUserInputCameraTranslationInterpolator = std::make_shared<Setting<int>>
		(
			1, // 0 none, 1 is proportional
			[](int in) { return in >= 0 && in <= 1; }, // within enum range
			nameof(freeCameraUserInputCameraTranslationInterpolator)
		);

	std::shared_ptr<Setting<float>> freeCameraUserInputCameraTranslationInterpolatorLinearFactor = std::make_shared<Setting<float>>
		(
			0.06f,
			[](float in) { return in > 0.f && in <= 1.f; },
			nameof(freeCameraUserInputCameraTranslationInterpolatorLinearFactor)
		);


	std::shared_ptr<Setting<float>> freeCameraUserInputCameraRotationSpeed = std::make_shared<Setting<float>>
		(
			3.f,
			[](float in) { return true; }, // if the user wants to have a negative speed that's their perogative
			nameof(freeCameraUserInputCameraRotationSpeed)
		);

	// actually an enum but stored as int
	std::shared_ptr<Setting<int>> freeCameraUserInputCameraRotationInterpolator = std::make_shared<Setting<int>>
		(
			1, // 0 none, 1 is proportional
			[](int in) { return in >= 0 && in <= 1; }, // within enum range
			nameof(freeCameraUserInputCameraRotationInterpolator)
		);

	std::shared_ptr<Setting<float>> freeCameraUserInputCameraRotationInterpolatorLinearFactor = std::make_shared<Setting<float>>
		(
			0.06f,
			[](float in) { return in > 0.f && in <= 1.f; },
			nameof(freeCameraUserInputCameraRotationInterpolatorLinearFactor)
		);



	std::shared_ptr<Setting<float>> freeCameraUserInputCameraFOVSpeed = std::make_shared<Setting<float>>
		(
			1.f,
			[](float in) { return true; }, // if the user wants to have a negative speed that's their perogative
			nameof(freeCameraUserInputCameraFOVSpeed)
		);

	// actually an enum but stored as int
	std::shared_ptr<Setting<int>> freeCameraUserInputCameraFOVInterpolator = std::make_shared<Setting<int>>
		(
			1, // 0 none, 1 is proportional
			[](int in) { return in >= 0 && in <= 1; }, // within enum range
			nameof(freeCameraUserInputCameraFOVInterpolator)
		);

	std::shared_ptr<Setting<float>> freeCameraUserInputCameraFOVInterpolatorLinearFactor = std::make_shared<Setting<float>>
		(
			0.06f,
			[](float in) { return in > 0.f && in <= 1.f; },
			nameof(freeCameraUserInputCameraFOVInterpolatorLinearFactor)
		);

	std::shared_ptr<Setting<bool>> freeCameraAnchorPositionToObjectPosition = std::make_shared<Setting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(freeCameraAnchorPositionToObjectPosition)
		);

		// actually an enum but stored as int
		std::shared_ptr<Setting<int>> freeCameraAnchorPositionToObjectPositionObjectToTrackComboGroup = std::make_shared<Setting<int>>
			(
				0, // 0 player, 1 is custom object
				[](int in) { return in >= 0 && in <= 1; }, // within enum range
				nameof(freeCameraAnchorPositionToObjectPositionObjectToTrackComboGroup)
			);

	std::shared_ptr<Setting<uint32_t>> freeCameraAnchorPositionToObjectPositionObjectToTrackCustomObjectDatum = std::make_shared<Setting<uint32_t>>
		(
			0xDEADBEEF,
			[](uint32_t in) { return true; },
			nameof(freeCameraAnchorPositionToObjectPositionObjectToTrackCustomObjectDatum)
		);

	// actually an enum but stored as int
	std::shared_ptr<Setting<int>> freeCameraAnchorPositionToObjectPositionTranslationInterpolator = std::make_shared<Setting<int>>
		(
			1, // 0 none, 1 is proportional
			[](int in) { return in >= 0 && in <= 1; }, // within enum range
			nameof(freeCameraAnchorPositionToObjectPositionTranslationInterpolator)
		);

	std::shared_ptr<Setting<float>> freeCameraAnchorPositionToObjectPositionTranslationInterpolatorLinearFactor = std::make_shared<Setting<float>>
		(
			0.06f,
			[](float in) { return in > 0.f && in <= 1.f; },
			nameof(freeCameraAnchorPositionToObjectPositionTranslationInterpolatorLinearFactor)
		);


	std::shared_ptr<Setting<bool>> freeCameraAnchorPositionToObjectRotation = std::make_shared<Setting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(freeCameraAnchorPositionToObjectRotation)
		);

	std::shared_ptr<Setting<bool>> freeCameraAnchorRotationToObjectPosition = std::make_shared<Setting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(freeCameraAnchorRotationToObjectPosition)
		);

	// actually an enum but stored as int
	std::shared_ptr<Setting<int>> freeCameraAnchorRotationToObjectPositionObjectToTrackComboGroup = std::make_shared<Setting<int>>
		(
			0, // 0 player, 1 is custom object, 2 is absolute position
			[](int in) { return in >= 0 && in <= 2; }, // within enum range
			nameof(freeCameraAnchorRotationToObjectPositionObjectToTrackComboGroup)
		);

	std::shared_ptr<Setting<uint32_t>> freeCameraAnchorRotationToObjectPositionObjectToTrackCustomObjectDatum = std::make_shared<Setting<uint32_t>>
		(
			0xDEADBEEF,
			[](uint32_t in) { return true; },
			nameof(freeCameraAnchorRotationToObjectPositionObjectToTrackCustomObjectDatum)
		);

	std::shared_ptr<Setting<SimpleMath::Vector3>> freeCameraAnchorRotationToObjectPositionObjectToTrackManualPositionVec3 = std::make_shared<Setting<SimpleMath::Vector3>>
		(
			SimpleMath::Vector3{ 0.f, 0.f, 0.f },
			[](SimpleMath::Vector3 in) { return true; },
			nameof(freeCameraAnchorRotationToObjectPositionObjectToTrackManualPositionVec3)
		);

	// actually an enum but stored as int
	std::shared_ptr<Setting<int>> freeCameraAnchorRotationToObjectPositionRotationInterpolator = std::make_shared<Setting<int>>
		(
			1, // 0 none, 1 is proportional
			[](int in) { return in >= 0 && in <= 1; }, // within enum range
			nameof(freeCameraAnchorRotationToObjectPositionRotationInterpolator)
		);

	std::shared_ptr<Setting<float>> freeCameraAnchorRotationToObjectPositionRotationInterpolatorLinearFactor = std::make_shared<Setting<float>>
		(
			0.06f,
			[](float in) { return in > 0.f && in <= 1.f; },
			nameof(freeCameraAnchorRotationToObjectPositionRotationInterpolatorLinearFactor)
		);

	std::shared_ptr<Setting<bool>> freeCameraAnchorRotationToObjectFacing = std::make_shared<Setting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(freeCameraAnchorRotationToObjectFacing)
		);

	// actually an enum but stored as int
	std::shared_ptr<Setting<int>> freeCameraAnchorRotationToObjectFacingObjectToTrackComboGroup = std::make_shared<Setting<int>>
		(
			0, // 0 player, 1 is custom object
			[](int in) { return in >= 0 && in <= 1; }, // within enum range
			nameof(freeCameraAnchorRotationToObjectFacingObjectToTrackComboGroup)
		);

	std::shared_ptr<Setting<uint32_t>> freeCameraAnchorRotationToObjectFacingObjectToTrackCustomObjectDatum = std::make_shared<Setting<uint32_t>>
		(
			0xDEADBEEF,
			[](uint32_t in) { return true; },
			nameof(freeCameraAnchorRotationToObjectFacingObjectToTrackCustomObjectDatum)
		);

	// actually an enum but stored as int
	std::shared_ptr<Setting<int>> freeCameraAnchorRotationToObjectFacingRotationInterpolator = std::make_shared<Setting<int>>
		(
			1, // 0 none, 1 is proportional
			[](int in) { return in >= 0 && in <= 1; }, // within enum range
			nameof(freeCameraAnchorRotationToObjectFacingRotationInterpolator)
		);

	std::shared_ptr<Setting<float>> freeCameraAnchorRotationToObjectFacingRotationInterpolatorLinearFactor = std::make_shared<Setting<float>>
		(
			0.06f,
			[](float in) { return in > 0.f && in <= 1.f; },
			nameof(freeCameraAnchorRotationToObjectFacingRotationInterpolatorLinearFactor)
		);

	std::shared_ptr<Setting<bool>> freeCameraAnchorFOVToObjectDistance = std::make_shared<Setting<bool>>
		(
			false,
			[](bool in) { return true; },
			nameof(freeCameraAnchorFOVToObjectDistance)
		);

	// actually an enum but stored as int
	std::shared_ptr<Setting<int>> freeCameraAnchorFOVToObjectDistanceObjectToTrackComboGroup = std::make_shared<Setting<int>>
		(
			0, // 0 player, 1 is custom object, 2 is absolute position
			[](int in) { return in >= 0 && in <= 2; }, // within enum range
			nameof(freeCameraAnchorFOVToObjectDistanceObjectToTrackComboGroup)
		);

	std::shared_ptr<Setting<uint32_t>> freeCameraAnchorFOVToObjectDistanceObjectToTrackCustomObjectDatum = std::make_shared<Setting<uint32_t>>
		(
			0xDEADBEEF,
			[](uint32_t in) { return true; },
			nameof(freeCameraAnchorFOVToObjectDistanceObjectToTrackCustomObjectDatum)
		);

	std::shared_ptr<Setting<SimpleMath::Vector3>> freeCameraAnchorFOVToObjectDistanceObjectToTrackManualPositionVec3 = std::make_shared<Setting<SimpleMath::Vector3>>
		(
			SimpleMath::Vector3{ 0.f, 0.f, 0.f },
			[](SimpleMath::Vector3 in) { return true; },
			nameof(freeCameraAnchorFOVToObjectDistanceObjectToTrackManualPositionVec3)
		);

	// actually an enum but stored as int
	std::shared_ptr<Setting<int>> freeCameraAnchorFOVToObjectDistanceFOVInterpolator = std::make_shared<Setting<int>>
		(
			1, // 0 none, 1 is proportional
			[](int in) { return in >= 0 && in <= 1; }, // within enum range
			nameof(freeCameraAnchorFOVToObjectDistanceFOVInterpolator)
		);

	std::shared_ptr<Setting<float>> freeCameraAnchorFOVToObjectDistanceFOVInterpolatorLinearFactor = std::make_shared<Setting<float>>
		(
			0.06f,
			[](float in) { return in > 0.f && in <= 1.f; },
			nameof(freeCameraAnchorFOVToObjectDistanceFOVInterpolatorLinearFactor)
		);

	std::shared_ptr<Setting<int>> switchBSPIndex = std::make_shared<Setting<int>>
		(
			0,
			[](int in) { return true; },
			nameof(switchBSPIndex)
		);




	enum class FreeCameraObjectTrackEnum
	{
		Player,
		CustomObject
	};

	enum class FreeCameraObjectTrackEnumPlusAbsolute
	{
		Player,
		CustomObject,
		ManualPosition
	};

	// settings that ought to be serialised/deserialised between HCM runs
	std::vector<std::shared_ptr<SerialisableSetting>> allSerialisableOptions
	{
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
		invulnerabilityNPCToggle, 
		speedhackSetting, 
		GUIShowingFreesCursor, 
		GUIShowingBlocksInput, 
		GUIShowingPausesGame, 
		pauseAlsoBlocksInput,
		pauseAlsoFreesCursor,
		consoleCommandString,
		getObjectAddressDWORD,
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
		//freeCameraToggle,
		freeCameraHideWatermark,
		freeCameraThirdPersonRendering,
		freeCameraHideMessages,
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
		switchBSPIndex,

	};
};

