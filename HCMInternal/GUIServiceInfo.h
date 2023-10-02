#pragma once
#include "MessagesGUI.h"
#include "GuiElementEnum.h"
#include "GameState.h"
class GUIServiceInfo
{
private:
	std::shared_ptr<MessagesGUI> messagesGUI;
	std::vector<std::string> failureMessages;
public:
	GUIServiceInfo(std::shared_ptr<MessagesGUI> mes) : messagesGUI(mes) {}
	void addFailure(GUIElementEnum guielementenum, GameState game, std::vector<HCMInitException> serviceFailures);
	void addFailure(GUIElementEnum guielementenum, GameState game, HCMInitException miscFailure);
	void printFailures()
	{
		for (auto& message : failureMessages)
		{
			messagesGUI->addMessage(message);
		}
	}
};

