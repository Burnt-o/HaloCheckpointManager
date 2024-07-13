#include "pch.h"
#include "CommandConsoleGUI.h"
#include "imgui.h"
#include "imgui_stdlib.h"
#include "ScopedImFontScaler.h"


constexpr SimpleMath::Vector4 backgroundColor = { 0, 0, 0, 1 };
constexpr SimpleMath::Vector4 fontColor = { 1, 1, 1, 1 };


void CommandConsoleGUI::clear_buffer()
{

}

void CommandConsoleGUI::clear_history()
{
	mCommandHistory.clear();
	mCurrentIndex = 0;
}


void CommandConsoleGUI::history_cursor_up()
{
	mCurrentIndex = std::clamp<int>(mCurrentIndex + 1, 0, (int)mCommandHistory.size());
}


void CommandConsoleGUI::history_cursor_down()
{
	mCurrentIndex = std::clamp<int>(mCurrentIndex - 1, 0, (int)mCommandHistory.size());
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

	mCommandHistory.push_back(command);

	engineCommand->sendEngineCommand(command);

	mCurrentIndex = 0;

	if (clearBuffer)
	{
		clear_buffer();
	}

}

void CommandConsoleGUI::render_console()
{
	ImGui::AlignTextToFramePadding();
	ImGui::SetNextItemWidth(100);
	ImGui::Text("mcctas(");
	ImGui::SameLine();

	if (!ImGui::IsAnyItemActive())
		ImGui::SetKeyboardFocusHere();

	// Fill remaining space with console input
	float command_line_width = std::max<float>(static_cast<float>(ImGui::GetContentRegionAvail().x), 720.0f);
	ImGui::SetNextItemWidth(command_line_width);
	ImGuiInputTextFlags command_line_flags = ImGuiInputTextFlags_EnterReturnsTrue;
	if (ImGui::InputText("##mCommandBuffer", &mCommandBuffer, command_line_flags)) {
		execute(true);
	}
}


void CommandConsoleGUI::render_console_output()
{
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
	ImGui::BeginChild("ScrollingRegion##CommandConsoleGUI", ImVec2(720, 300), false, flags);


	for (auto& s : mLog) {
		ImGui::Text(s.c_str());
	}

	ImGui::EndChild();
}

void CommandConsoleGUI::render_history()
{
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
	ImGui::BeginChild("ScrollingRegion##CommandConsoleGUI", ImVec2(720, 300), false, flags);

	int id = 0;
	for (auto s = mCommandHistory.cbegin(); s != mCommandHistory.cend(); s = std::next(s)) {
		ImGui::PushID(id);
		if (ImGui::Button("<")) {
			if (mCurrentIndex >= 1 && mCommandHistory.size() > id) {
				mCommandBuffer = mCommandHistory[id];
			}
		}
		ImGui::SameLine();
		ImGui::AlignTextToFramePadding();
		ImGui::Text(s->c_str());
		ImGui::PopID();
		id++;
	}

	ImGui::SetScrollHereY(1.0f);

	ImGui::EndChild();
}




void CommandConsoleGUI::render(SimpleMath::Vector2 screenSize)
{

#ifndef HCM_DEBUG 
#error "check these values"
#endif
	const float PADDING = 10.0f;
	const float MARGIN = 2.0f;
	const float CONSOLE_HEIGHT = 40.0f;



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
	ImVec2 console_output_window_pos = ImVec2(PADDING, io.DisplaySize.y - PADDING - CONSOLE_HEIGHT - MARGIN - 300 - MARGIN);
	ImGui::SetNextWindowPos(console_output_window_pos, ImGuiCond_Always, window_pos_pivot);
	ImGui::SetNextWindowBgAlpha(0.5f); // Transparent background
	if (ImGui::Begin("MCCTAS Console Output##CommandConsoleGUI", nullptr, window_flags)) {
		render_console_output();
	}
	ImGui::End();


	// History Window
	ImVec2 history_window_pos = ImVec2(PADDING, io.DisplaySize.y - PADDING - CONSOLE_HEIGHT - MARGIN);
	ImGui::SetNextWindowPos(history_window_pos, ImGuiCond_Always, window_pos_pivot);
	ImGui::SetNextWindowBgAlpha(0.25f); // Transparent background
	if (ImGui::Begin("MCCTAS Console History##CommandConsoleGUI", nullptr, window_flags)) {
		render_history();
	}
	ImGui::End();

	// Console input window
	ImVec2 console_window_pos = ImVec2(PADDING, io.DisplaySize.y - PADDING);
	ImGui::SetNextWindowPos(console_window_pos, ImGuiCond_Always, window_pos_pivot);
	ImGui::SetNextWindowSize(ImVec2(screenSize.y - PADDING * 2, CONSOLE_HEIGHT));
	ImGui::SetNextWindowBgAlpha(0.25f); // Transparent background
	if (ImGui::Begin("MCCTAS Console##CommandConsoleGUI", nullptr, window_flags))
	{
		render_console();
		
	}
	ImGui::End();

	// Cleanup
	if (color_style_count)
		ImGui::PopStyleColor(color_style_count);
}

void CommandConsoleGUI::onCommandOutput(const std::string& outputString)
{
	mLog.push_back(outputString);
}