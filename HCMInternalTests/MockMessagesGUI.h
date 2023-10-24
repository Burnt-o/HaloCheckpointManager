#pragma once
#include "IMessagesGUI.h"

class MockMessagesGUI : public IMessagesGUI
{
public:
	virtual void addMessage(std::string message) override
	{
		PLOG_INFO << "USER FACING MESSAGE: " << message;
	}
	virtual void setAnchorPoint(std::weak_ptr<IAnchorPoint> anchorPoint) override {}
};