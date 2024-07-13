#include "pch.h"
#include "CommandConsoleGUI.h"
#include "imgui.h"
#include "imgui_stdlib.h"
#include "ScopedImFontScaler.h"


constexpr SimpleMath::Vector4 backgroundColor = { 0, 0, 0, 1 };
constexpr SimpleMath::Vector4 fontColor = { 1, 1, 1, 1 };


void CommandConsoleGUI::clear_buffer()
{
	mCommandBuffer.clear();
}

void CommandConsoleGUI::clear_history()
{
	mConsoleRecords.clear();
}




void CommandConsoleGUI::execute(bool clearBuffer)
{
	std::string command(mCommandBuffer);
	if (command.length() <= 0) {
		return;
	}

	// Remove newlines from enter press
	command.erase(std::remove(command.begin(), command.end(), '\n'), command.end());
	command.erase(std::remove(command.begin(), command.end(), '\r'), command.end());

	mConsoleRecords.push_back(ConsoleRecord(command, ConsoleRecord::Type::Command));

	engineCommand->sendEngineCommand(command);


	if (clearBuffer)
	{
		clear_buffer();
	}

}

void CommandConsoleGUI::render_console_input()
{
	ImGui::AlignTextToFramePadding();
	ImGui::SetNextItemWidth(100);
	ImGui::Text("Engine(");
	ImGui::SameLine();

	if (!ImGui::IsAnyItemActive())
		ImGui::SetKeyboardFocusHere();

	// Fill remaining space with console input
	float command_line_width = std::max<float>(static_cast<float>(ImGui::GetContentRegionAvail().x), 720.0f);
	ImGui::SetNextItemWidth(command_line_width);
	ImGuiInputTextFlags command_line_flags = ImGuiInputTextFlags_EnterReturnsTrue;
	if (ImGui::InputText("##mCommandBuffer", &mCommandBuffer, command_line_flags)) {
		execute(true);
		needToScrollToBottom = true;
	}
}


template<typename T>
void pop_front(std::vector<T>& vec)
{
	assert(!vec.empty());
	vec.erase(vec.begin());
}

void CommandConsoleGUI::render_console_output()
{

	while (mConsoleRecords.size() > mMaxRecordHistory)
		pop_front(mConsoleRecords);

	ImGui::BeginChild("ScrollingRegion##CommandConsoleGUI", ImVec2(720, 300), false);

	int commandID = 0;
	for (auto& record : mConsoleRecords) {
		switch (record.type)
		{
		case ConsoleRecord::Type::Command:
			ImGui::PushID(commandID);
			if (ImGui::Button("<"))
			{
				mCommandBuffer = record.str;
			}
			ImGui::SameLine();
			ImGui::AlignTextToFramePadding();
			ImGui::Text(record.str.c_str());
			ImGui::PopID();
			commandID++;
			break;

		case ConsoleRecord::Type::Output:
			ImGui::Text(record.str.c_str());
			break;

		case ConsoleRecord::Type::Error:
			ImGui::TextColored({ 1, 0, 0, 1 }, record.str.c_str());
			break;
		}

		
	}

	if (needToScrollToBottom)
	{
		needToScrollToBottom = false;
		ImGui::SetScrollHereY(1.0f);
	}

	ImGui::EndChild();
}



void CommandConsoleGUI::render(SimpleMath::Vector2 screenSize)
{

#ifndef HCM_DEBUG 
#error "check these values"
#endif
	float PADDING = 10.0f;
	const float MARGIN = 2.0f;
	const float CONSOLE_HEIGHT = 40.0f * (fontSize / 16.f);



	ImGuiIO& io = ImGui::GetIO();

	int color_style_count = 0;
	ImGui::PushStyleColor(ImGuiCol_FrameBg, backgroundColor); color_style_count++;
	ImGui::PushStyleColor(ImGuiCol_Text, fontColor); color_style_count++;

	ScopedImFontScaler fontScale(fontSize);
#ifndef HCM_DEBUG 
#error "how does fontsize interact with window heights?"
#endif

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;
	ImVec2 window_pos_pivot = ImVec2(0.0f, 1.0f);

	// Console Output Window
	ImVec2 console_output_window_pos = ImVec2(PADDING, io.DisplaySize.y - PADDING - CONSOLE_HEIGHT - MARGIN);
	ImGui::SetNextWindowPos(console_output_window_pos, ImGuiCond_Always, window_pos_pivot);
	ImGui::SetNextWindowBgAlpha(0.5f); // Transparent background
	if (ImGui::Begin("MCCTAS Console Output##CommandConsoleGUI", nullptr, window_flags)) {
		render_console_output();
	}
	ImGui::End();


	// Console input window
	ImVec2 console_window_pos = ImVec2(PADDING, io.DisplaySize.y - PADDING);
	ImGui::SetNextWindowPos(console_window_pos, ImGuiCond_Always, window_pos_pivot);
	ImGui::SetNextWindowSize(ImVec2(screenSize.x - PADDING * 2, CONSOLE_HEIGHT));
	ImGui::SetNextWindowBgAlpha(0.25f); // Transparent background
	if (ImGui::Begin("MCCTAS Console##CommandConsoleGUI", nullptr, window_flags))
	{
		render_console_input();
		
	}
	ImGui::End();

	// Cleanup
	if (color_style_count)
		ImGui::PopStyleColor(color_style_count);




}

void CommandConsoleGUI::onCommandOutput(const std::string& outputString)
{
	mConsoleRecords.push_back(ConsoleRecord(outputString, ConsoleRecord::Type::Output));
}

void CommandConsoleGUI::onCommandError(const std::string& outputString)
{
	mConsoleRecords.push_back(ConsoleRecord(outputString, ConsoleRecord::Type::Error));
}