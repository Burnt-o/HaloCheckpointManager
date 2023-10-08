#pragma once

class UnhandledExceptionHandler
{
private:
	std::string mDirPath;
	static inline UnhandledExceptionHandler* instance = nullptr;
	void make_minidump(EXCEPTION_POINTERS* e);
	static LONG CALLBACK unhandled_handler(EXCEPTION_POINTERS* e);
	static void acquire_global_unhandled_exception_handler();
	static void release_global_unhandled_exception_handler();
public:

	UnhandledExceptionHandler(std::string dirpath) : mDirPath(dirpath)
	{
		PLOG_DEBUG << "UnhandledExceptionHandler con";
		if (instance) throw HCMInitException("Cannot have more than one UnhandledExceptionHandler");
		instance = this;
		acquire_global_unhandled_exception_handler();
	}


	~UnhandledExceptionHandler()
	{
		release_global_unhandled_exception_handler();
		instance = nullptr;
		PLOG_DEBUG << "~UnhandledExceptionHandler()";
	}
};

