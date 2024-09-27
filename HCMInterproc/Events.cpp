#include "pch.h"
#include "Events.h"





pHookStateChangedEvent_Callback HookStateChangedEvent_Callback;
void HookStateChangedEvent_Subscribe(pHookStateChangedEvent_Callback callback)
{
	HookStateChangedEvent_Callback = callback;
}

void HookStateChangedEvent_Invoke(const HookStateEnum newHookState)
{
	if (HookStateChangedEvent_Callback)
		HookStateChangedEvent_Callback(newHookState);
}






pLogEvent_Callback LogEvent_Callback;
void LogEvent_Subscribe(pLogEvent_Callback callback)
{
	LogEvent_Callback = callback;
}

void LogEvent_Invoke(const wchar_t* log_str)
{
	if (!log_str)
	{
		// this is an extremely bad condition that afaik never actually happens. so throw unhandled exception if it somehow does.
		throw std::exception("log_str nullptr on interproc log event invocation! how the hell?!"); // PANIC
	}
		

	if (LogEvent_Callback)
		LogEvent_Callback(log_str);
}





pErrorEvent_Callback ErrorEvent_Callback;
void ErrorEvent_Subscribe(pErrorEvent_Callback callback)
{
	ErrorEvent_Callback = callback;
}

void ErrorEvent_Invoke(const wchar_t* Error_str)
{
	if (!Error_str)
	{
		// this is an extremely bad condition that afaik never actually happens. so throw unhandled exception if it somehow does.
		throw std::exception("Error_str nullptr on interproc Error event invocation! how the hell?!"); // PANIC
	}


	if (ErrorEvent_Callback)
		ErrorEvent_Callback(Error_str);
}

