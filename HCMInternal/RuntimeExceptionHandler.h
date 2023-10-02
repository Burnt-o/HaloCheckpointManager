#pragma once
#include "MessagesGUI.h"

class RuntimeExceptionHandler
{
private:
	std::shared_ptr<MessagesGUI> mMessagesGUI;
public:

	// Shows error message to user, and disables related toggle options if any were passed to it (optional)
	// TODO: get rid of these and find a better way to handle runtime exceptions in toggle hooks
	 void handleSilent(HCMExceptionBase& ex);
	 void handleMessage(HCMExceptionBase& ex);
	 void handlePopup(HCMExceptionBase& ex);
	
	 explicit RuntimeExceptionHandler(std::shared_ptr<MessagesGUI> mes) : mMessagesGUI(mes) {}



};

