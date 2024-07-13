#pragma once
#include "pch.h"
#include "ScopedServiceRequest.h"
#include "GameState.h"
#include "EngineCommand.h"
#include "UnarySetting.h"

// Credit to Scales for letting me rip off his console implementation here https://github.com/Scaless/HaloTAS/blob/b9d55d2fe2aff2a1a654984d4d909b5cd14b8780/HaloTAS/MCCTAS/tas_console.h


#ifndef HCM_DEBUG
#error Need to add a destructor guard or something i think - rn the ScopedImFontScaler is getting fubared if hcm shutdown while console open
#endif

class CommandConsoleGUI
{
private:
	std::shared_ptr<EngineCommand> engineCommand;



	std::string mCommandBuffer;
	std::vector<std::string> mCommandHistory;
	std::vector<std::string> mLog;
	int mVisibleHistoryLines = 4;
	int mCurrentIndex = 0;


	void clear_buffer();
	void clear_history();
	void history_cursor_up();
	void history_cursor_down();

	void render_console();
	void render_console_output();
	void render_history();

public:
	float fontSize; // CommandConsoleManager will update this whenever the user changes the font size setting
	void render(SimpleMath::Vector2 screenSize); // CommandConsoleManager sets callback to render event that invokes this
	void onCommandOutput(const std::string& outputString); // CommandConsoleManager sets callback to console output event that invokes this
	void execute(bool clearBuffer); // CommandConsoleManager sets callback to execute event that invokes this. Also called by CommandConsoleGUI if user presses enter.

	CommandConsoleGUI(
		std::shared_ptr<EngineCommand> engineCommand, 
		float fontSize)
		: engineCommand(engineCommand),
		 fontSize(fontSize)
	{
	}
};