#pragma once


extern "C"
{
	typedef void (*PFN_ERRORCALLBACK)(const char* error_str);
	__declspec(dllexport) void ErrorEventSubscribe(PFN_ERRORCALLBACK callback);
	void ErrorEventInvoke(const char* error_str);

	typedef void (*PFN_LOGCALLBACK)(const wchar_t* log_str);
	__declspec(dllexport) void LogEventSubscribe(PFN_LOGCALLBACK callback);
	void LogEventInvoke(const wchar_t* log_str);
}

