#pragma once
#include "pch.h"

class IFireCallbackListChanged
{
public:
	virtual ~IFireCallbackListChanged() = default;
	virtual std::shared_ptr<eventpp::CallbackList<void()>> getCallbackListChangedEvent() = 0;
};