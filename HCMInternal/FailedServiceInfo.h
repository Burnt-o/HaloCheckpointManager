#pragma once

class FailedServiceInfo
{
private:
	static FailedServiceInfo* instance;

	struct failureInfo
	{
		std::string failedCheatService;
		HCMInitException ex;
	};
	
	std::vector<failureInfo> allFailures;

public:

	FailedServiceInfo()
	{
		if (instance) throw HCMInitException("Cannot have more than one FailedServiceInfo");
		instance = this;
	}

	static void addFailure(std::string failedCheatService, HCMInitException ex)
	{
		failureInfo fail{ failedCheatService, ex };
	instance->allFailures.emplace_back(fail);
	}

	static void printFailures();
};

