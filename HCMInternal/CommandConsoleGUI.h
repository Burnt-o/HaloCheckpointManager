#pragma once
#include "pch.h"
#include "ScopedServiceRequest.h"
#include "GameState.h"
#include "EngineCommand.h"
#include "UnarySetting.h"
#include "HaloScriptOutputHookEvent.h"
// Credit to Scales for letting me rip off his console implementation here https://github.com/Scaless/HaloTAS/blob/b9d55d2fe2aff2a1a654984d4d909b5cd14b8780/HaloTAS/MCCTAS/tas_console.h


#ifndef HCM_DEBUG
#error Need to add a destructor guard or something i think - rn the ScopedImFontScaler is getting fubared if hcm shutdown while console open
#endif


struct ConsoleRecord
{
	std::string str;
	enum class Type
	{
		Command,
		Output,
		Error
	}type;
	
};


class CommandConsoleGUI
{
private:
	std::shared_ptr<EngineCommand> engineCommand;

	bool needToScrollOutputToBottom = false; // set true on execute() to scroll output window to bottom
	std::optional<std::string> queuedBufferUpdate; // when manually modifying the mCommandBuffer we need to do it in the inputTextCallback; put the new value here.

	std::string mCommandBuffer; // current console input string bound to by Imgui::InputText
	std::vector<ConsoleRecord> mConsoleRecords; // console output strings
	const int mMaxStoredRecords = 40;

	std::vector<std::string> mCommandHistory; // previousuly inputed commands
	std::optional<int> mCommandHistoryNavigationIndex;
	const int mMaxStoredCommands = 20;

	void processInputs();

	void set_buffer(const std::string& str);
	void clear_buffer();
	void clear_history();

	void navigateToHistoryOffset(int amount);
	void pasteFromClipboard();

	void render_console_input();
	void render_console_output();

	// see cpp
	friend int inputTextCallback(ImGuiInputTextCallbackData* data);

public:
	float fontSize; // CommandConsoleManager will update this whenever the user changes the font size setting
	void render(SimpleMath::Vector2 screenSize); // CommandConsoleManager sets callback to render event that invokes this
	void onHaloScriptOutput(const std::string& outputString, const HSOutputType& outputType); // CommandConsoleManager sets callback to console output event that invokes this

	void execute(bool clearBuffer); // CommandConsoleManager sets callback to execute event that invokes this. Also called by CommandConsoleGUI if user presses enter.

	CommandConsoleGUI(
		std::shared_ptr<EngineCommand> engineCommand, 
		float fontSize)
		: engineCommand(engineCommand),
		 fontSize(fontSize)
	{
	}
};