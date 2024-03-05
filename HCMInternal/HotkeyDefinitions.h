#pragma once
#include "pch.h"
#include "RebindableHotkey.h"
#include "EventOnPressHotkey.h"

#include "SettingsStateAndEvents.h"
#include "imgui.h"
#include "boost\preprocessor.hpp"
class HotkeyDefinitions {
public:

	HotkeyDefinitions(std::shared_ptr<SettingsStateAndEvents> settings) : mSettings(settings)
	{
		PLOG_DEBUG << "HotkeyDefinitions constructor";

		assert(rebindableOnlyCount >= 0 && "How the fuck do you have fewer rebindable hotkeys than eventonpresshotkeys?!");



		// need to manually setup allRebindableHotkeys since it's too much of pain to do with initializer lists

		// all rebindable inclues all eventsOnPress hotkeys
		for (auto& [key, val] : allEventOnPressHotkeys)
		{
			allRebindableHotkeys.insert(std::make_pair(key, val));
		}

		// add rebindable-only data
		for (auto& [key, val] : allRebindableOnlyHotkeysData)
		{
			allRebindableHotkeys.insert(std::make_pair(key, val));
		}

		PLOG_VERBOSE << "std::size(allEventOnPressHotkeys) " << std::size(allEventOnPressHotkeys);
		PLOG_VERBOSE << "allRebindableHotkeyEnumCount " << allRebindableHotkeyEnumCount;
		PLOG_VERBOSE << "std::size(allRebindableHotkeys) " << std::size(allRebindableHotkeys);
		PLOG_VERBOSE << "allEventOnPressHotkeyEnumCount " << allEventOnPressHotkeyEnumCount;
		PLOG_VERBOSE << "rebindableOnlyCount " << rebindableOnlyCount;

		assert(std::size(allEventOnPressHotkeys) == allEventOnPressHotkeyEnumCount && "Bad eventonpresshotkey definition count - do you have a hotkey defined for each enum entry?");
		assert(std::size(allRebindableHotkeys) == allRebindableHotkeyEnumCount && "Bad rebindablehotkey definition count - do you have a hotkey defined for each enum entry?");


	}

	~HotkeyDefinitions()
	{
		PLOG_DEBUG << "~HotkeyDefinitions";
	}
	//friend class HotkeyManager; 
	const std::map<RebindableHotkeyEnum, std::shared_ptr<EventOnPressHotkey>>& getAllEventOnPressHotkeys() { return allEventOnPressHotkeys; }
	const std::map<RebindableHotkeyEnum, std::shared_ptr<RebindableHotkey>>& getAllRebindableHotkeys() { return allRebindableHotkeys; }

private:
	std::shared_ptr< SettingsStateAndEvents> mSettings;

	typedef std::vector<std::vector<ImGuiKey>> vvk;

#define initEventOnPressHotkey(enumName, hotkeyEvent, defaultBinding) \
		{RebindableHotkeyEnum::enumName, std::make_shared<EventOnPressHotkey>\
		(hotkeyEvent,\
		RebindableHotkeyEnum::enumName,\
		defaultBinding) }

#define initRebindableHotkey(enumName, defaultBinding) \
		{RebindableHotkeyEnum::enumName, std::make_shared<RebindableHotkey>\
		(RebindableHotkeyEnum::enumName,\
		defaultBinding) }

	static constexpr inline int allEventOnPressHotkeyEnumCount = BOOST_PP_TUPLE_SIZE((ALL_EVENTONPRESS_HOTKEYS));
	static constexpr inline int allRebindableHotkeyEnumCount = BOOST_PP_TUPLE_SIZE((ALL_REBINDABLE_HOTKEYS));
	static constexpr inline int rebindableOnlyCount = allRebindableHotkeyEnumCount - allEventOnPressHotkeyEnumCount;

