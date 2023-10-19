#pragma once
#include "IModalDialogRenderer.h"

class MockModalDialogRenderer : public IModalDialogRenderer
{
public:
	virtual std::tuple<bool, std::string> showSaveDumpNameDialog(std::string dialogTitle, std::string defaultName) override { return std::tuple<bool, std::string>(true, "mock"); };
	virtual bool showInjectionWarningDialog(std::string dialogTitle, std::string dialogText) override { return true; };
	virtual void showFailedOptionalCheatServices() override {}
};