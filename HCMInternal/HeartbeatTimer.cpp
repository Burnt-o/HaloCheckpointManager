#include "pch.h"
#include "HeartbeatTimer.h"
#include "GlobalKill.h"
#include <winternl.h>
#include <TlHelp32.h>

DWORD findProcess(std::wstring targetProcessName)
{


	// Get a snapshot of running processes
	HandlePtr hSnap(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL));

	// Success check on the snapshot tool.
	if (hSnap.get() == INVALID_HANDLE_VALUE) {
		throw HCMInitException(std::format("Failed to get snapshot of running processes: {}", GetLastError()).c_str());
	}

	// PROCESSENTRY32 is used to open and get information about a running process..
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	// If a first process exist (there are running processes), iterate through
	// all running processes.
	if (Process32First(hSnap.get(), &entry)) {
		do
		{

				// If the current process entry is the target process, return its ID (ignore case)
				if (_wcsicmp(entry.szExeFile, targetProcessName.c_str()) == 0)
				{
					return entry.th32ProcessID;
				}

		} while (Process32Next(hSnap.get(), &entry));        // Move on to the next running process.
	}


	return 0;
}


HeartbeatTimer::HeartbeatTimer(std::weak_ptr<SharedMemoryInternal> shm, std::weak_ptr<SettingsStateAndEvents> set)
	: sharedMemWeak(shm), settingsWeak(set)
    {
	DWORD HCMExternalPID = findProcess(L"HCMExternal.exe");

	HandlePtr h(OpenProcess(PROCESS_QUERY_INFORMATION, TRUE, HCMExternalPID));
	if (!h)
	{
		HCMExternalPID = findProcess(L"HaloCheckpointManager.exe");
		h = HandlePtr(OpenProcess(PROCESS_QUERY_INFORMATION, TRUE, HCMExternalPID));

		if (!h)
		{
			throw HCMInitException(std::format("Could not open HCMExternal process with perm PROCESS_QUERY_INFORMATION, {}", GetLastError()));
		}

	}
		

	HCMExternalHandle = std::move(h);

        _thd = std::thread([this]()
            {
                while (!GlobalKill::isKillSet())
                {
					// process inject command queue
					try
					{
						lockOrThrow(settingsWeak, settings);
						lockOrThrow(sharedMemWeak, sharedMem);

						if (sharedMem->getAndClearInjectQueue())
						{
							PLOG_DEBUG << "Inject command recieved, firing event!";
							settings->injectCheckpointEvent->operator()();
						}
					}
					catch (HCMRuntimeException ex)
					{
						PLOG_ERROR << "Heartbeat timer unable to check inject command queue";
					}



					DWORD exitCode = 0;
					if (GetExitCodeProcess(HCMExternalHandle.get(), &exitCode) == FALSE)
					{
						PLOG_ERROR << "GetExitCodeProcess failed, error code: " << GetLastError();
						GlobalKill::killMe();
						return;
					}
					else
					{
						// https://youtu.be/APc8QCGOdUE
						if (exitCode != STILL_ACTIVE)
						{
							PLOG_INFO << "HCMExternal terminated with code: " << exitCode;
							GlobalKill::killMe();
							return;
						}
					}


                    auto nextWakeup = std::chrono::steady_clock::now() + std::chrono::milliseconds(1000);
                    std::this_thread::sleep_until(nextWakeup);
                }
            });
    }

HeartbeatTimer::~HeartbeatTimer()
{
    PLOG_VERBOSE << "~HeartbeatTimer";
    _thd.join();
    PLOG_VERBOSE << "aye the thread died as it should";
}