#pragma once
#include "SettingsStateAndEvents.h"


// actual hotkey events are defined in SettingsStateAndEvents. this class simply binds the hotkey event of eg toggle settings to actually doing something (ie flip the toggle setting).
// simple ActionEvent hotkeys do not need this.

class HotkeyEventsLambdas
{
	ScopedCallback<ActionEvent> mSpeedhackHotkeyCallbackHandle;
	ScopedCallback<ActionEvent> mInvulnerabilityHotkeyCallbackHandle;
	ScopedCallback<ActionEvent> mAIFreezeHotkeyCallbackHandle;
	ScopedCallback<ActionEvent> mMedusaHotkeyCallbackHandle;
	ScopedCallback<ActionEvent> mToggleGUIHotkeyCallbackHandle;
	ScopedCallback<ActionEvent> mTogglePauseHotkeyCallbackHandle;
	ScopedCallback<ActionEvent> mNaturalCheckpointDisableHotkeyCallbackHandle;
	ScopedCallback<ActionEvent> mInfiniteAmmoHotkeyCallbackHandle;
	ScopedCallback<ActionEvent> mBottomlessClipHotkeyCallbackHandle;
	ScopedCallback<ActionEvent> mDisplay2DInfoHotkeyCallbackHandle;
	ScopedCallback<ActionEvent> mFreeCameraHotkeyCallbackHandle;
	ScopedCallback<ActionEvent> mFreeCameraGameInputDisableHotkeyCallbackHandle;
	ScopedCallback<ActionEvent> mFreeCameraCameraInputDisableHotkeyCallbackHandle;
	ScopedCallback<ActionEvent> mFreeCameraUserInputCameraMaintainVelocityHotkeyCallbackHandle;
	ScopedCallback<ActionEvent> mHideHUDToggleHotkeyEventCallbackHandle;
	ScopedCallback<ActionEvent> mtoggleWaypoint3DHotkeyEventCallbackHandle;
	ScopedCallback<ActionEvent> mtriggerOverlayToggleHotkeyEventCallbackHangle;
	ScopedCallback<ActionEvent> mdisableBarriersHotkeyEventCallbackHangle;
	ScopedCallback<ActionEvent> msoftCeilingOverlayToggleHotkeyEventCallbackHandle;
	ScopedCallback<ActionEvent> mabilityMeterToggleHotkeyEventCallbackHandle;
	ScopedCallback<ActionEvent> msensDriftOverlayToggleHotkeyEventCallbackHandle;

public:
	HotkeyEventsLambdas(std::shared_ptr< SettingsStateAndEvents> settings)
		: mSpeedhackHotkeyCallbackHandle(settings->speedhackHotkeyEvent, [boolsetting = settings->speedhackToggle]() { boolsetting->flipBoolSetting(); }),
		mInvulnerabilityHotkeyCallbackHandle(settings->invulnerabilityHotkeyEvent, [boolsetting = settings->invulnerabilityToggle]() { boolsetting->flipBoolSetting(); }),
		mAIFreezeHotkeyCallbackHandle(settings->aiFreezeHotkeyEvent, [boolsetting = settings->aiFreezeToggle]() { boolsetting->flipBoolSetting(); }),
		mMedusaHotkeyCallbackHandle(settings->medusaHotkeyEvent, [boolsetting = settings->medusaToggle]() { boolsetting->flipBoolSetting(); }),
		mToggleGUIHotkeyCallbackHandle(settings->toggleGUIHotkeyEvent, [boolsetting = settings->GUIWindowOpen]() { boolsetting->flipBoolSetting(); }),
		mTogglePauseHotkeyCallbackHandle(settings->togglePauseHotkeyEvent, [boolsetting = settings->togglePause]() { boolsetting->flipBoolSetting(); }),
		mNaturalCheckpointDisableHotkeyCallbackHandle(settings->naturalCheckpointDisableHotkeyEvent, [boolsetting = settings->naturalCheckpointDisable]() { boolsetting->flipBoolSetting(); }),
		mInfiniteAmmoHotkeyCallbackHandle(settings->infiniteAmmoHotkeyEvent, [boolsetting = settings->infiniteAmmoToggle]() { boolsetting->flipBoolSetting(); }),
		mBottomlessClipHotkeyCallbackHandle(settings->bottomlessClipHotkeyEvent, [boolsetting = settings->bottomlessClipToggle]() { boolsetting->flipBoolSetting(); }),
		mDisplay2DInfoHotkeyCallbackHandle(settings->display2DInfoHotkeyEvent, [boolsetting = settings->display2DInfoToggle]() { boolsetting->flipBoolSetting(); }),
		mFreeCameraHotkeyCallbackHandle(settings->freeCameraHotkeyEvent, [boolsetting = settings->freeCameraToggle]() { boolsetting->flipBoolSetting(); }),
		mFreeCameraGameInputDisableHotkeyCallbackHandle(settings->freeCameraGameInputDisableHotkeyEvent, [boolsetting = settings->freeCameraGameInputDisable]() { boolsetting->flipBoolSetting(); }),
		mFreeCameraCameraInputDisableHotkeyCallbackHandle(settings->freeCameraCameraInputDisableHotkeyEvent, [boolsetting = settings->freeCameraCameraInputDisable]() { boolsetting->flipBoolSetting(); }),
		mFreeCameraUserInputCameraMaintainVelocityHotkeyCallbackHandle(settings->freeCameraUserInputCameraMaintainVelocityHotkeyEvent, [boolsetting = settings->freeCameraUserInputCameraMaintainVelocity]() { boolsetting->flipBoolSetting(); }),
		mHideHUDToggleHotkeyEventCallbackHandle(settings->hideHUDToggleHotkeyEvent, [boolsetting = settings->hideHUDToggle]() { boolsetting->flipBoolSetting(); }),
		mtoggleWaypoint3DHotkeyEventCallbackHandle(settings->toggleWaypoint3DHotkeyEvent, [boolsetting = settings->waypoint3DToggle]() { boolsetting->flipBoolSetting(); }),
		mtriggerOverlayToggleHotkeyEventCallbackHangle(settings->triggerOverlayToggleHotkeyEvent, [boolsetting = settings->triggerOverlayToggle]() {boolsetting->flipBoolSetting(); }),
		mdisableBarriersHotkeyEventCallbackHangle(settings->disableBarriersHotkeyEvent, [boolsettings = settings->disableBarriersToggle]() {boolsettings->flipBoolSetting(); }),
		msoftCeilingOverlayToggleHotkeyEventCallbackHandle(settings->softCeilingOverlayToggleHotkeyEvent, [boolsetting = settings->softCeilingOverlayToggle]() {boolsetting->flipBoolSetting(); }),
		mabilityMeterToggleHotkeyEventCallbackHandle(settings->abilityMeterToggleHotkeyEvent, [boolsetting = settings->abilityMeterOverlayToggle]() {boolsetting->flipBoolSetting(); }),
		msensDriftOverlayToggleHotkeyEventCallbackHandle(settings->sensDriftOverlayToggleHotkeyEvent, [boolsetting = settings->sensDriftOverlayToggle]() {boolsetting->flipBoolSetting(); })
		

	{ PLOG_DEBUG << "HotkeyEvents con"; }

	~HotkeyEventsLambdas() { PLOG_DEBUG << "~HotkeyEvents"; }
};