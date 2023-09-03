#pragma once
#include "pch.h"
#include "imgui.h"


class Hotkey  //represents hotkeys for keyboard, gamepad, and mouse
{
public:
	constexpr static inline int bindingTextLength = 9;

private:
	std::shared_ptr<ActionEvent> mEvent;
	std::string mHotkeyName;
	std::string mBindingText; 
	std::string mBindingTextShort;

	std::vector<std::vector<ImGuiKey>> mBindings{}; // an ImGuiKey can be for either mouse, gamepad, or keyboard. Having a vector of them means we can do combo bindings, ie press "Shift + F1" to activate a hotkey. Also allows us to mix input types, ie "MouseLeft + F1".
	// vector of vectors so user can do more than one set of bindings
public:
	std::string_view getName() const { return mHotkeyName; } // used in serialisation.
	std::string_view getBindingText() const { return mBindingText;  } // a user facing text string of what the hotkey is currently bound to.
	std::string_view getBindingTextShort() const { return mBindingTextShort; } // a user facing text string of what the hotkey is currently bound to.



	const std::vector<std::vector<ImGuiKey>>& getBindings() const { return mBindings; }
	void setBindings(std::vector<std::vector<ImGuiKey>> newBindings) { mBindings = newBindings; mBindingText = generateBindingText(newBindings); mBindingTextShort = generateBindingTextShort(mBindingText);
	}



	
	// actionEvent is what event to fire when hotkey activated. hotkeyName is just used for (de)serialisation. Bindings default to empty (unbound). BindingText and BindingTextShort are used in the GUI.
	explicit Hotkey(std::shared_ptr<ActionEvent> actionEvent, std::string hotkeyName, std::vector<std::vector<ImGuiKey>> defaultBindings = {})
		: mEvent(actionEvent), mHotkeyName(hotkeyName), mBindings(defaultBindings), mBindingText(generateBindingText(defaultBindings)), mBindingTextShort(generateBindingTextShort(mBindingText))
	{}

	void invokeEvent()
	{
		if (!mEvent.get()) PLOG_ERROR << "event not set";
		mEvent.get()->operator()();
	}
	
	//friend std::ostream& operator<< (std::ostream& out, const Hotkey& g) // to string stream, for serialisation
	//{
	//	for (auto binding : g.mBindings)
	//	{
	//		for (auto key : binding)
	//		{
	//			out << (int)key << ", ";
	//		}
	//		out << " - ";
	//	}

	//	return out;
	//}

	static std::string generateBindingText(const std::vector<std::vector<ImGuiKey>>& bindings)
	{
		if (bindings.empty()) return "Unbound";

		std::ostringstream out;

		for (auto& binding : bindings)
		{
			std::ostringstream tempss;
			if (binding.empty())
			{
				PLOG_ERROR << "a binding was empty. this shouldn't happen.";
				return "Unbound?";
			}
			for (auto key : binding)
			{
				tempss << ImGui::GetKeyName(key) << " + ";
			}
			// remove last " + "
			std::string temp1 = tempss.str();
			temp1.resize(temp1.size() - 3);
			out << temp1 << ", ";
		}
		// remove last ", "
		std::string temp2 = out.str();
		temp2.resize(temp2.size() - 2);

		return temp2;
	}

	static std::string generateBindingTextShort(const std::string_view longStringView)
	{
		std::string shortString = longStringView.data();
		if (shortString.length() > bindingTextLength) // truncate
		{
			shortString.resize(bindingTextLength);
			// and replace last chars with ".." so user knows it was cut off
			shortString.replace(shortString.size() - 2, 2, "..");
		}
		return shortString;
	}

	static std::string generateBindingTextSingle(const std::vector<ImGuiKey>& binding)
	{
		std::ostringstream out;
		if (binding.empty())
		{
			return "Unbound";
		}
		for (auto key : binding)
		{
			out << ImGui::GetKeyName(key) << " + ";
		}
		// remove last " + "
		std::string temp = out.str();
		temp.resize(temp.size() - 3);
		return temp;

	}

	// functions for serialising and deserialising are defined in HotkeyManager



};