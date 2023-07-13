#include "pch.h"
#include "Events.h"


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
	if (LogEventCallback)
		LogEventCallback(log_str);
}
