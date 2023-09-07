#include "pch.h"
#include "Speedhack.h"
#include "safetyhook.hpp" // hooking

// this implementation of a speedhack is adapted from https://github.com/Letomaniy/Speed-Hack
// it hooks QueryPerformanceCounter, GetTickCount, GetTickCount64, and TimeGetTime,
// and basically uses a timeHacker class to keep track of a seperate flow of "fake time" to return to anyone asking the hooks



template<class DataType>
class timeHacker {
	DataType time_offset;
	DataType time_last_update;

	double speed_;


public:


	timeHacker(DataType currentRealTime, double initialSpeed) {
		time_offset = currentRealTime;
		time_last_update = currentRealTime;

		speed_ = initialSpeed;
	}

	void setSpeed(DataType currentRealTime, double speed) {
		time_offset = getCurrentTime(currentRealTime);
		time_last_update = currentRealTime;

		speed_ = speed;
	}

	DataType getCurrentTime(DataType currentRealTime) {
		DataType difference = currentRealTime - time_last_update;

		return (DataType)(speed_ * difference) + time_offset;
	}


};



class SpeedhackImpl : public SpeedhackImplBase
{
private:

	static inline std::mutex hookMutex{};



	static inline safetyhook::InlineHook queryPerformanceCounterHook{};
	static inline safetyhook::InlineHook getTickCountHook{};
	static inline safetyhook::InlineHook getTickCount64Hook{};
	static inline safetyhook::InlineHook timeGetTimeHook{};

	static inline timeHacker<DWORD>     speedHack{GetTickCount(), 1.0};
	static inline timeHacker<ULONGLONG> speedHackULL{GetTickCount64(), 1.0};
	static inline timeHacker<LONGLONG>  speedHackLL{0, 1.0};


	static BOOL __stdcall queryPerformanceCounterHookFunction(LARGE_INTEGER* lpPerformanceCount)
	{

		//std::scoped_lock<std::mutex> lock(hookMutex);

		LARGE_INTEGER performanceCounter;
		BOOL result = queryPerformanceCounterHook.stdcall<BOOL, _LARGE_INTEGER*>(&performanceCounter);
		lpPerformanceCount->QuadPart = speedHackLL.getCurrentTime(performanceCounter.QuadPart);
		return result;
	}

	static DWORD __stdcall getTickCountHookFunction()
	{

		//std::scoped_lock<std::mutex> lock(hookMutex);
		return speedHack.getCurrentTime(getTickCountHook.stdcall<DWORD>());
	}

	static ULONGLONG __stdcall getTickCount64HookFunction()
	{

		//std::scoped_lock<std::mutex> lock(hookMutex);
		return speedHackULL.getCurrentTime(getTickCount64Hook.stdcall<ULONGLONG>());
	}

	static void setAllTimeHackerSpeeds(double speedInput) {
		//std::scoped_lock<std::mutex> lock(hookMutex);
		speedHack.setSpeed(getTickCountHook.stdcall<DWORD>(), speedInput);

		speedHackULL.setSpeed(getTickCount64Hook.stdcall<ULONGLONG>(), speedInput);

		LARGE_INTEGER performanceCounter;
		queryPerformanceCounterHook.stdcall<BOOL, _LARGE_INTEGER*>(&performanceCounter);

		speedHackLL.setSpeed(performanceCounter.QuadPart, speedInput);
	}

public:


	SpeedhackImpl()
	{

		// init speedHackLL timehacker 
		LARGE_INTEGER performanceCounter;
		QueryPerformanceCounter((_LARGE_INTEGER*)&performanceCounter);
		speedHackLL = timeHacker<LONGLONG>(performanceCounter.QuadPart, 1.0);

		// create hooks
		queryPerformanceCounterHook = safetyhook::create_inline(GetProcAddress(GetModuleHandleA("Kernel32.dll"), "QueryPerformanceCounter"), queryPerformanceCounterHookFunction);
		getTickCountHook = safetyhook::create_inline(GetProcAddress(GetModuleHandleA("Kernel32.dll"), "GetTickCount"), getTickCountHookFunction);
		getTickCount64Hook = safetyhook::create_inline(GetProcAddress(GetModuleHandleA("Kernel32.dll"), "GetTickCount64"), getTickCount64HookFunction);
		timeGetTimeHook = safetyhook::create_inline(GetProcAddress(GetModuleHandleA("Winmm.dll"), "timeGetTime"), getTickCountHookFunction);

	}

	~SpeedhackImpl() = default;

	virtual void setSpeed(double in) override
	{
		setAllTimeHackerSpeeds(in);
	}
};

Speedhack::Speedhack() : impl(std::make_unique< SpeedhackImpl>()) 
{
	instance = this;
}

Speedhack::~Speedhack() = default;

