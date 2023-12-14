#pragma once
#include "pch.h"
#include "imgui.h"
#include "HotkeysEnum.h"




class RebindableHotkey  //represents hotkeys for keyboard, gamepad, and mouse that can be rebound
{
public:
	constexpr static inline int bindingTextLength = 9;
	friend class HotkeyManagerImpl;
	friend class HotkeyManager;
	friend class HotkeyRendererImpl;
private:

	RebindableHotkeyEnum mHotkeyEnum;
	std::string mBindingText; 
	std::string mBindingTextShort;

	std::vector<std::vector<ImGuiKey>> mBindings{}; // an ImGuiKey can be for either mouse, gamepad, or keyboard. Having a vector of them means we can do combo bindings, ie press "Shift + F1" to activate a hotkey. Also allows us to mix input types, ie "MouseLeft + F1".
	// vector of vectors so user can do more than one set of bindings



	const std::vector<std::vector<ImGuiKey>>& getBindings() 
	{ 
		return mBindings; 
	}
	void setBindings(const std::vector<std::vector<ImGuiKey>>& newBindings) 
	{ 
		PLOG_DEBUG << "setBindings called on " << magic_enum::enum_name(mHotkeyEnum);
		mBindings = newBindings; 
		mBindingText = generateBindingText(mBindings);
		mBindingTextShort = generateBindingTextShort(mBindingText);
	}


	

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


	bool bindingSetDown(const std::vector<ImGuiKey>& bindingSet)
	{
		if (bindingSet.empty()) return false;

		for (auto key : bindingSet)
		{
			if (!ImGui::IsKeyDown(key)) return false;
		}
		return true;
	}

	public:
		std::string_view getName() const { return magic_enum::enum_name(mHotkeyEnum); } // used in serialisation.
		std::string_view getBindingText() const { return mBindingText; } // a user facing text string of what the hotkey is currently bound to.
		std::string_view getBindingTextShort() const { return mBindingTextShort; } // a user facing text string of what the hotkey is currently bound to.

		// actionEvent is what event to fire when hotkey activated. hotkeyName is just used for (de)serialisation. Bindings default to empty (unbound). BindingText and BindingTextShort are used in the GUI.
		explicit RebindableHotkey(RebindableHotkeyEnum hotkeyEnum, std::vector<std::vector<ImGuiKey>> defaultBindings = { })
			:mHotkeyEnum(hotkeyEnum), 
			//mBindings(std::erase_if(defaultBindings, [](const auto& bindingSet) { return bindingSet.empty(); })), // delete empty bindings (this shouldn't happen anyway)
			mBindings(defaultBindings), // delete empty bindings (this shouldn't happen anyway)
			mBindingText(generateBindingText(defaultBindings)), 
			mBindingTextShort(generateBindingTextShort(mBindingText))
		{
		}

		static inline constexpr  std::vector<std::vector<ImGuiKey>> noBindings() { return { {} }; }


		bool isCurrentlyDown()
		{
				for (auto& bindingSet : getBindings())
				{
					if (bindingSetDown(bindingSet)) return true;
				}
				return false;
		}

};

