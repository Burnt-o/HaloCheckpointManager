#include "pch.h"
#include "CommandConsoleGUI.h"
#include "imgui.h"
#include "imgui_stdlib.h"
#include "ScopedImFontScaler.h"


constexpr SimpleMath::Vector4 backgroundColor = { 0, 0, 0, 1 };
constexpr SimpleMath::Vector4 fontColorCommand = { 1.00f, 0.80f, 0.40f, 1.00f };
constexpr SimpleMath::Vector4 fontColorOutput= { 0, 0.75, 0.75, 1 };
constexpr SimpleMath::Vector4 fontColorError = { 1, 0, 0, 1 };
constexpr SimpleMath::Vector4 fontColorConsole = fontColorCommand;


template<typename T>
void pop_front(std::vector<T>& vec)
{
	assert(!vec.empty());
	vec.erase(vec.begin());
}

void CommandConsoleGUI::clear_buffer()
{
	mCommandBuffer.clear();
	queuedBufferUpdate = std::nullopt;
}

void CommandConsoleGUI::clear_history()
{
	mConsoleRecords.clear();
	mCommandHistory.clear();
}

void CommandConsoleGUI::set_buffer(const std::string& str)
{
	queuedBufferUpdate = str;
}


void CommandConsoleGUI::navigateToHistoryOffset(int amount)
{
	while (mCommandHistory.size() > mMaxStoredRecords)
		pop_front(mCommandHistory);


	PLOG_DEBUG << "navigateToHistoryOffset, amount: " << amount;
	if (!mCommandHistory.empty())
	{
		if (!mCommandHistoryNavigationIndex)
		{
			mCommandHistoryNavigationIndex = mCommandHistory.size() - 1;
		}
		else
		{
			mCommandHistoryNavigationIndex.value() += amount;
			mCommandHistoryNavigationIndex.value() = std::clamp(mCommandHistoryNavigationIndex.value(), 0, (int)mCommandHistory.size() - 1);
		}

		PLOG_DEBUG << "setting buffer to index " << mCommandHistoryNavigationIndex.value() << ", value: " << mCommandHistory.at(mCommandHistoryNavigationIndex.value());
		set_buffer(mCommandHistory.at(mCommandHistoryNavigationIndex.value()));
	}

}

void CommandConsoleGUI::pasteFromClipboard()
{
	PLOG_DEBUG << "pasting clipboard";
	// appends to buffer
	set_buffer(std::format("{}{}", mCommandBuffer,ImGui::GetClipboardText()));
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
	mCommandHistory.push_back(command);
	engineCommand->sendEngineCommand(command);


	if (clearBuffer)
	{
		clear_buffer();
	}

	mCommandHistoryNavigationIndex = std::nullopt;
	needToScrollOutputToBottom = true;

}

// We cast user data to CommandConsole [this] ptr. That's just how imgui callbacks work.
int inputTextCallback(ImGuiInputTextCallbackData* data)
{
	auto* thisPointer = static_cast<CommandConsoleGUI*>(data->UserData);

	if (thisPointer->queuedBufferUpdate.has_value())
	{
		// replace the buffer with the queued value
		data->DeleteChars(0, data->BufTextLen);
		data->InsertChars(0, thisPointer->queuedBufferUpdate.value().data());
		thisPointer->queuedBufferUpdate = std::nullopt;

		// cursor pos to end (rightmost)
		data->CursorPos = data->SelectionEnd;
	}
	
	return 0;
}

void CommandConsoleGUI::render_console_input()
{
	ImGui::AlignTextToFramePadding();
	ImGui::SetNextItemWidth(100);
	ImGui::Text("HaloScript: ");
	ImGui::SameLine();

	if (!ImGui::IsAnyItemActive())
	{
		ImGui::SetKeyboardFocusHere();
	}

	// Fill remaining space with console input
	float command_line_width = std::max<float>(static_cast<float>(ImGui::GetContentRegionAvail().x), 720.0f);
	ImGui::SetNextItemWidth(command_line_width);
	ImGuiInputTextFlags command_line_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackAlways;
	ImGui::PushStyleColor(ImGuiCol_Text, fontColorConsole); 

	if (ImGui::InputText("##mCommandBuffer", &mCommandBuffer, command_line_flags, &inputTextCallback, this))
	{
		execute(true);
	}

	ImGui::PopStyleColor();
}




