#include "pch.h"
#include "Speedhack.h"
#include "ModuleHook.h"

// this implementation of a speedhack is adapted from https://github.com/Letomaniy/Speed-Hack
// it hooks QueryPerformanceCounter, GetTickCount, GetTickCount64, and TimeGetTime,
// and basically uses a timeHacker class to keep track of a seperate flow of "fake time" to return to anyone asking the hooks

#define SpeedHackHooksDoNothing 0

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

	eventpp::CallbackList<void(bool&)>::Handle mSpeedhackToggleCallbackHandle = {};
	eventpp::CallbackList<void(double&)>::Handle mSpeedhackSettingCallbackHandle = {};

	static inline std::mutex hookMutex{};



	static inline std::shared_ptr<ModuleInlineHook> queryPerformanceCounterHook;
	static inline std::shared_ptr<ModuleInlineHook> getTickCountHook;
	static inline std::shared_ptr<ModuleInlineHook> getTickCount64Hook;
	static inline std::shared_ptr<ModuleInlineHook> timeGetTimeHook;

	static inline timeHacker<DWORD>     speedHack{GetTickCount(), 1.0};
	static inline timeHacker<ULONGLONG> speedHackULL{GetTickCount64(), 1.0};
	static inline timeHacker<LONGLONG>  speedHackLL{0, 1.0};


	static BOOL __stdcall queryPerformanceCounterHookFunction(LARGE_INTEGER* lpPerformanceCount)
	{

		//std::scoped_lock<std::mutex> lock(hookMutex);

		 LARGE_INTEGER performanceCounter;
		 BOOL result = queryPerformanceCounterHook.get()->getInlineHook().stdcall<BOOL, _LARGE_INTEGER*>(&performanceCounter);
		lpPerformanceCount->QuadPart = speedHackLL.getCurrentTime(performanceCounter.QuadPart);
		return result;
	}

	static DWORD __stdcall getTickCountHookFunction()
	{
#if SpeedHackHooksDoNothing == 1
		return getTickCountHook.get()->getInlineHook().stdcall<DWORD>();
#endif
		//std::scoped_lock<std::mutex> lock(hookMutex);
		return speedHack.getCurrentTime(getTickCountHook.get()->getInlineHook().stdcall<DWORD>());
	}

	static ULONGLONG __stdcall getTickCount64HookFunction()
	{
#if SpeedHackHooksDoNothing == 1
		return getTickCount64Hook.get()->getInlineHook().stdcall<ULONGLONG>();
#endif
		//std::scoped_lock<std::mutex> lock(hookMutex);
		return speedHackULL.getCurrentTime(getTickCount64Hook.get()->getInlineHook().stdcall<ULONGLONG>());
	}

	static void setAllTimeHackerSpeeds(double speedInput) {
		//std::scoped_lock<std::mutex> lock(hookMutex);
		speedHack.setSpeed(getTickCountHook.get()->getInlineHook().stdcall<DWORD>(), speedInput);

		speedHackULL.setSpeed(getTickCount64Hook.get()->getInlineHook().stdcall<ULONGLONG>(), speedInput);

		LARGE_INTEGER performanceCounter;
		queryPerformanceCounterHook.get()->getInlineHook().stdcall<BOOL, _LARGE_INTEGER*>(&performanceCounter);

		speedHackLL.setSpeed(performanceCounter.QuadPart, speedInput);
	}

