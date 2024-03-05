#pragma once
#include "RuntimeExceptionHandler.h"
#include "IMessagesGUI.h"
#include "BinarySetting.h"
#include "ISettingsSerialiser.h"
class SettingsSerialiser : public ISettingsSerialiser
{
private:
	std::string mDirPath;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;
	std::shared_ptr<IMessagesGUI> messagesGUI;


public:
	SettingsSerialiser(std::string dirPath, std::shared_ptr<RuntimeExceptionHandler> exp, std::shared_ptr<IMessagesGUI> mes) 
		: mDirPath(dirPath), runtimeExceptions(exp), messagesGUI(mes) {}
	virtual void serialise(std::vector<std::shared_ptr<SerialisableSetting>>& allSerialisableOptions) override;
	virtual void deserialise(std::vector<std::shared_ptr<SerialisableSetting>>& allSerialisableOptions) override;
};

