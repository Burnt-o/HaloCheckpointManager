#pragma once
#include "HaloEnums.h"

class FailedServiceInfo
{
private:
	static FailedServiceInfo* instance;

	struct serviceFailure
	{
		std::string nameOfFailedCheatService;
		HCMInitException ex;
	};

	struct GUIFailure
	{
		GameState game;
		std::function<std::string_view()> pGetName;
		std::vector<std::string> namesOfFailedRequiredServices;
	};
	
	std::vector<serviceFailure> allServiceFailures;
	std::vector<GUIFailure> allGUIFailures;

public:

	FailedServiceInfo()
	{
		if (instance) throw HCMInitException("Cannot have more than one FailedServiceInfo");
		instance = this;
	}

	static void addServiceFailure(std::string failedCheatService, HCMInitException ex)
	{
		serviceFailure fail{ failedCheatService, ex };
	instance->allServiceFailures.emplace_back(fail);
	}

	static void addGUIFalure(GameState game, std::function<std::string_view()> pGetName, std::vector<std::string> namesOfFailedRequiredServices)
	{
		GUIFailure fail{ game, pGetName, namesOfFailedRequiredServices };
		instance->allGUIFailures.emplace_back(fail);

	}

	static void printFailures();
};

