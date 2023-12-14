#pragma once
#include "pch.h"
#include "imgui.h"
#include "HotkeysEnum.h"
#include "RebindableHotkey.h"



class EventOnPressHotkey : public RebindableHotkey // a hotkey that fires an event when going from unpressed to pressed
{
private:
	std::shared_ptr<ActionEvent> mEvent;

public:
	explicit EventOnPressHotkey(std::shared_ptr<ActionEvent> actionEvent, RebindableHotkeyEnum hotkeyEnum, std::vector<std::vector<ImGuiKey>> defaultBindings = { })
		: RebindableHotkey(hotkeyEnum, defaultBindings), mEvent(actionEvent)
	{
	}

	void invokeEvent()
	{
		if (!mEvent.get()) PLOG_ERROR << "event not set";

		mEvent->operator()();
	}
};