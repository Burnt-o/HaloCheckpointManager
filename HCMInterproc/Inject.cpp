#include "pch.h"
#include "Inject.h"
#include "Events.h"
#include "WinHandle.h"
#include "RPCServerExternal.h"
std::unique_ptr<RPCServerExternal> server;

class MissingPermissionException : public std::exception {
private:
	std::string message;
public:
	MissingPermissionException(std::string msg) : message(msg) {}
	std::string what() { return message; }
};

// Forward declaration


DWORD findMCCProcessID();
void InjectModule(DWORD pid, std::string dllFilePath);


constexpr auto dllName = "HCMInternal";
constexpr WCHAR wdllChars[] = L"HCMInternal.DLL";



bool SetupInternal()
{
	PLOG_DEBUG << "SetupInternal running";
	try
	{
		CHAR buffer[MAX_PATH] = { 0 };
		GetModuleFileNameA(NULL, buffer, MAX_PATH);
		std::wstring::size_type pos = std::string(buffer).find_last_of("\\/");
		auto currentDirectory = std::string(buffer).substr(0, pos);
		if (!currentDirectory.ends_with('\\'))
			currentDirectory += '\\';

		PLOG_DEBUG << "HCMInternal path: " << currentDirectory;

		auto dllFilePath = std::string(currentDirectory + dllName + ".dll");

		// test if dll exists and can be read
		std::ifstream inFile(dllFilePath.c_str());
		if (inFile.is_open())
			inFile.close();
		else
			throw std::exception(std::format("Could not find or read {}.dll! Error: {}", dllName, GetLastError()).c_str());

		PLOG_DEBUG << "Found HCMInternal.dll at " << dllFilePath;

		// find mcc process (loop if we can't find it)
		DWORD mccPID = findMCCProcessID();
		if (!mccPID) throw std::exception("Could not find MCC process!");

		PLOG_INFO << "Found MCC process! ID: 0x" << std::hex << mccPID;

		// Init rpc server
		server.reset();
		server = std::make_unique<RPCServerExternal>();

		InjectModule(mccPID, dllFilePath);

		// wait up to 5s to confirm connection establised
		int timeout = 0;
		while (!(server.get()->connectionEstablised))
		{
			Sleep(10);
			timeout++;
			if (timeout * 10 > 5000) throw std::exception("HCMInternal timed out on startup");
		}
		PLOG_DEBUG << "time spent to connect to internal: " << (timeout * 10) << " milliseconds";
		return true;

	}
	catch (MissingPermissionException ex)
	{
		PLOG_FATAL << "CEER didn't have appropiate permissions to modify MCC. If MCC or steam are running as admin, CEER needs to be run as admin too.\nNerdy details: " << ex.what();
		PLOG_INFO << "Press Enter to shutdown CEER";

	}
	catch (std::exception ex)
	{
		PLOG_FATAL << "An error occured: " << ex.what();
		PLOG_INFO << "Press Enter to shutdown CEER";

	}
	catch (...)
	{
		PLOG_FATAL << "An unknown error occured.";
		PLOG_INFO << "Press Enter to shutdown CEER";

	}
	return false;
}








DWORD findMCCProcessID()
{
	constexpr std::array targetProcessNames{ L"MCC-Win64-Shipping.exe", L"MCCWinStore-Win64-Shipping.exe" };

	// Get a snapshot of running processes
	HandlePtr hSnap(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL));

	// Success check on the snapshot tool.
	if (hSnap.get() == INVALID_HANDLE_VALUE) {
		throw std::exception(std::format("Failed to get snapshot of running processes: {}", GetLastError()).c_str());
	}

	// PROCESSENTRY32 is used to open and get information about a running process..
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	// If a first process exist (there are running processes), iterate through
	// all running processes.
	if (Process32First(hSnap.get(), &entry)) {
		do
		{
			for (auto targetProcName : targetProcessNames)
			{
				// If the current process entry is the target process, return its ID (ignore case)
				if (_wcsicmp(entry.szExeFile, targetProcName) == 0)
				{
					return entry.th32ProcessID;
				}
			}

		} while (Process32Next(hSnap.get(), &entry));        // Move on to the next running process.
	}


	return 0;
}




void InjectModule(DWORD pid, std::string dllFilePath)
{
	HandlePtr mcc(OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, TRUE, pid));
	if (!mcc) throw MissingPermissionException(std::format("InjectCEER: Couldn't open MCC with createRemoteThread permissions: {}", GetLastError()).c_str());

	// Get the address of our own Kernel32's loadLibraryA (it will be the same in the target process because Kernel32 is loaded in the same virtual memory in all processes)
	auto loadLibraryAddr = GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryA");
	if (!loadLibraryAddr) throw std::exception("Couldn't find addr of loadLibraryA");

	// Allocate some memory on the target process, enough to store the filepath of the DLL
	auto pathAlloc = VirtualAllocEx(mcc.get(), 0, dllFilePath.size(), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (!pathAlloc) throw std::exception("Failed to allocate memory in MCC for dll path");

	// Write the dll filepath string to allocated memory
	DWORD oldProtect;
	DWORD dontcare;
	size_t bytesWritten;
	if (!VirtualProtectEx(mcc.get(), pathAlloc, dllFilePath.size(), PAGE_READWRITE, &oldProtect))
		throw std::exception(std::format("Failed to unprotect pathAlloc: {}", GetLastError()).c_str());
	if (!WriteProcessMemory(mcc.get(), pathAlloc, dllFilePath.c_str(), dllFilePath.size(), &bytesWritten))
		throw std::exception(std::format("Failed to write pathAlloc: {}", GetLastError()).c_str());
	if (!VirtualProtectEx(mcc.get(), pathAlloc, dllFilePath.size(), oldProtect, &dontcare))
		throw std::exception(std::format("Failed to reprotect pathAlloc: {}", GetLastError()).c_str());
	if (bytesWritten != dllFilePath.size())
		throw std::exception(std::format("Failed to completely write pathAlloc: {}", GetLastError()).c_str());

	// Create a thread to call LoadLibraryA with pathAlloc as parameter
	auto tHandle = CreateRemoteThread(mcc.get(), NULL, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(loadLibraryAddr), pathAlloc, NULL, NULL);
	if (!tHandle) throw std::exception("Couldn't create loadLibrary thread in mcc");

	// Check if thread completed successfully
	auto waitResult = WaitForSingleObject(tHandle, 3000);

	switch (waitResult)
	{
	case 0x00000080:
		throw std::exception("Remote thread failed unexpectedly (WAIT_ABANDONED)");
	case 0x00000102:
		throw std::exception("Remote thread timed out (WAIT_TIMEOUT)");
	default:
		break;
	}

	// Get thread exit code 
	DWORD exitCode;
	GetExitCodeThread(tHandle, &exitCode);
	if (exitCode == 0) throw std::exception(std::format("LoadLibraryA failed: {}", GetLastError()).c_str());

	PLOG_INFO << "Success!";
	// Should I do a check here if module is actually in process? eh
}