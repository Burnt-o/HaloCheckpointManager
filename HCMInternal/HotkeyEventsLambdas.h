#pragma once
#include "SettingsStateAndEvents.h"


// actual hotkey events are defined in SettingsStateAndEvents. this class simply binds the hotkey event of eg toggle settings to actually doing something (ie flip the toggle setting).
// simple ActionEvent hotkeys do not need this.

class HotkeyEventsLambdas
{
	ScopedCallback<ActionEvent> mSpeedhackHotkeyCallbackHandle;
	ScopedCallback<ActionEvent> mInvulnerabilityHotkeyCallbackHandle;
	ScopedCallback<ActionEvent> mAIFreezeHotkeyCallbackHandle;
	ScopedCallback<ActionEvent> mToggleGUIHotkeyCallbackHandle;

public:
	HotkeyEventsLambdas(std::shared_ptr< SettingsStateAndEvents> settings)
		: mSpeedhackHotkeyCallbackHandle(settings->speedhackHotkeyEvent, [boolsetting = settings->speedhackToggle]() { boolsetting->flipBoolSetting(); }),
		mInvulnerabilityHotkeyCallbackHandle(settings->invulnerabilityHotkeyEvent, [boolsetting = settings->invulnerabilityToggle]() { boolsetting->flipBoolSetting(); }),
		mAIFreezeHotkeyCallbackHandle(settings->aiFreezeHotkeyEvent, [boolsetting = settings->aiFreezeToggle]() { boolsetting->flipBoolSetting(); }),
		mToggleGUIHotkeyCallbackHandle(settings->toggleGUIHotkeyEvent, [boolsetting = settings->GUIWindowOpen]() { boolsetting->flipBoolSetting(); })

	{ PLOG_DEBUG << "HotkeyEvents con"; }

	~HotkeyEventsLambdas() { PLOG_DEBUG << "~HotkeyEvents"; }
};