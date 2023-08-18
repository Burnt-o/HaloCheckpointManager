#include "pch.h"
#include "Events.h"


PFN_HEARTBEATCALLBACK HeartbeatEventCallback;
void HeartbeatEventSubscribe(PFN_HEARTBEATCALLBACK callback)
{
	HeartbeatEventCallback = callback;
}

int64_t HeartbeatEventInvoke()
{
	if (HeartbeatEventCallback)
	{
		return HeartbeatEventCallback();
	}
	else
	{
		PLOG_VERBOSE << "nothing subscribed to HeartbeatEventCallback";
		return -1; // deliberately invalid value, internal will check for this
	}

}


PFN_ERRORCALLBACK ErrorEventCallback;
void ErrorEventSubscribe(PFN_ERRORCALLBACK callback)
{
	ErrorEventCallback = callback;
}

void ErrorEventInvoke(const char* error_str)
{
	if (ErrorEventCallback)
		ErrorEventCallback(error_str);
}


PFN_LOGCALLBACK LogEventCallback;
void LogEventSubscribe(PFN_LOGCALLBACK callback)
{
	LogEventCallback = callback;
}

void LogEventInvoke(const wchar_t* log_str)
{
	if (!log_str) PLOG_FATAL << "what the fuck (this might infinitely recurse)";

	if (LogEventCallback)
		LogEventCallback(log_str);
}

