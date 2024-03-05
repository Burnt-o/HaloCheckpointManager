#include "pch.h"
#include "RuntimeExceptionHandler.h"

#include "BinarySetting.h"
#include "HCMInternalGUI.h"
#include "MessagesGUI.h"


// Shows error message to user, and disables related toggle options if any were passed to it (optional)
void RuntimeExceptionHandler::handleMessage(HCMExceptionBase& ex)
{
	// Log to console
	PLOG_ERROR << "EXCEPTION: " << std::string(ex.what()) << std::endl << std::string(ex.source()) << std::endl << std::string(ex.trace());


	mMessagesGUI->addMessage(
		std::format(
			"An error occured:\n{}\nSee HCMInternal_logging.txt for more info, and send it to Burnt so he can fix it.", 
			ex.what()));

}

void RuntimeExceptionHandler::handleSilent(HCMExceptionBase& ex)
{
	// Log to console
	PLOG_ERROR << "EXCEPTION: " << std::string(ex.what()) << std::endl << std::string(ex.source()) << std::endl << std::string(ex.trace());

}

void RuntimeExceptionHandler::handlePopup(HCMExceptionBase& ex)
{
	// Log to console
	PLOG_ERROR << "EXCEPTION: " << std::string(ex.what()) << std::endl << std::string(ex.source()) << std::endl << std::string(ex.trace());


	// TODO : for now just sending regular message.
	handleMessage(ex);


}

