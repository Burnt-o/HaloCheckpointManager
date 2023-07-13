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
		GlobalKill::get().mKillFlag = true;
	}
	static bool isKillSet()
	{
		return GlobalKill::get().mKillFlag;
	}
};