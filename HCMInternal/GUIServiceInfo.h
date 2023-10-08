#pragma once
#include "IMessagesGUI.h"
#include "GuiElementEnum.h"
#include "GameState.h"
class GUIServiceInfo
{
private:
	std::shared_ptr<IMessagesGUI> messagesGUI;
	std::map<std::pair<GameState, GUIElementEnum>, std::string> failureMessagesMap; 
public:
	GUIServiceInfo(std::shared_ptr<IMessagesGUI> mes) : messagesGUI(mes) {}
	void addFailure(GUIElementEnum guielementenum, GameState game, std::vector<HCMInitException> serviceFailures);
	void addFailure(GUIElementEnum guielementenum, GameState game, HCMInitException miscFailure);
	void printAllFailures();
	std::string getFailure(GUIElementEnum guielementenum, GameState game)
	{
		std::pair<GameState, GUIElementEnum> gameElePair(game, guielementenum);
		if (!failureMessagesMap.contains(gameElePair)) return "No failure message stored!";
		return failureMessagesMap.at(gameElePair);
	}
};

