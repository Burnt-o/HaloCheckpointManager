#pragma once

#include "Option.h"


class RuntimeExceptionHandler
{
private:
	// private singleton
	static RuntimeExceptionHandler* instance;


public:

	// Shows error message to user, and disables related toggle options if any were passed to it (optional)
	static void handleSilent(HCMExceptionBase& ex, std::vector<Option<bool>*> optionsToDisable = {});
	static void handleMessage(HCMExceptionBase& ex, std::vector<Option<bool>*> optionsToDisable = {});
	static void handlePopup(HCMExceptionBase& ex, std::vector<Option<bool>*> optionsToDisable = {});
	
	explicit RuntimeExceptionHandler()
	{
		if (instance) throw HCMInitException("Cannot have more than one RuntimeExceptionHandler");
		instance = this;
	}

	~RuntimeExceptionHandler() { instance = nullptr; }


};

