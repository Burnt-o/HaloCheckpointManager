#pragma once
#include "pch.h"
#include "Hotkey.h"

#include "SettingsStateAndEvents.h"
#include "imgui.h"
#include "boost\preprocessor.hpp"
class HotkeyDefinitions {
public:

	HotkeyDefinitions(std::shared_ptr<SettingsStateAndEvents> settings) : mSettings(settings) 
	{
		PLOG_DEBUG << "HotkeyDefinitions constructor";


		assert(std::size(allHotkeys) == hotkeyEnumCount && "Bad hotkey definition count - do you have a hotkey defined for each enum entry?");
	}

	~HotkeyDefinitions()
	{
		PLOG_DEBUG << "~HotkeyDefinitions";
	}
	//friend class HotkeyManager; 
	const std::map<HotkeysEnum, std::shared_ptr<Hotkey>>& getHotkeys() { return allHotkeys; }

private:
	std::shared_ptr< SettingsStateAndEvents> mSettings;

	typedef std::vector<std::vector<ImGuiKey>> vvk;
#define initHotkey(enumName, hotkeyEvent, defaultBinding) \
		{HotkeysEnum::enumName, std::make_shared<Hotkey>\
		(hotkeyEvent,\
		HotkeysEnum::enumName,\
		defaultBinding) }

	static constexpr inline int hotkeyEnumCount = BOOST_PP_TUPLE_SIZE((ALLHOTKEYS));

	const std::map<HotkeysEnum, std::shared_ptr<Hotkey>>::value_type allHotkeysData[hotkeyEnumCount]
	{
		// TODO: make a macro that automates this, since the names are set up good like.
		// TODO: bind the lambda for toggle events here instead of HotkeyEventsLambdas

		initHotkey(toggleGUI,
		mSettings->toggleGUIHotkeyEvent,
		vvk{{ ImGuiKey_GraveAccent }}),

		initHotkey(togglePause,
		mSettings->togglePauseHotkeyEvent,
		vvk{{ ImGuiKey_P }}),

		initHotkey(advanceTicks,
		mSettings->advanceTicksEvent,
		vvk{}),

		initHotkey(forceCheckpoint,
		mSettings->forceCheckpointEvent,
		vvk{{ ImGuiKey_F1 }}),

		initHotkey(forceRevert,
		mSettings->forceRevertEvent,
		vvk{{ ImGuiKey_F2 }}),

		initHotkey(forceDoubleRevert,
		mSettings->forceDoubleRevertEvent,
		vvk{{ ImGuiKey_F3 }}),

		initHotkey(forceCoreSave,
		mSettings->forceCoreSaveEvent,
		vvk{}),

		initHotkey(forceCoreLoad,
		mSettings->forceCoreLoadEvent,
		vvk{}),

		initHotkey(injectCheckpoint,
		mSettings->injectCheckpointEvent,
		vvk{}),

		initHotkey(dumpCheckpoint,
		mSettings->dumpCheckpointEvent,
		vvk{}),

		initHotkey(injectCore,
		mSettings->injectCoreEvent,
		vvk{}),

		initHotkey(dumpCore,
		mSettings->dumpCoreEvent,
		vvk{}),

		initHotkey(speedhack,
		mSettings->speedhackHotkeyEvent,
		vvk{{ ImGuiKey_F4 }}),

		initHotkey(invuln,
		mSettings->invulnerabilityHotkeyEvent,
		vvk{{ ImGuiKey_F5 }}),

		initHotkey(aiFreeze,
		mSettings->aiFreezeHotkeyEvent,
		vvk{}),

		initHotkey(forceTeleport,
		mSettings->forceTeleportEvent,
		vvk{}),

		initHotkey(forceLaunch,
		mSettings->forceLaunchEvent,
		vvk{}),

		initHotkey(medusa,
		mSettings->medusaHotkeyEvent,
		vvk{}),

		initHotkey(naturalCheckpointDisable,
		mSettings->naturalCheckpointDisableHotkeyEvent,
		vvk{}),


		initHotkey(infiniteAmmo,
		mSettings->infiniteAmmoHotkeyEvent,
		vvk{}),


		initHotkey(bottomlessClip,
		mSettings->bottomlessClipHotkeyEvent,
		vvk{}),


		initHotkey(display2DInfo,
		mSettings->display2DInfoHotkeyEvent,
		vvk{}),

		initHotkey(freeCamera,
		mSettings->freeCameraHotkeyEvent,
		vvk{}),

		initHotkey(freeCameraGameInputDisable,
		mSettings->freeCameraGameInputDisableHotkeyEvent,
		vvk{}),

		initHotkey(freeCameraCameraInputDisable,
		mSettings->freeCameraCameraInputDisableHotkeyEvent,
		vvk{}),

		initHotkey(freeCameraUserInputCameraIncreaseTranslationSpeedHotkey,
		mSettings->freeCameraUserInputCameraIncreaseTranslationSpeedHotkey,
		vvk{}),

		initHotkey(freeCameraUserInputCameraDecreaseTranslationSpeedHotkey,
		mSettings->freeCameraUserInputCameraDecreaseTranslationSpeedHotkey,
		vvk{}),

		initHotkey(freeCameraUserInputCameraSetPosition,
		mSettings->freeCameraUserInputCameraSetPosition,
		vvk{}),

		initHotkey(freeCameraUserInputCameraSetRotation,
		mSettings->freeCameraUserInputCameraSetRotation,
		vvk{}),

		initHotkey(freeCameraUserInputCameraMaintainVelocity,
		mSettings->freeCameraUserInputCameraMaintainVelocityHotkeyEvent,
		vvk{}),

		initHotkey(freeCameraUserInputCameraSetVelocity,
		mSettings->freeCameraUserInputCameraSetVelocity,
		vvk{}),

			initHotkey(freeCameraAnchorPositionToObjectPosition,
				mSettings->freeCameraAnchorPositionToObjectPositionHotkeyEvent,
				vvk{}),

			initHotkey(freeCameraAnchorPositionToObjectRotation,
				mSettings->freeCameraAnchorPositionToObjectRotationHotkeyEvent,
				vvk{}),

			initHotkey(freeCameraAnchorRotationToObjectPosition,
				mSettings->freeCameraAnchorRotationToObjectPositionHotkeyEvent,
				vvk{}),

			initHotkey(freeCameraAnchorRotationToObjectFacing,
				mSettings->freeCameraAnchorRotationToObjectFacingHotkeyEvent,
				vvk{}),

			initHotkey(freeCameraAnchorFOVToObjectDistance,
				mSettings->freeCameraAnchorFOVToObjectDistanceHotkeyEvent,
				vvk{}),

			initHotkey(editPlayerViewAngleSet,
				mSettings->editPlayerViewAngleSet,
				vvk{}),

			initHotkey(editPlayerViewAngleAdjustHorizontal,
				mSettings->editPlayerViewAngleAdjustHorizontal,
				vvk{}),

			initHotkey(editPlayerViewAngleAdjustVertical,
				mSettings->editPlayerViewAngleAdjustVertical,
				vvk{}),

			initHotkey(switchBSP,
				mSettings->switchBSPEvent,
				vvk{}),




	};

	 std::map<HotkeysEnum, std::shared_ptr<Hotkey>> allHotkeys = std::map<HotkeysEnum, std::shared_ptr<Hotkey>>
		(std::begin(allHotkeysData), std::end(allHotkeysData) );

	 friend class HotkeyManager;




};
