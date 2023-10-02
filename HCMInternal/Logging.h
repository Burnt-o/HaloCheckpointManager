#pragma once


// Sets up console and/or file logging 
class Logging
{
private:
	std::string logFileDestination = "";

	bool consoleOpen = false;

public:
	void initConsoleLogging();
	void initFileLogging(std::string dirPath);

	void closeConsole();
	void openConsole();

	void SetConsoleLoggingLevel(plog::Severity newLevel);
	void SetFileLoggingLevel(plog::Severity newLevel);

	std::string GetLogFileDestination()
	{
		return logFileDestination;
	}
};