public:


	virtual void onToggle(bool& newValue) override
	{
		PLOG_DEBUG << "SpeedhackImpl recevied onToggle event, value: " << newValue;
		PLOG_DEBUG << "(the speedhack value is: " << OptionsState::speedhackSetting.get()->GetValue() << ")";
		//std::scoped_lock<std::mutex> lock(hookMutex);
		if (newValue)
		{

			// hooks only attached on first enabling
			queryPerformanceCounterHook.get()->setWantsToBeAttached(true);
			getTickCountHook.get()->setWantsToBeAttached(true);
			getTickCount64Hook.get()->setWantsToBeAttached(true);
			timeGetTimeHook.get()->setWantsToBeAttached(true);

			PLOG_DEBUG << "successfully set hooks";

			setAllTimeHackerSpeeds(OptionsState::speedhackSetting.get()->GetValue());

			PLOG_DEBUG << "successfully set timeHackerSpeeds";

		}
		else
		{

			setAllTimeHackerSpeeds(1.00);

			// hooks NOT detached, since this would cause the game to think time has reversed.
		}
	}
	virtual void updateSetting(double& newValue) override
	{
		PLOG_DEBUG << "SpeedhackImpl recevied updateSetting event, value: " << newValue;
		setAllTimeHackerSpeeds(newValue);
	}

	SpeedhackImpl()
	{
		// subscribe to gui events
		mSpeedhackToggleCallbackHandle = OptionsState::speedhackToggle.get()->valueChangedEvent.append([this](bool& i) {onToggle(i); });
		mSpeedhackSettingCallbackHandle = OptionsState::speedhackSetting.get()->valueChangedEvent.append([this](double& i) {updateSetting(i); });


		// init speedHackLL timehacker 
		LARGE_INTEGER performanceCounter;
		QueryPerformanceCounter((_LARGE_INTEGER*)&performanceCounter);
		speedHackLL = timeHacker<LONGLONG>(performanceCounter.QuadPart, 1.0);


		HMODULE kernel32 = GetModuleHandleA("Kernel32.dll");
		HMODULE winmm = GetModuleHandleA("Winmm.dll");

		if (!kernel32) throw HCMInitException("Could not find Kernel32.dll");
		if (!winmm) throw HCMInitException("Could not find Winmm.dll");

		// get raw pointers
		auto rawpQPC = GetProcAddress(kernel32, "QueryPerformanceCounter");
		auto rawGTC = GetProcAddress(kernel32, "GetTickCount");
		auto rawGTC64 = GetProcAddress(kernel32, "GetTickCount64");
		auto rawTGT = GetProcAddress(winmm, "timeGetTime");

		if (!rawpQPC) throw HCMInitException("Could not find QueryPerformanceCounter");
		if (!rawGTC) throw HCMInitException("Could not find GetTickCount");
		if (!rawGTC64) throw HCMInitException("Could not find GetTickCount64");
		if (!rawTGT) throw HCMInitException("Could not find timeGetTime");

		// convert to multilevelpointers
		auto pQPC = MultilevelPointer::make(rawpQPC);
		auto pGTC = MultilevelPointer::make(rawGTC);
		auto pGTC64 = MultilevelPointer::make(rawGTC64);
		auto pTGT = MultilevelPointer::make(rawTGT);

		// create hooks
		queryPerformanceCounterHook = ModuleInlineHook::make(L"kernel32", pQPC, queryPerformanceCounterHookFunction, false);
		getTickCountHook = ModuleInlineHook::make(L"kernel32", pGTC, getTickCountHookFunction, false);
		getTickCount64Hook = ModuleInlineHook::make(L"kernel32", pGTC, getTickCount64HookFunction, false);
		timeGetTimeHook = ModuleInlineHook::make(L"winmm", pTGT, getTickCountHookFunction, false);

	}
	~SpeedhackImpl()
	{
		// unsubscribe to gui events
		if (mSpeedhackToggleCallbackHandle)
			OptionsState::speedhackToggle.get()->valueChangedEvent.remove(mSpeedhackToggleCallbackHandle);

		if (mSpeedhackSettingCallbackHandle)
			OptionsState::speedhackSetting.get()->valueChangedEvent.remove(mSpeedhackSettingCallbackHandle);

		// destroy hooks (this would happen automagically but I want to be sure it happens before timeHackers are destructed
		queryPerformanceCounterHook.reset();
		getTickCountHook.reset();
		getTickCount64Hook.reset();
		timeGetTimeHook.reset();
	}
};

// we only need one implementation that covers all games
void Speedhack::initialize()
{
	if
		(impl.get()) return;
	else
		impl = std::make_unique<SpeedhackImpl>();

}