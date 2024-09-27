#include "pch.h"
#include "ImbueInternal.h"
#include "Events.h"
#include "WinHandle.h"
#include "SharedMemoryExternal.h"
#include "WindowsUtilities.h"
#include <winternl.h>
#include <Psapi.h>
#include <processthreadsapi.h>



class InjectionException : public std::exception {
private:
	std::string message;
public:
	InjectionException(std::string msg) : message(msg) {
		PLOG_ERROR << msg;
	}
	std::string what() { return message; }
	void append(std::string ap) { message = message + ap; }
};


// Forward declaration


void InjectModule(DWORD pid, std::string dllFilePath);
bool processContainsModule(DWORD pid, std::wstring moduleName);


constexpr auto dllName = "HCMInternal";
constexpr WCHAR wdllChars[] = L"HCMInternal.dll";


// throws InjectionException on failure
void SetupInternal(DWORD mccPID)
{
	PLOG_DEBUG << "SetupInternal running";

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
		}
		else
		{
			PLOG_INFO << "Confirmed that MCC contains HCMInternal!";
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

DWORD checkActualHandlePermissions(HANDLE handle)
{
	// certain driver-level anticheats can hook openProcess and strip certain access privledges.
	// The only way to test for this (besides doing the actions that require those privledges and checking that they failed)
	// is using NTQueryObject.
	// https://learn.microsoft.com/en-us/windows/win32/api/winternl/nf-winternl-ntqueryobject

	// NtQueryObject is not defined in any header files so we gotta grab it ourselves from ntdll.dll.
	// ntdll.dll is technically loaded in every process but let's be safe and call loadLibrary anyway
	auto ntHandle = LoadLibraryA("ntdll.dll");
	if (!ntHandle) throw InjectionException(std::format("Could not get handle to ntdll ?!?! {}", GetErrorMessage(GetLastError())));

	typedef NTSTATUS(WINAPI* NTQUERYOBJECT)(
		HANDLE Handle,
		OBJECT_INFORMATION_CLASS ObjectInformationClass,
		PVOID ObjectInformation,
		ULONG ObjectInformationLength,
		PULONG ReturnLength);
	
	NTQUERYOBJECT pNtQueryObject = (NTQUERYOBJECT)GetProcAddress(ntHandle, "NtQueryObject");
	if (!pNtQueryObject) throw InjectionException(std::format("Could not resolve NtQueryObject: {}", GetErrorMessage(GetLastError())));


	PUBLIC_OBJECT_BASIC_INFORMATION obi;
	if (0 <= pNtQueryObject(handle, ObjectBasicInformation, &obi, sizeof(obi), 0))
	{
		// ACCESS_MASK contains the specific rights in the lower half of the DWORD; these are the ones we care about so mask out the rest
		return (obi.GrantedAccess & ~(0xFFFF0000));
	}
	else
	{
		throw InjectionException(std::format("NTQueryObject failed with: {}", GetErrorMessage(GetLastError())));
	}

	
}

// checks if process is elevated.
std::string checkProcessElevation(DWORD pid)
{
	//Only need PROCESS_QUERY_LIMITED_INFORMATION.
	HandlePtr secPrivHandleCheck(OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid));
	if (!secPrivHandleCheck) return std::format("Could not open handle with PROCESS_QUERY_LIMITED_INFORMATION: {}", GetErrorMessage(GetLastError()));

	HANDLE hToken = NULL;
	if (!OpenProcessToken(secPrivHandleCheck.get(), TOKEN_QUERY, &hToken))
		return std::format("OpenProcessToken failed: {}", GetErrorMessage(GetLastError()));

	DWORD dwReturnLength = 0;
	TOKEN_ELEVATION_TYPE tokenElevationType;
	if (!GetTokenInformation(hToken, TokenElevationType, &tokenElevationType, sizeof(tokenElevationType), &dwReturnLength))
		return std::format("GetTokenInformation failed: {}", GetErrorMessage(GetLastError()));

	if (dwReturnLength != sizeof(tokenElevationType))
		return std::format("Bad GetTokenInformation read, expected 0x{:X} bytes but got 0x{:X}", sizeof(tokenElevationType), dwReturnLength);

	switch (tokenElevationType)
	{
	case TOKEN_ELEVATION_TYPE::TokenElevationTypeDefault:
		return "Unlinked";

	case TOKEN_ELEVATION_TYPE::TokenElevationTypeFull:
		return "Admin";

	case TOKEN_ELEVATION_TYPE::TokenElevationTypeLimited:
		return "Normal";

	default:
		return std::format("TOKEN_ELEVATION_TYPE was invalid value: 0x{:X}", (DWORD)tokenElevationType);
	}

}

void InjectModule(DWORD pid, std::string dllFilePath)
{
	try
	{

		DWORD desiredMCCAccess = PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ;
		HandlePtr mcc(OpenProcess(desiredMCCAccess, FALSE, pid));
		if (!mcc) throw InjectionException(std::format("HCM didn't have appropiate permissions to modify MCC. If MCC or steam are running as admin, HCM needs to be run as admin too.\nNerdy details: {}", GetErrorMessage(GetLastError())).c_str());

		try
		{

			// Get the address of our own Kernel32's loadLibrary (it will be the same in the target process because Kernel32 is loaded in the same virtual memory in all processes)
			auto loadLibraryAddr = GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryA");
			if (!loadLibraryAddr) throw InjectionException(std::format("Couldn't find addr of loadLibraryA, error code: {}", GetErrorMessage(GetLastError())).c_str());


			// Allocate some memory on the target process, enough to store the filepath of the DLL
			auto pathAlloc = VirtualAllocEx(mcc.get(), 0, dllFilePath.size(), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
			if (!pathAlloc)
			{
				throw InjectionException(std::format("Failed to allocate memory in MCC for dll path, error code: {}", GetErrorMessage(GetLastError())).c_str());
			}
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
		catch (InjectionException ex) // Append more information to the exception using the process handle, then rethrow
		{
			// Check that the handle from openProcess was actually granted the permissions we requested.
			// Certain driver-level anti-cheats will hook openProcess to remove certain permissions (without causing the function to fail)
			try
			{
				DWORD grantedMCCAccess = checkActualHandlePermissions(mcc.get());

				// we want to know what bits are set in desired that AREN'T set in granted. Bitwise subtraction.
				DWORD missingRights = desiredMCCAccess & ~grantedMCCAccess;

				if (missingRights != 0)
				{
					ex.append(
						std::format(
							"\n\nGranted handle access was not desired handle access! \nThis probably (? not sure tbh) means an anti-cheat is running, stripping access rights. \nGranted access rights were: 0x{:04X}\nDifference is: 0x{:04X}", 
							grantedMCCAccess, missingRights));
				}
			}
			catch (InjectionException accessCheckEx)
			{
				ex.append(std::format("\n\nCould not double check process handles granted access bits: {}", accessCheckEx.what()));
			}

			throw ex;

		}
	}
	catch (InjectionException ex) // Append security level info then rethrow
	{
		// no mcc handle here but we can still check security levels
		ex.append("\n\nProcess Elevation Info: ");
		ex.append(std::format("\nHCM elevation level: {}", checkProcessElevation(GetCurrentProcessId())));
		ex.append(std::format("\nMCC elevation level: {}", checkProcessElevation(pid)));


		throw ex;
	}
}




std::optional<std::wstring> imbueInternal(HandlePtr& mccHandle)
{
	try
	{
		SetupInternal(GetProcessId(mccHandle.get()));
		return std::nullopt;
	}
	catch (InjectionException ex)
	{
		return str_to_wstr(ex.what());
	}
}
