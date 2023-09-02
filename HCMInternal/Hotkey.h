#pragma once
#include "pch.h"
#include "imgui.h"
class Hotkey  //represents hotkeys for keyboard, gamepad, and mouse
{
private:
	std::shared_ptr<ActionEvent> mEvent;
	std::string mHotkeyName;
public:
	std::string_view getName() const { return mHotkeyName; }


	std::vector<std::vector<ImGuiKey>> mBindings{}; // an ImGuiKey can be for either mouse, gamepad, or keyboard. Having a vector of them means we can do combo bindings, ie press "Shift + F1" to activate a hotkey. Also allows us to mix input types, ie "MouseLeft + F1".
	// vector of vectors so user can do more than one set of bindings




	
	// actionEvent is what event to fire when hotkey activated. hotkeyName is just used for (de)serialisation. Bindings default to empty (unbound).
	explicit Hotkey(std::shared_ptr<ActionEvent> actionEvent, std::string hotkeyName, std::vector<std::vector<ImGuiKey>> defaultBindings = {}) : mEvent(actionEvent), mHotkeyName(hotkeyName), mBindings(defaultBindings) {}

	void invokeEvent()
	{
		if (!mEvent.get()) PLOG_ERROR << "event not set";
		mEvent.get()->operator()();
	}
	
	friend std::ostream& operator<< (std::ostream& out, const Hotkey& g) // to string stream, for serialisation
	{
		for (auto binding : g.mBindings)
		{
			for (auto key : binding)
			{
				out << (int)key << ", ";
			}
			out << " - ";
		}

		return out;
	}

	// functions for serialising and deserialising are defined in HotkeyManager


	
};