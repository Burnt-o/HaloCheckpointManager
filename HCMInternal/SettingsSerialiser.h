#pragma once
#include "RuntimeExceptionHandler.h"
#include "MessagesGUI.h"
#include "Setting.h"
class SettingsSerialiser
{
private:
	std::string mDirPath;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;
	std::shared_ptr<MessagesGUI> messagesGUI;


public:
	SettingsSerialiser(std::string dirPath, std::shared_ptr<RuntimeExceptionHandler> exp, std::shared_ptr<MessagesGUI> mes) 
		: mDirPath(dirPath), runtimeExceptions(exp), messagesGUI(mes) {}
	void serialise(std::vector<std::shared_ptr<SerialisableSetting>>& allSerialisableOptions);
	void deserialise(std::vector<std::shared_ptr<SerialisableSetting>>& allSerialisableOptions);
};