	const std::map<RebindableHotkeyEnum, std::shared_ptr<EventOnPressHotkey>>::value_type allEventOnPressHotkeysData[allEventOnPressHotkeyEnumCount]
	{

		initEventOnPressHotkey(toggleGUI,
		mSettings->toggleGUIHotkeyEvent,
		vvk{{ ImGuiKey_GraveAccent }}),

		initEventOnPressHotkey(togglePause,
		mSettings->togglePauseHotkeyEvent,
		vvk{{ ImGuiKey_P }}),

		initEventOnPressHotkey(advanceTicks,
		mSettings->advanceTicksEvent,
		vvk{}),

		initEventOnPressHotkey(forceCheckpoint,
		mSettings->forceCheckpointEvent,
		vvk{{ ImGuiKey_F1 }}),

		initEventOnPressHotkey(forceRevert,
		mSettings->forceRevertEvent,
		vvk{{ ImGuiKey_F2 }}),

		initEventOnPressHotkey(forceDoubleRevert,
		mSettings->forceDoubleRevertEvent,
		vvk{{ ImGuiKey_F3 }}),

		initEventOnPressHotkey(forceCoreSave,
		mSettings->forceCoreSaveEvent,
		vvk{}),

		initEventOnPressHotkey(forceCoreLoad,
		mSettings->forceCoreLoadEvent,
		vvk{}),

		initEventOnPressHotkey(injectCheckpoint,
		mSettings->injectCheckpointEvent,
		vvk{}),

		initEventOnPressHotkey(dumpCheckpoint,
		mSettings->dumpCheckpointEvent,
		vvk{}),

		initEventOnPressHotkey(injectCore,
		mSettings->injectCoreEvent,
		vvk{}),

		initEventOnPressHotkey(dumpCore,
		mSettings->dumpCoreEvent,
		vvk{}),

		initEventOnPressHotkey(speedhack,
		mSettings->speedhackHotkeyEvent,
		vvk{{ ImGuiKey_F4 }}),

		initEventOnPressHotkey(invuln,
		mSettings->invulnerabilityHotkeyEvent,
		vvk{{ ImGuiKey_F5 }}),

		initEventOnPressHotkey(aiFreeze,
		mSettings->aiFreezeHotkeyEvent,
		vvk{}),

		initEventOnPressHotkey(forceTeleport,
		mSettings->forceTeleportEvent,
		vvk{}),

		initEventOnPressHotkey(forceLaunch,
		mSettings->forceLaunchEvent,
		vvk{}),

		initEventOnPressHotkey(medusa,
		mSettings->medusaHotkeyEvent,
		vvk{}),

		initEventOnPressHotkey(naturalCheckpointDisable,
		mSettings->naturalCheckpointDisableHotkeyEvent,
		vvk{}),


		initEventOnPressHotkey(infiniteAmmo,
		mSettings->infiniteAmmoHotkeyEvent,
		vvk{}),


		initEventOnPressHotkey(bottomlessClip,
		mSettings->bottomlessClipHotkeyEvent,
		vvk{}),


		initEventOnPressHotkey(display2DInfo,
		mSettings->display2DInfoHotkeyEvent,
		vvk{}),

		initEventOnPressHotkey(freeCamera,
		mSettings->freeCameraHotkeyEvent,
		vvk{}),

		initEventOnPressHotkey(freeCameraGameInputDisable,
		mSettings->freeCameraGameInputDisableHotkeyEvent,
		vvk{}),

			initEventOnPressHotkey(freeCameraCameraInputDisable,
		mSettings->freeCameraCameraInputDisableHotkeyEvent,
		vvk{}),

			initEventOnPressHotkey(freeCameraUserInputCameraIncreaseTranslationSpeedHotkey,
		mSettings->freeCameraUserInputCameraIncreaseTranslationSpeedHotkey,
		vvk{}),

			initEventOnPressHotkey(freeCameraUserInputCameraDecreaseTranslationSpeedHotkey,
		mSettings->freeCameraUserInputCameraDecreaseTranslationSpeedHotkey,
		vvk{}),

			initEventOnPressHotkey(freeCameraUserInputCameraSetPosition,
		mSettings->freeCameraUserInputCameraSetPosition,
		vvk{}),

			initEventOnPressHotkey(freeCameraUserInputCameraSetRotation,
		mSettings->freeCameraUserInputCameraSetRotation,
		vvk{}),

			initEventOnPressHotkey(freeCameraUserInputCameraMaintainVelocity,
		mSettings->freeCameraUserInputCameraMaintainVelocityHotkeyEvent,
		vvk{}),

			initEventOnPressHotkey(freeCameraUserInputCameraSetVelocity,
		mSettings->freeCameraUserInputCameraSetVelocity,
		vvk{}),

		initEventOnPressHotkey(freeCameraAnchorPositionToObjectPosition,
			mSettings->freeCameraAnchorPositionToObjectPositionHotkeyEvent,
			vvk{}),

		initEventOnPressHotkey(freeCameraAnchorPositionToObjectRotation,
			mSettings->freeCameraAnchorPositionToObjectRotationHotkeyEvent,
			vvk{}),

		initEventOnPressHotkey(freeCameraAnchorRotationToObjectPosition,
			mSettings->freeCameraAnchorRotationToObjectPositionHotkeyEvent,
			vvk{}),

		initEventOnPressHotkey(freeCameraAnchorRotationToObjectFacing,
			mSettings->freeCameraAnchorRotationToObjectFacingHotkeyEvent,
			vvk{}),

		initEventOnPressHotkey(freeCameraAnchorFOVToObjectDistance,
			mSettings->freeCameraAnchorFOVToObjectDistanceHotkeyEvent,
			vvk{}),

		initEventOnPressHotkey(editPlayerViewAngleSet,
			mSettings->editPlayerViewAngleSet,
			vvk{}),

		initEventOnPressHotkey(editPlayerViewAngleAdjustHorizontal,
			mSettings->editPlayerViewAngleAdjustHorizontal,
			vvk{}),

		initEventOnPressHotkey(editPlayerViewAngleAdjustVertical,
			mSettings->editPlayerViewAngleAdjustVertical,
			vvk{}),

		initEventOnPressHotkey(switchBSP,
			mSettings->switchBSPEvent,
			vvk{}),


		initEventOnPressHotkey(hideHUDToggle,
			mSettings->hideHUDToggleHotkeyEvent,
			vvk{}),



		initEventOnPressHotkey(setPlayerHealth,
		mSettings->setPlayerHealthEvent,
		vvk{}),

		initEventOnPressHotkey(toggleWaypoint3D,
		mSettings->toggleWaypoint3DHotkeyEvent,
		vvk{}),


	};

