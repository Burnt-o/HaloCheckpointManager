#pragma once
#include "pch.h"
#include "imgui.h"
#include "HotkeysEnum.h"
#include <numeric>

template<typename valueType, typename Container>
std::string contents_as_string(Container const& c,
	std::string const& separator, std::function<std::string(valueType)> stringFunctor) {
	if (c.size() == 0) return "";
	auto fold_operation = [&separator, &stringFunctor](std::string const& accum,
		auto const& item) {
			return accum + separator + stringFunctor(item); };
	return std::accumulate(std::next(std::begin(c)), std::end(c),
		stringFunctor(*std::begin(c)), fold_operation);
}


// a set of buttons that all are pressed, triggers the hotkey
using BindingCombo = std::set<ImGuiKey>;

class RebindableHotkey  //represents hotkeys for keyboard, gamepad, and mouse that can be rebound
{
public:
	constexpr static inline int bindingTextLength = 9;
	friend class HotkeyManagerImpl;
	friend class HotkeyManager;

private:

	RebindableHotkeyEnum mHotkeyEnum;
	std::string mBindingText; 
	std::string mBindingTextShort;

	std::vector<BindingCombo> mBindings{}; // an ImGuiKey can be for either mouse, gamepad, or keyboard. Having a vector of them means we can do combo bindings, ie press "Shift + F1" to activate a hotkey. Also allows us to mix input types, ie "MouseLeft + F1".
	// vector of sets so user can do more than one set of bindings






	bool bindingSetDown(const BindingCombo& bindingSet)
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
		explicit RebindableHotkey(RebindableHotkeyEnum hotkeyEnum, std::vector<BindingCombo> defaultBindings = { }, bool ignoresHotkeyDisabler = false)
			:mHotkeyEnum(hotkeyEnum), 
			//mBindings(std::erase_if(defaultBindings, [](const auto& bindingSet) { return bindingSet.empty(); })), // delete empty bindings (this shouldn't happen anyway)
			mBindings(defaultBindings), // delete empty bindings (this shouldn't happen anyway)
			mBindingText(generateBindingText(defaultBindings)), 
			mBindingTextShort(generateBindingTextShort(mBindingText)),
			ignoresHotkeyDisabler(ignoresHotkeyDisabler)
		{
		}

		static inline constexpr  std::vector<std::vector<ImGuiKey>> noBindings() { return { {} }; }

		const bool ignoresHotkeyDisabler;

		bool isCurrentlyDown()
		{
				for (auto& bindingSet : getBindings())
				{
					if (bindingSetDown(bindingSet)) return true;
				}
				return false;
		}

		const std::vector<BindingCombo>& getBindings() const
		{
			return mBindings;
		}

		void setBindings(const std::vector<BindingCombo>& newBindings)
		{
			PLOG_DEBUG << "setBindings called on " << magic_enum::enum_name(mHotkeyEnum);
			mBindings = newBindings;
			mBindingText = generateBindingText(mBindings);
			mBindingTextShort = generateBindingTextShort(mBindingText);
		}


		static std::string generateBindingText(const std::vector<BindingCombo>& bindings)
		{
			if (bindings.empty()) return "Unbound";


			std::vector<std::string> comboStringVec;

			std::function<std::string(ImGuiKey)> ImGuiKeyToString = [](ImGuiKey key) { return std::string(ImGui::GetKeyName(key)); };

			for (auto& bindingCombo : bindings)
			{
				comboStringVec.push_back(contents_as_string(bindingCombo, " + ", ImGuiKeyToString));
			}

			std::function<std::string(std::string)> stringToString = [](auto s) { return s; };

			return contents_as_string(comboStringVec, ", ", stringToString);
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

		static std::string generateBindingTextSingle(const BindingCombo& binding)
		{
			std::ostringstream out;
			if (binding.empty())
			{
				return "Unbound";
			}

			std::function<std::string(ImGuiKey)> ImGuiKeyToString = [](ImGuiKey key) { return std::string(ImGui::GetKeyName(key)); };

			return contents_as_string(binding, " + ", ImGuiKeyToString);

		}

		// functions for serialising and deserialising are defined in HotkeyManager
};

