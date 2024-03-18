#include "pch.h"
#include "Inject.h"
#include "Events.h"
#include "WinHandle.h"
#include "SharedMemoryExternal.h"
#include "WindowsUtilities.h"


class InjectionException : public std::exception {
private:
	std::string message;
public:
	InjectionException(std::string msg) : message(msg) {
		PLOG_ERROR << msg;
	}
	std::string what() { return message; }
};


// Forward declaration


void InjectModule(DWORD pid, std::string dllFilePath);
bool processContainsModule(DWORD pid, std::wstring moduleName);


constexpr auto dllName = "HCMInternal";
constexpr WCHAR wdllChars[] = L"HCMInternal.dll";



bool SetupInternal(DWORD mccPID, char* errorMessage, int errorMessageCapacity)
{
	PLOG_DEBUG << "SetupInternal running";
	try
	{

		if (!g_SharedMemoryExternal.get())
		{
			throw InjectionException("g_SharedMemoryExternal not initialised!");
		}

		if (!g_SharedMemoryExternal->HCMInternalStatusFlag)
		{
			throw InjectionException("g_SharedMemoryExternal->HCMInternalStatusFlag was null!");
		}

		*g_SharedMemoryExternal->HCMInternalStatusFlag = (int)HCMInternalStatus::Initialising;

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
			throw InjectionException(std::format("Could not find or read {}.dll! Error: {}", dllName, GetErrorMessage(GetLastError())).c_str());

		PLOG_DEBUG << "Found HCMInternal.dll at " << dllFilePath;

		if (!mccPID) throw InjectionException("Was passed null mcc process ID!");

		PLOG_INFO << "Given MCC process ID: 0x" << std::hex << mccPID;

		InjectModule(mccPID, dllFilePath);
		

		if (!processContainsModule(mccPID, std::wstring(wdllChars)))
		{
			PLOG_ERROR << "Process didn't appear to contain HCMInternal after injecting!";
			return true; // cooooould return false instead, but the issue could be with processContainsModule func and not the injection
		}
		else
		{
			PLOG_INFO << "Confirmed that MCC contains HCMInternal!";
		}

		return true;

	}
	catch (InjectionException ex)
	{
		std::string exTruncated = ex.what().substr(0, errorMessageCapacity - 1);
		strcpy_s(errorMessage, errorMessageCapacity, exTruncated.c_str());
		return false;
	}

}




bool processContainsModule(DWORD pid, std::wstring moduleName)
{
	// Now to actually confirm that the module was injected succesfully by enumerating mcc's modules

// Fill with current values
	HMODULE hMods[1024];
	DWORD cbNeeded;


	HandlePtr mcc(OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, TRUE, pid));
	if (!mcc)
	{
		PLOG_ERROR << (std::format("InjectCEER: Couldn't open MCC with EnumProcessModules permissions: {}", GetErrorMessage(GetLastError())).c_str());
		return false;
	}


	if (EnumProcessModules(mcc.get(), hMods, sizeof(hMods), &cbNeeded))
	{
		for (int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
		{
			TCHAR szModName[MAX_PATH];

			if (GetModuleBaseName(mcc.get(), hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR))) 
			{
				MODULEINFO info;
				if (GetModuleInformation(mcc.get(), hMods[i], &info, sizeof(info))) 
				{
					std::wstring name{ szModName };					
					if (name == moduleName) return true;
				}
				else
				{
					PLOG_ERROR << "GetModuleInformation failed with error code: " << GetErrorMessage(GetLastError());
					return false;
				}
			}
			else
			{
				PLOG_ERROR << "GetModuleBaseName failed with error code: " << GetErrorMessage(GetLastError());
				return false;
			}
		}
	}
	else
	{
		PLOG_ERROR << "EnumProcessModules failed with error code: " << GetErrorMessage(GetLastError());
		return false;
	}

	// no match
	PLOG_DEBUG << "No matching module found in target process! moduleName: " << moduleName;
	return false;
}


