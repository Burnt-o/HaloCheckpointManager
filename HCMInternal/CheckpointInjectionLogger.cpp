#include "pch.h"
#include "CheckpointInjectionLogger.h"
#include "time.h"


constexpr const char* explanation = "This file logs checkpoint injections so you can more easily recreate theater files and etc.";
constexpr const char* logFileName = "CheckpointInjections.txt";


class CheckpointInjectionLogger::CheckpointInjectionLoggerImpl
{
private:
	static inline bool firstLog = true;
	std::string initTime;
	std::string logFilePath;

public:

	CheckpointInjectionLoggerImpl(std::string dirPath)
	{
		// setup init time string
		std::tm tm = {};
		const time_t t = std::time(nullptr);
		auto err = localtime_s(&tm, &t);
		std::stringstream ss;
		ss << std::put_time(&tm, "%Y-%m-%d %H-%M-%S") << std::endl;
		initTime = ss.str();

		// setup log file 
		logFilePath = dirPath + logFileName;
		if (!fileExists(logFilePath))
		{
			// create the file w/ explanation
			std::ofstream file(logFilePath);
			file << explanation << std::endl;

		}

	}

	void logInjection(SelectedCheckpointData& data)
	{
		std::ofstream file(logFilePath);
		if (firstLog)
		{
			firstLog = false;
			file << "HCM Session started: " << initTime << std::endl;
		}

		// get current time
		std::tm tm = {};
		const time_t t = std::time(nullptr);
		auto err = localtime_s(&tm, &t);
		file << "At time " << std::put_time(&tm, "%Y-%m-%d %H-%M-%S") << " you injected: " << std::endl << data.selectedCheckpointFilePath << std::endl << std::endl;
	}
};




CheckpointInjectionLogger::CheckpointInjectionLogger(std::string dirPath)
{
	pimpl = std::make_unique< CheckpointInjectionLoggerImpl>(dirPath);
}

CheckpointInjectionLogger::~CheckpointInjectionLogger()
{

}

void CheckpointInjectionLogger::logInjection(SelectedCheckpointData& data) { return pimpl->logInjection(data); }