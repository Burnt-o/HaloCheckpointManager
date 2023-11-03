#pragma once
#include "pch.h"
#include "Hotkey.h"

#include "SettingsStateAndEvents.h"
#include "imgui.h"
class HotkeyDefinitions {
public:

	HotkeyDefinitions(std::shared_ptr<SettingsStateAndEvents> settings) : mSettings(settings) 
	{
		PLOG_DEBUG << "HotkeyDefinitions constructor";
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

	static constexpr inline int hotkeyEnumCount = 17;// magic_enum::enum_count<HotkeysEnum>();
	static constexpr inline int magicEnumCount = magic_enum::enum_count<HotkeysEnum>();

	const std::map<HotkeysEnum, std::shared_ptr<Hotkey>>::value_type allHotkeysData[hotkeyEnumCount]
	{
		// TODO: make a macro that automates this, since the names are set up good like.
		// TODO: bind the lambda for toggle events here instead of HotkeyEventsLambdas
		// TODO: figure out how to get the array count automagically (prob can do in the macro)

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
		vvk{})


	};

	 std::map<HotkeysEnum, std::shared_ptr<Hotkey>> allHotkeys = std::map<HotkeysEnum, std::shared_ptr<Hotkey>>
		(std::begin(allHotkeysData), std::end(allHotkeysData) );

	 friend class HotkeyManager;

	// TODO: why is magic_enum::enum_count not working?
	//static_assert(std::size(allHotkeysData) == hotkeyEnumCount, "Bad hotkey definition count - do you have a hotkey defined for each enum entry?");


};
