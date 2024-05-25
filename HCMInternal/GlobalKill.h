#pragma once

// Kills dllmain RealMain() thread
class GlobalKill
{
private:
	bool mKillFlag;

	// singleton
	static GlobalKill& get() {
		static GlobalKill instance;
		return instance;
	}

	GlobalKill() = default;
	~GlobalKill() = default;
public:

	static void killMe()
	{
		// For logging purposes, we want to know what called killMe
		std::stringstream buffer;
		buffer << boost::stacktrace::stacktrace();
		PLOG_INFO << "GlobalKill::killMe (aka program shutdown) called, stacktrace: " << std::endl << buffer.str();
		GlobalKill::get().mKillFlag = true;
	}
	static bool isKillSet()
	{
		return GlobalKill::get().mKillFlag;
	}

	// Only called in dllmain init
	static void reviveMe()
	{
		PLOG_DEBUG << "GlobalKill::reviveMe";
		GlobalKill::get().mKillFlag = false;
	}

	static inline HMODULE HCMInternalModuleHandle = NULL;
};