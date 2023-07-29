#pragma once
#include "CheckpointInfo.h"
struct checkpointInjectInfoExternal // passed in callback from external. must match definition in HCMExternal.Services.InterprocService
{
	char checkpointFilePath[260];
	char levelCode[260];
	int64_t difficulty;
	char version[260];
	bool requestFailed = true;
};


struct checkpointDumpInfoExternal // passed in callback from external. must match definition in HCMExternal.Services.InterprocService
{
	char dumpFolderPath[260];
	bool requestFailed = true;
};

extern "C"
{

	typedef int64_t (*PFN_HEARTBEATCALLBACK)();
	__declspec(dllexport) void HeartbeatEventSubscribe(PFN_HEARTBEATCALLBACK callback);
	int64_t HeartbeatEventInvoke();

	typedef void (*PFN_ERRORCALLBACK)(const char* error_str);
	__declspec(dllexport) void ErrorEventSubscribe(PFN_ERRORCALLBACK callback);
	void ErrorEventInvoke(const char* error_str);

	typedef void (*PFN_LOGCALLBACK)(const wchar_t* log_str);
	__declspec(dllexport) void LogEventSubscribe(PFN_LOGCALLBACK callback);
	void LogEventInvoke(const wchar_t* log_str);

	typedef const checkpointInjectInfoExternal(*PFN_InternalRequestsInjectInfoCALLBACK)(int game);
	__declspec(dllexport) void InternalRequestsInjectInfoEventSubscribe(PFN_InternalRequestsInjectInfoCALLBACK callback);
	checkpointInjectInfoExternal InternalRequestsInjectInfoInvoke(int game);

	typedef const checkpointDumpInfoExternal(*PFN_InternalRequestsDumpInfoCALLBACK)(int game);
	__declspec(dllexport) void InternalRequestsDumpInfoEventSubscribe(PFN_InternalRequestsDumpInfoCALLBACK callback);
	checkpointDumpInfoExternal InternalRequestsDumpInfoInvoke(int game);

}

