
#include "includes.h"
#include "detours.h"
#include <time.h>
#include <iostream>

#pragma comment(lib, "Detours.lib") 

extern double speed;
extern bool initialised;



template<class DataType>
class SpeedHack {
	DataType time_offset;
	DataType time_last_update;

	double speed_;


public:


	SpeedHack(DataType currentRealTime, double initialSpeed) {
		time_offset = currentRealTime;
		time_last_update = currentRealTime;

		speed_ = initialSpeed;
	}

	void setSpeed(DataType currentRealTime, double speed) {
		time_offset = getCurrentTime(currentRealTime);
		time_last_update = currentRealTime;

		speed_ = speed;
	}

	bool isUnModified()
	{
		return speed_ == 1.00;
	}

	DataType getCurrentTime(DataType currentRealTime) {
		DataType difference = currentRealTime - time_last_update;

		return (DataType)(speed_ * difference) + time_offset;
	}





};


typedef DWORD(WINAPI* GetTickCountType)(void);
typedef ULONGLONG(WINAPI* GetTickCount64Type)(void);

typedef BOOL(WINAPI* QueryPerformanceCounterType)(LARGE_INTEGER* lpPerformanceCount);

GetTickCountType   g_GetTickCountOriginal;
GetTickCount64Type g_GetTickCount64Original;
GetTickCountType   g_TimeGetTimeOriginal;

QueryPerformanceCounterType g_QueryPerformanceCounterOriginal;


const double kInitialSpeed = 1.0;

SpeedHack<DWORD>     g_speedHack(GetTickCount(), kInitialSpeed);
SpeedHack<ULONGLONG> g_speedHackULL(GetTickCount64(), kInitialSpeed);
SpeedHack<LONGLONG>  g_speedHackLL(0, kInitialSpeed);




DWORD     WINAPI GetTickCountHacked(void);
ULONGLONG WINAPI GetTickCount64Hacked(void);

BOOL      WINAPI QueryPerformanceCounterHacked(LARGE_INTEGER* lpPerformanceCount);




LONG InitSpeedhack()
{
	std::cout << "Initializing speedhack" << std::endl;
	HMODULE kernel32 = GetModuleHandleA("Kernel32.dll");
	HMODULE winmm = GetModuleHandleA("Winmm.dll");

	g_GetTickCountOriginal = (GetTickCountType)GetProcAddress(kernel32, "GetTickCount");
	g_GetTickCount64Original = (GetTickCount64Type)GetProcAddress(kernel32, "GetTickCount64");
	g_TimeGetTimeOriginal = (GetTickCountType)GetProcAddress(winmm, "timeGetTime");
	g_QueryPerformanceCounterOriginal = (QueryPerformanceCounterType)GetProcAddress(kernel32, "QueryPerformanceCounter");
	LARGE_INTEGER performanceCounter;
	g_QueryPerformanceCounterOriginal(&performanceCounter);

	g_speedHackLL = SpeedHack<LONGLONG>(performanceCounter.QuadPart, kInitialSpeed);

	std::cout << "Applying speedhack detours" << std::endl;
	LONG error = NO_ERROR;
	error = DetourTransactionBegin();
	if (error != NO_ERROR)  return error;
	error = DetourAttach((PVOID*)&g_GetTickCountOriginal, (PVOID)GetTickCountHacked);
	if (error != NO_ERROR)  return error;
	error = DetourAttach((PVOID*)&g_GetTickCount64Original, (PVOID)GetTickCount64Hacked);
	if (error != NO_ERROR)  return error;
	error = DetourAttach((PVOID*)&g_TimeGetTimeOriginal, (PVOID)GetTickCountHacked);
	if (error != NO_ERROR)  return error;
	error = DetourAttach((PVOID*)&g_QueryPerformanceCounterOriginal, (PVOID)QueryPerformanceCounterHacked);
	if (error != NO_ERROR)  return error;
	error = DetourTransactionCommit();

	return error;
}


extern "C" __declspec(dllexport) BOOL SpeedIsUnmodified() {

	if (!g_hcmInitialised) return TRUE;
	return g_speedHackLL.isUnModified();
}

extern "C" __declspec(dllexport) BOOL SetSpeedhack(double* speedInput) {

	if (!g_hcmInitialised) return FALSE;

	double speed = *speedInput;
	g_speedHack.setSpeed(g_GetTickCountOriginal(), speed);

	g_speedHackULL.setSpeed(g_GetTickCount64Original(), speed);

	LARGE_INTEGER performanceCounter;
	g_QueryPerformanceCounterOriginal(&performanceCounter);

	g_speedHackLL.setSpeed(performanceCounter.QuadPart, speed);

	return TRUE;
}







DWORD WINAPI GetTickCountHacked(void) {
	return g_speedHack.getCurrentTime(g_GetTickCountOriginal());
}

ULONGLONG WINAPI GetTickCount64Hacked(void) {
	return g_speedHackULL.getCurrentTime(g_GetTickCount64Original());
}

BOOL WINAPI QueryPerformanceCounterHacked(LARGE_INTEGER* lpPerformanceCount) {
	LARGE_INTEGER performanceCounter;

	BOOL result = g_QueryPerformanceCounterOriginal(&performanceCounter);

	lpPerformanceCount->QuadPart = g_speedHackLL.getCurrentTime(performanceCounter.QuadPart);

	return result;
}
