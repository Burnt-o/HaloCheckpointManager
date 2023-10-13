#pragma once
#include "IGUIElement.h"
#include "SettingsStateAndEvents.h"
class GUIConsoleCommand : public IGUIElement {

private:
	std::shared_ptr<ActionEvent> mEventToFire;
	std::string mHotkeyButtonLabelId = std::format("..###{}", getName());
	gsl::not_null<std::shared_ptr<SettingsStateAndEvents>> mSettings;
public:

	GUIConsoleCommand(GameState implGame, std::optional<HotkeysEnum> hotkey, std::shared_ptr<SettingsStateAndEvents> settings)
		: IGUIElement(implGame, hotkey), mSettings(settings)
	{

		PLOG_VERBOSE << "Constructing GUIConsoleCommand, name: " << getName();
		PLOG_DEBUG << "&mEventToFire: " << std::hex << &mEventToFire;
		this->currentHeight = 20;
	}

	void render(HotkeyRenderer& hotkeyRenderer) override
	{
		static char commandString[255];
		ImGui::Text("Console command: ");
		ImGui::SameLine();

		ImGui::SetNextItemWidth(100);
		if (ImGui::InputText("##Command", commandString, 255, ImGuiInputTextFlags_EnterReturnsTrue))
		{
				mSettings->consoleCommandString->GetValue() = commandString;
		PLOG_VERBOSE << "GUIConsoleCommand firing event, command: " << commandString;
			mSettings->consoleCommandEvent->operator()();
		}
		ImGui::SameLine();
		if (ImGui::Button("Send"))
		{
			mSettings->consoleCommandString->GetValue() = commandString;
			PLOG_VERBOSE << "GUIConsoleCommand firing event, command: " << commandString;
			mSettings->consoleCommandEvent->operator()();
		}


	}

	std::string_view getName() override { return nameof(GUIConsoleCommand); }

};