	const std::map<RebindableHotkeyEnum, std::shared_ptr<RebindableHotkey>>::value_type allRebindableOnlyHotkeysData[rebindableOnlyCount]
	{

						initRebindableHotkey(cameraTranslateUpBinding,
			(vvk{{ ImGuiKey_Space}, { ImGuiKey_GamepadFaceDown }})),

						initRebindableHotkey(cameraTranslateDownBinding,
			(vvk{{ ImGuiKey_LeftCtrl }, { ImGuiKey_GamepadL3}})),

						initRebindableHotkey(cameraRollLeftBinding,
			(vvk{{ ImGuiKey_G }, { ImGuiKey_GamepadR1}})),

						initRebindableHotkey(cameraRollRightBinding,
			(vvk{{ ImGuiKey_T }, { ImGuiKey_GamepadL1 }})),

						initRebindableHotkey(cameraFOVIncreaseBinding,
			(vvk{{ ImGuiKey_Y }, { ImGuiKey_GamepadL2 }})),

						initRebindableHotkey(cameraFOVDecreaseBinding,
			(vvk{{ ImGuiKey_H }, { ImGuiKey_GamepadR2 }})),


	};

	 std::map<RebindableHotkeyEnum, std::shared_ptr<EventOnPressHotkey>> allEventOnPressHotkeys = std::map<RebindableHotkeyEnum, std::shared_ptr<EventOnPressHotkey>>
		(std::begin(allEventOnPressHotkeysData), std::end(allEventOnPressHotkeysData) );

	 std::map<RebindableHotkeyEnum, std::shared_ptr<RebindableHotkey>> allRebindableHotkeys; // defined in constructor

	 friend class HotkeyManager;




};