void CommandConsoleGUI::render_console_output()
{

	while (mConsoleRecords.size() > mMaxStoredRecords)
		pop_front(mConsoleRecords);

	ImGui::BeginChild("ScrollingRegion##CommandConsoleGUI");

	int commandID = 0;
	for (auto& record : mConsoleRecords) {
		switch (record.type)
		{
		case ConsoleRecord::Type::Command:
			ImGui::PushID(commandID);
			if (ImGui::Button("<"))
			{
				set_buffer(record.str);
			}
			ImGui::SameLine();
			ImGui::AlignTextToFramePadding();
			ImGui::TextColored(fontColorCommand, record.str.c_str());
			ImGui::PopID();
			commandID++;
			break;

		case ConsoleRecord::Type::Output:
			ImGui::TextColored(fontColorOutput, record.str.c_str()); // aqua
			break;

		case ConsoleRecord::Type::Error:
			ImGui::TextColored(fontColorError, record.str.c_str()); // red error text
			break;
		}

		
	}

	if (needToScrollOutputToBottom)
	{
		needToScrollOutputToBottom = false;
		ImGui::SetScrollHereY(1.0f);
	}

	ImGui::EndChild();
}



void CommandConsoleGUI::render(SimpleMath::Vector2 screenSize)
{

	processInputs();


	float PADDING = 10.0f;
	const float MARGIN = 2.0f;
	const float CONSOLE_HEIGHT = 40.0f * (fontSize / 16.f);


	int color_style_count = 0;
	ImGui::PushStyleColor(ImGuiCol_FrameBg, backgroundColor); ;

	ScopedImFontScaler fontScale(fontSize);

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;
	ImVec2 window_pos_pivot = ImVec2(0.0f, 1.0f);

	// Console Output Window
	ImVec2 console_output_window_pos = ImVec2(PADDING, screenSize.y - PADDING - CONSOLE_HEIGHT - MARGIN);
	ImGui::SetNextWindowPos(console_output_window_pos, ImGuiCond_Always, window_pos_pivot);
	ImGui::SetNextWindowSize(ImVec2(screenSize.x - PADDING * 2, (screenSize.y / 3.f) - CONSOLE_HEIGHT - PADDING - MARGIN)); // 1/3rd vertically
	ImGui::SetNextWindowBgAlpha(0.5f); // Transparent background
	if (ImGui::Begin("HaloScript Console Output##CommandConsoleGUI", nullptr, window_flags)) {
		render_console_output();
	}
	ImGui::End();


	// Console input window
	ImVec2 console_window_pos = ImVec2(PADDING, screenSize.y - PADDING);
	ImGui::SetNextWindowPos(console_window_pos, ImGuiCond_Always, window_pos_pivot);
	ImGui::SetNextWindowSize(ImVec2(screenSize.x - PADDING * 2, CONSOLE_HEIGHT));
	ImGui::SetNextWindowBgAlpha(0.25f); // Transparent background
	if (ImGui::Begin("HaloScript Console##CommandConsoleGUI", nullptr, window_flags))
	{
		render_console_input();
		
	}
	ImGui::End();

	// Cleanup
	ImGui::PopStyleColor();




}

void CommandConsoleGUI::onCommandOutput(const std::string& outputString)
{
	mConsoleRecords.push_back(ConsoleRecord(outputString, ConsoleRecord::Type::Output));
}

void CommandConsoleGUI::onCommandError(const std::string& outputString)
{
	mConsoleRecords.push_back(ConsoleRecord(outputString, ConsoleRecord::Type::Error));
}

void CommandConsoleGUI::processInputs()
{
	// check for up arrow, down arrow, and right click. We only want to fire on press (from up to down)
	static bool downArrowWasDownLastFrame = false;
	static bool upArrowWasDownLastFrame = false;


	bool downArrowIsDownThisFrame = ImGui::IsKeyDown(ImGuiKey_DownArrow);
	bool upArrowIsDownThisFrame = ImGui::IsKeyDown(ImGuiKey_UpArrow);

	bool rightClickPressedThisFrame = ImGui::IsMouseClicked(ImGuiMouseButton_Right, false); 
	bool downArrowPressedThisFrame = downArrowIsDownThisFrame && !downArrowWasDownLastFrame;
	bool upArrowPressedThisFrame = upArrowIsDownThisFrame && !upArrowWasDownLastFrame;

	upArrowWasDownLastFrame = upArrowIsDownThisFrame;
	downArrowWasDownLastFrame = downArrowIsDownThisFrame;

	if (rightClickPressedThisFrame)
		pasteFromClipboard();

	if (downArrowPressedThisFrame)
		navigateToHistoryOffset(1);

	if (upArrowPressedThisFrame)
		navigateToHistoryOffset(-1);
}