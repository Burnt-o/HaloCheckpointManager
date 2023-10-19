#pragma once
#include "ModalDialogRenderer.h"

class MockModalDialogRenderer : public IModalDialogRenderer
{
public:
	virtual std::tuple<bool, std::string> showSaveDumpNameDialog(std::string dialogTitle, std::string defaultName) override { return std::tuple<bool, std::string>(true, "mock"); };
	virtual bool showCheckpointInjectWrongLevelWarningDialog(std::string expectedLevel, std::string observedLevel) override { return true; };
	virtual void showFailedOptionalCheatServices() override {}
};