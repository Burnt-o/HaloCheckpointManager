#include "pch.h"
#include "FailedServiceInfo.h"
#include "MessagesGUI.h"

FailedServiceInfo* FailedServiceInfo::instance = nullptr;

void FailedServiceInfo::printFailures()
{
	if (!instance)
	{
		PLOG_ERROR << "no instance";
		return;
	}

	if (!instance->allGUIFailures.empty())
	{
		MessagesGUI::addMessage("Some cheats failed to load.");
	}

	for (auto GUIFail : instance->allGUIFailures)
	{
		std::string nameOfFailedGUIElement = GameStateToString.at(GUIFail.game) + "::" + std::string{GUIFail.pGetName()};
		std::string myFailedServices;
		for (auto ser : GUIFail.namesOfFailedRequiredServices)
		{
			myFailedServices += ser + ", ";
		}

		// cleanup the last ", "
		if (myFailedServices.empty()) myFailedServices.pop_back();
		if (myFailedServices.empty()) myFailedServices.pop_back();

		MessagesGUI::addMessage(std::format("Cheat {} may not work properly due to dependent services failing initialization. \nFailed Services: {}", nameOfFailedGUIElement, myFailedServices));
	}

	if (!instance->allServiceFailures.empty())
	{
		MessagesGUI::addMessage("Some services failed to initialize.");
	}

	for (auto serviceFail : instance->allServiceFailures)
	{
		MessagesGUI::addMessage(std::format("Service {} failed initialization. \nError: {}", serviceFail.nameOfFailedCheatService, serviceFail.ex.what()));
	}
}