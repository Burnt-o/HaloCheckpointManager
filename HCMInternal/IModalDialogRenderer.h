#pragma once


class IModalDialogRenderer
{
public:
	virtual ~IModalDialogRenderer() = default;
	virtual std::tuple<bool, std::string> showSaveDumpNameDialog(std::string dialogTitle, std::string defaultName) = 0; // This call blocks
	virtual bool showInjectionWarningDialog(std::string dialogTitle, std::string dialogText) = 0; // This call blocks
	virtual void showFailedOptionalCheatServices() = 0; // This call blocks
};