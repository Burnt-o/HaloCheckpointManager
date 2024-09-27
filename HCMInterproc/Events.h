#pragma once
#include "HookStateEnum.h"

extern "C"
{

	// External displays a little 
	typedef void (*pHookStateChangedEvent_Callback)(const HookStateEnum newHookState);
	__declspec(dllexport) void HookStateChangedEvent_Subscribe(pHookStateChangedEvent_Callback callback);
	void HookStateChangedEvent_Invoke(const HookStateEnum newHookState);



	// Interproc logging pipes thru back to HCMExternal
	typedef void (*pLogEvent_Callback)(const wchar_t* log_str);
	__declspec(dllexport) void LogEvent_Subscribe(pLogEvent_Callback callback);
	void LogEvent_Invoke(const wchar_t* log_str);


	// Message will be displayed to user in External dialog 
	typedef void (*pErrorEvent_Callback)(const wchar_t* error_str);
	__declspec(dllexport) void ErrorEvent_Subscribe(pErrorEvent_Callback callback);
	void ErrorEvent_Invoke(const wchar_t* error_str);




}
