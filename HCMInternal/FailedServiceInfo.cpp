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

	for (auto fail : instance->allFailures)
	{
		MessagesGUI::addMessage(std::format("Service {} failed initialized, some cheats will be unavailable. \nError: {}", fail.failedCheatService, fail.ex.what()));
	}
}