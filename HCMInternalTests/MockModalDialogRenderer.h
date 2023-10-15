#pragma once
#include "ModalDialogRenderer.h"

class MockModalDialogRenderer : public IModalDialogRenderer
{
public:
	virtual std::tuple<bool, std::string> showCheckpointDumpNameDialog(std::string defaultName) override { return std::tuple<bool, std::string>(true, "mock"); };
	virtual bool showCheckpointInjectWrongLevelWarningDialog(std::string expectedLevel, std::string observedLevel) override { return true; };
};