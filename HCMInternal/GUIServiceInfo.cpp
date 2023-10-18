#include "pch.h"
#include "GUIServiceInfo.h"

// defining how to convert a failure into a string message to print at the user

void GUIServiceInfo::addFailure(GUIElementEnum guielementenum, GameState game, std::vector<HCMInitException> serviceFailures)
{
	std::string errorMessage;
	for (auto& ex : serviceFailures)
	{
		errorMessage.append(ex.what());
	}

	failureMessagesMap.emplace(std::pair<GameState, GUIElementEnum>{game, guielementenum}, errorMessage);
}
void GUIServiceInfo::addFailure(GUIElementEnum guielementenum, GameState game, HCMInitException miscFailure)
{
	failureMessagesMap.emplace(std::pair<GameState, GUIElementEnum>{game, guielementenum}, miscFailure.what());
}



void GUIServiceInfo::printAllFailures()
{
	for (auto& [gameElementPair, errorMessage] : failureMessagesMap)
	{
		messagesGUI->addMessage(std::format("{0}::{1} failed: {2}", gameElementPair.first.toString(), magic_enum::enum_name(gameElementPair.second), errorMessage));
	}
}