void InjectModule(DWORD pid, std::string dllFilePath)
{
	HandlePtr mcc(OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, TRUE, pid));
	if (!mcc) throw InjectionException(std::format("CEER didn't have appropiate permissions to modify MCC. If MCC or steam are running as admin, HCM needs to be run as admin too.\nNerdy details: InjectCEER: Couldn't open MCC with createRemoteThread permissions: {}", GetErrorMessage(GetLastError())).c_str());

	// Get the address of our own Kernel32's loadLibrary (it will be the same in the target process because Kernel32 is loaded in the same virtual memory in all processes)
	auto loadLibraryAddr = GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryA");
	if (!loadLibraryAddr) throw InjectionException(std::format("Couldn't find addr of loadLibraryA, error code: {}", GetErrorMessage(GetLastError())).c_str());



	// Allocate some memory on the target process, enough to store the filepath of the DLL
	auto pathAlloc = VirtualAllocEx(mcc.get(), 0, dllFilePath.size(), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (!pathAlloc) throw InjectionException(std::format("Failed to allocate memory in MCC for dll path, error code: {}", GetErrorMessage(GetLastError())).c_str());

	// Write the dll filepath string to allocated memory
	DWORD oldProtect;
	DWORD dontcare;
	size_t bytesWritten;
	if (!VirtualProtectEx(mcc.get(), pathAlloc, dllFilePath.size(), PAGE_READWRITE, &oldProtect))
		throw InjectionException(std::format("Failed to unprotect pathAlloc: {}", GetErrorMessage(GetLastError())).c_str());
	if (!WriteProcessMemory(mcc.get(), pathAlloc, dllFilePath.c_str(), dllFilePath.size(), &bytesWritten))
		throw InjectionException(std::format("Failed to write pathAlloc: {}", GetErrorMessage(GetLastError())).c_str());
	if (!VirtualProtectEx(mcc.get(), pathAlloc, dllFilePath.size(), oldProtect, &dontcare))
		throw InjectionException(std::format("Failed to reprotect pathAlloc: {}", GetErrorMessage(GetLastError())).c_str());
	if (bytesWritten != dllFilePath.size())
		throw InjectionException(std::format("Failed to completely write pathAlloc: {}", GetErrorMessage(GetLastError())).c_str());

	PLOG_DEBUG << "Wrote path " << dllFilePath << "to MCC allocated memory at 0x" << std::hex << pathAlloc << "(0x" << bytesWritten << " bytes written)";

	PLOG_DEBUG << "Calling createRemoteThread";
	// Create a thread to call LoadLibraryA with pathAlloc as parameter
	auto tHandle = CreateRemoteThread(mcc.get(), NULL, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(loadLibraryAddr), pathAlloc, NULL, NULL);
	if (!tHandle) throw InjectionException(std::format("Couldn't create loadLibrary thread in mcc, error code: {}", GetErrorMessage(GetLastError())).c_str());

	// Check if thread completed successfully
	auto waitResult = WaitForSingleObject(tHandle, 3000);

	switch (waitResult)
	{
	case 0x00000080:
		throw InjectionException("Remote thread failed unexpectedly (WAIT_ABANDONED)");
	case 0x00000102:
		throw InjectionException("Remote thread timed out (WAIT_TIMEOUT)");
	default:
		break;
	}

	PLOG_DEBUG << "Remote thread finished execution";

	// Get thread exit code 
	DWORD exitCode;
	GetExitCodeThread(tHandle, &exitCode);
	if (exitCode == 0) throw InjectionException(std::format("LoadLibraryA failed: {}", GetErrorMessage(GetLastError())).c_str());

	PLOG_DEBUG << "Remote thread exit code: 0x" << std::hex << exitCode;

	PLOG_INFO << "Successfully injected module " << dllName;
}




