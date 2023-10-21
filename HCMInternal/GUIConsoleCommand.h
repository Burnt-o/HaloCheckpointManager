#pragma once
#include "IGUIElement.h"
#include "SettingsStateAndEvents.h"
class GUIConsoleCommand : public IGUIElement {

private:
	std::shared_ptr<ActionEvent> mEventToFire;
	std::string mHotkeyButtonLabelId = std::format("..###{}", getName());
	std::weak_ptr<SettingsStateAndEvents> mSettingsWeak;
	std::vector<std::thread> mFireEventThreads;
public:

	GUIConsoleCommand(GameState implGame, std::optional<HotkeysEnum> hotkey, std::shared_ptr<SettingsStateAndEvents> settings)
		: IGUIElement(implGame, hotkey), mSettingsWeak(settings)
	{

		PLOG_VERBOSE << "Constructing GUIConsoleCommand, name: " << getName();
		PLOG_DEBUG << "&mEventToFire: " << std::hex << &mEventToFire;
		this->currentHeight = 20;
	}

	void render(HotkeyRenderer& hotkeyRenderer) override
	{
		auto mSettings = mSettingsWeak.lock();
		if (!mSettings)
		{
			PLOG_ERROR << "bad mSettings weakptr when rendering " << getName();
			return;
		}

		static char commandString[255];
		ImGui::Text("Console command: ");
		ImGui::SameLine();

		ImGui::SetNextItemWidth(100);
		if (ImGui::InputText("##Command", commandString, 255, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			mSettings->consoleCommandString->GetValue() = commandString;
			PLOG_VERBOSE << "GUIConsoleCommand firing event, command: " << commandString;

			auto& newThread = mFireEventThreads.emplace_back(std::thread([mEvent = mSettings->consoleCommandEvent]() {mEvent->operator()(); }));
			newThread.detach();
		}
		ImGui::SameLine();
		if (ImGui::Button("Send"))
		{
			mSettings->consoleCommandString->GetValue() = commandString;
			PLOG_VERBOSE << "GUIConsoleCommand firing event, command: " << commandString;
			auto& newThread = mFireEventThreads.emplace_back(std::thread([mEvent = mSettings->consoleCommandEvent]() {mEvent->operator()(); }));
			newThread.detach();
		}


	}
	~GUIConsoleCommand()
	{
		for (auto& thread : mFireEventThreads)
		{
			if (thread.joinable())
			{
				PLOG_DEBUG << getName() << "joining mFireEventThread";
				thread.join();
				PLOG_DEBUG << getName() << "FireEventThread finished";
			}

		}
	}


	std::string_view getName() override { return nameof(GUIConsoleCommand); }

};