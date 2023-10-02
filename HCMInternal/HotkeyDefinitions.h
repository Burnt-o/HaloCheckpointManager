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
	gsl::not_null<std::shared_ptr< SettingsStateAndEvents>> mSettings;

	typedef std::vector<std::vector<ImGuiKey>> vvk;
#define initHotkey(enumName, hotkeyEvent, defaultBinding) \
		{HotkeysEnum::enumName, std::make_shared<Hotkey>\
		(hotkeyEvent,\
		HotkeysEnum::enumName,\
		defaultBinding) }

	static constexpr inline int hotkeyEnumCount = 11;// magic_enum::enum_count<HotkeysEnum>();
	static constexpr inline int magicEnumCount = magic_enum::enum_count<HotkeysEnum>();

	const std::map<HotkeysEnum, std::shared_ptr<Hotkey>>::value_type allHotkeysData[hotkeyEnumCount]
	{
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
		vvk{{ ImGuiKey_F5 }})

	};

	 std::map<HotkeysEnum, std::shared_ptr<Hotkey>> allHotkeys = std::map<HotkeysEnum, std::shared_ptr<Hotkey>>
		(std::begin(allHotkeysData), std::end(allHotkeysData) );

	 friend class HotkeyManager;

	// TODO: why is magic_enum::enum_count not working?
	//static_assert(std::size(allHotkeysData) == hotkeyEnumCount, "Bad hotkey definition count - do you have a hotkey defined for each enum entry?");


};
