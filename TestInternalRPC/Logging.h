#pragma once



class Logging
{
private:
	static std::string logFileDestination;

	static bool consoleOpen;

public:
	static void initLogging();

	static void closeConsole();
	static void openConsole();

	static void SetConsoleLoggingLevel(plog::Severity newLevel);
	static void SetFileLoggingLevel(plog::Severity newLevel);

	static std::string GetLogFileDestination()
	{
		return logFileDestination;
	}
};

