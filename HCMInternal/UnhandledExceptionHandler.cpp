#include "pch.h"
#include "UnhandledExceptionHandler.h"

void UnhandledExceptionHandler::make_minidump(EXCEPTION_POINTERS* e)
{
	auto hDbgHelp = LoadLibraryA("dbghelp");
	if (hDbgHelp == nullptr)
		return;
	auto pMiniDumpWriteDump = (decltype(&MiniDumpWriteDump))GetProcAddress(hDbgHelp, "MiniDumpWriteDump");
	if (pMiniDumpWriteDump == nullptr)
		return;

	SYSTEMTIME t;
	GetSystemTime(&t);
	std::string dumpFilePath = std::format(
		"{}HCMInternal_CRASHDUMP_{:04}{:02}{:02}_{:02}{:02}{:02}.dmp",
		mDirPath,
		t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);


	auto hFile = CreateFileA(dumpFilePath.c_str(), GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		PLOG_FATAL << "Failed to create crash dump file at " << dumpFilePath;
		return;
	}


	MINIDUMP_EXCEPTION_INFORMATION exceptionInfo;
	exceptionInfo.ThreadId = GetCurrentThreadId();
	exceptionInfo.ExceptionPointers = e;
	exceptionInfo.ClientPointers = FALSE;

	pMiniDumpWriteDump(
		GetCurrentProcess(),
		GetCurrentProcessId(),
		hFile,
		MINIDUMP_TYPE(MiniDumpWithIndirectlyReferencedMemory | MiniDumpScanMemory),
		e ? &exceptionInfo : nullptr,
		nullptr,
		nullptr);

	if (mRPCclient)
	{
		mRPCclient->sendFatalInternalError(std::format("An unhandled exception occured! Dumped crash information to {}", dumpFilePath));
	}
	
	PLOG_FATAL << "Dumped crash information to " << dumpFilePath;
	CloseHandle(hFile);

	return;
}

static LPTOP_LEVEL_EXCEPTION_FILTER OriginalUnhandledExceptionFilter;
LONG CALLBACK UnhandledExceptionHandler::unhandled_handler(EXCEPTION_POINTERS* e)
{
	PLOG_FATAL << "Unhandled exception, creating minidump!";
	if (instance)
	{
		instance->make_minidump(e);
	}
	return EXCEPTION_CONTINUE_SEARCH;
}

void UnhandledExceptionHandler::acquire_global_unhandled_exception_handler()
{
	OriginalUnhandledExceptionFilter = SetUnhandledExceptionFilter(unhandled_handler);
}

void UnhandledExceptionHandler::release_global_unhandled_exception_handler()
{
	if (OriginalUnhandledExceptionFilter) {
		SetUnhandledExceptionFilter(OriginalUnhandledExceptionFilter);
	}
}