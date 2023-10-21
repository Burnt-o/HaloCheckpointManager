#pragma once
#include "pch.h"
#include "IAnchorPoint.h"


class IMessagesGUI
{
public:
	virtual void addMessage(std::string message) = 0;
	virtual void setAnchorPoint(std::weak_ptr<IAnchorPoint> anchorPoint) = 0;
	virtual ~IMessagesGUI() = default;
};