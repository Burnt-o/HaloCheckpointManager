#pragma once

// Windows 
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <Psapi.h>
#include <winternl.h>
#include <Dbghelp.h>

#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <iomanip>
#include <Shlwapi.h>
#pragma comment( lib, "shlwapi.lib")
#include <locale>
#include <codecvt>
#include <ctype.h>

#include <array>
#include <mutex>
#include <memory>

#include <aclapi.h>
#include <format>
#include <fstream>

// logging
#include <plog\Log.h>
#include <plog/Initializers/ConsoleInitializer.h>
#include <plog\Formatters\TxtFormatter.h>

#include "WinHandle.h"
#include "boost\interprocess\ipc\message_queue.hpp"

using namespace boost::interprocess;

void foo() {
	PLOG_DEBUG << "foo was called!" << std::endl;
}


class MissingPermissionException : public std::exception {
private:
	std::string message;
public:
	MissingPermissionException(std::string msg) : message(msg) {}
	std::string what() { return message; }
};

constexpr auto dllName = "TestInternalRPC";
constexpr WCHAR wdllChars[] = L"TestInternalRPC.DLL";



DWORD findMCCProcessID();
bool processOldEnough(DWORD pid);
HMODULE GetCEERModuleHandle(DWORD pid);
void SendShutdownCommand(HMODULE dll, DWORD pid);
HMODULE InjectCEER(DWORD pid, std::string dllFilePath); 
void SendInitCommand(HMODULE dll, DWORD pid, std::string injectorDirectory);
uintptr_t findFunction(std::string funcName, HMODULE dll, DWORD pid);

int main()
{
	// Logging
	plog::ConsoleAppender<plog::TxtFormatter> consoleAppender;
	plog::init(plog::verbose, &consoleAppender);
	PLOG_INFO << "CEER starting up";




	try
	{

		CHAR buffer[MAX_PATH] = { 0 };
		GetModuleFileNameA(NULL, buffer, MAX_PATH);
		std::wstring::size_type pos = std::string(buffer).find_last_of("\\/");
		auto currentDirectory = std::string(buffer).substr(0, pos);
		if (!currentDirectory.ends_with('\\'))
			currentDirectory += '\\';

		PLOG_DEBUG << "CEER path: " << currentDirectory;

		auto dllFilePath = std::string(currentDirectory + dllName + ".dll");

		// test if dll exists and can be read
		std::ifstream inFile(dllFilePath.c_str());
		if (inFile.is_open())
			inFile.close();
		else
			throw std::exception(std::format("Could not find or read {}.dll! Error: {}", dllName, GetLastError()).c_str());


		// find mcc process (loop if we can't find it)
		DWORD mccPID = findMCCProcessID();
		while (mccPID == 0)
		{
			static std::once_flag printWaitingFlag;
			std::call_once(printWaitingFlag, []() { PLOG_INFO << "Waiting for MCC to open.."; });

			Sleep(500);
			mccPID = findMCCProcessID();
			if (mccPID)
			{
				if (!processOldEnough(mccPID))
				{
					mccPID = 0;
					PLOG_INFO << "Waiting for MCC to finish booting up..";
				}
			}
		}
		PLOG_INFO << "Found MCC process! ID: 0x" << std::hex << mccPID;

		//auto oldCEER = GetCEERModuleHandle(mccPID);
		//if (oldCEER)
		//{
		//	SendShutdownCommand(oldCEER, mccPID);
		//	Sleep(100);
		//}

		auto newCEER = InjectCEER(mccPID, dllFilePath);
		if (!newCEER) throw std::exception("Error finding dll after injection");
		//SendInitCommand(newCEER, mccPID, currentDirectory);

		PLOG_INFO << "Success!";
	}
	catch (MissingPermissionException ex)
	{
		PLOG_FATAL << "CEER didn't have appropiate permissions to modify MCC. If MCC or steam are running as admin, CEER needs to be run as admin too.\nNerdy details: " << ex.what();
		PLOG_INFO << "Press Enter to shutdown CEER";
		std::cin.ignore();
	}
	catch (std::exception ex)
	{
		PLOG_FATAL << "An error occured: " << ex.what();
		PLOG_INFO << "Press Enter to shutdown CEER";
		std::cin.ignore();
	}
	catch (...)
	{
		PLOG_FATAL << "An unknown error occured.";
		PLOG_INFO << "Press Enter to shutdown CEER";
		std::cin.ignore();
	}

	Sleep(1000);

	try {
		//Erase previous message queue
		message_queue::remove("message_queue");

		//Create a message_queue.
		message_queue mqE
		(open_or_create               //only create
			, "External_Messages"           //name
			, 100                       //max message number
			, sizeof(int)               //max message size
		);

		message_queue mqI
		(open_or_create               //only create
			, "Internal_Messages"           //name
			, 100                       //max message number
			, sizeof(int)               //max message size
		);

		//Send 100 numbers
		for (int i = 0; i < 100; ++i) {
			mqE.send(&i, sizeof(i), 0);
		}


		// how to know if message was recieved? different queues?

		std::size_t recvd_size;
		unsigned int priority;
		try
		{
			int rec;
			mqI.receive(&rec, sizeof(rec), recvd_size, priority);
			PLOG_DEBUG << "recieved: " << rec;
		}
		catch (interprocess_exception& ex) {

			PLOG_ERROR << ex.what() << std::endl;
		}

	}
	catch (interprocess_exception& ex) {
		PLOG_ERROR << ex.what() << std::endl;
	}





	std::cin.ignore();


#ifdef _DEBUG
	//std::cin.ignore();
#endif // _DEBUG



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

bool processOldEnough(DWORD pid)
{
	try
	{
		if (pid == 0) return false;
		constexpr auto MCCBootWaitTime = 2;
		HandlePtr mcc(OpenProcess(PROCESS_QUERY_INFORMATION, TRUE, pid));
		if (!mcc.get()) throw MissingPermissionException("missing perm PROCESS_QUERY_LIMITED_INFORMATION");


		FILETIME lpCreationTime;
		FILETIME lpExitTime;
		FILETIME lpKernelTime;
		FILETIME lpUserTime;
		auto gptResult = GetProcessTimes(mcc.get(), &lpCreationTime, &lpExitTime, &lpKernelTime, &lpUserTime);
		if (gptResult == 0)
		{
			PLOG_ERROR << "GetProcessTimes Failed: " << GetLastError();
			return false;
		}

		auto userTimeInSeconds = ULARGE_INTEGER{ lpUserTime.dwLowDateTime, lpUserTime.dwHighDateTime }.QuadPart;
		userTimeInSeconds /= 10000000; // filetime stores 100-nanosecond intervals. Ten million of these in a second.


		return userTimeInSeconds > MCCBootWaitTime;
	}
	catch (...)
	{
		return false;
	}


}


HMODULE GetCEERModuleHandle(DWORD pid)
{
	HandlePtr mcc(OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, TRUE, pid));
	if (!mcc.get()) throw MissingPermissionException("missing perm PROCESS_QUERY_LIMITED_INFORMATION");

	HMODULE hMods[1024];
	DWORD cbNeeded;



	if (EnumProcessModulesEx(mcc.get(), hMods, sizeof(hMods), &cbNeeded, LIST_MODULES_ALL))
	{
		for (int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
		{
			WCHAR szModName[MAX_PATH];


			if (GetModuleBaseName(mcc.get(), hMods[i], szModName,
				sizeof(szModName) / sizeof(TCHAR)))
			{
				if (_wcsicmp(szModName, wdllChars) == 0)
				{
					return hMods[i];
				}
			}
		}
	}
	else
	{
		throw std::exception(std::format("Failed to enumerate MCC modules: {}", GetLastError()).c_str());
	}

	return 0;
}


void SendShutdownCommand(HMODULE dll, DWORD pid)
{
	PLOG_INFO << "Sending shutdown command to old CEER";
	auto shutdownFunc = findFunction("Shutdown", dll, pid);

	HandlePtr mcc(OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, TRUE, pid));
	if (!mcc.get()) throw MissingPermissionException(std::format("SendShutdownCommand: Couldn't open MCC with createRemoteThread permissions: {}", GetLastError()).c_str());
	auto threadHandle = CreateRemoteThread(mcc.get(), NULL, NULL, (LPTHREAD_START_ROUTINE)shutdownFunc, NULL, NULL, NULL);
	if (!threadHandle) throw std::exception(std::format("Failed to create remote thread in MCC process: {}", GetLastError()).c_str());

}



uintptr_t findFunction(std::string funcName, HMODULE dll, DWORD pid)
{
	//https://stackoverflow.com/questions/59287991/how-to-get-remote-proc-address-of-injected-dll-into-another-process/59537162#59537162

	HandlePtr mcc(OpenProcess(PROCESS_VM_READ, TRUE, pid));
	if (!mcc.get()) throw MissingPermissionException("Missing perms for readProcessMemory");

	uintptr_t moduleBase = reinterpret_cast<uintptr_t>(dll);
	PLOG_DEBUG << "dll addy?: " << std::hex << dll;
	IMAGE_DOS_HEADER Image_Dos_Header = { 0 };
	SIZE_T lpNumberOfBytesRead; // unused, can't be bothered checking

	if (!ReadProcessMemory(mcc.get(), reinterpret_cast<LPCVOID>(moduleBase), &Image_Dos_Header, sizeof(IMAGE_DOS_HEADER), &lpNumberOfBytesRead))
		throw std::exception("Could not read dos header");

	if (Image_Dos_Header.e_magic != IMAGE_DOS_SIGNATURE)
		throw std::exception("dos header magic incorrect");

	IMAGE_NT_HEADERS Image_Nt_Headers = { 0 };

	if (!ReadProcessMemory(mcc.get(), reinterpret_cast<LPCVOID>(moduleBase + Image_Dos_Header.e_lfanew), &Image_Nt_Headers, sizeof(IMAGE_NT_HEADERS), &lpNumberOfBytesRead))
		throw std::exception("Could not read lfanew");

	if (Image_Nt_Headers.Signature != IMAGE_NT_SIGNATURE)
		throw std::exception("Nt header magic incorrect");

	IMAGE_EXPORT_DIRECTORY Image_Export_Directory = { 0 };
	uintptr_t img_exp_dir_rva = 0;

	if (!(img_exp_dir_rva = Image_Nt_Headers.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress))
		throw std::exception("Failed accessing data directory");

	if (!ReadProcessMemory(mcc.get(), reinterpret_cast<LPCVOID>(moduleBase + img_exp_dir_rva), &Image_Export_Directory, sizeof(IMAGE_EXPORT_DIRECTORY), &lpNumberOfBytesRead))
		throw std::exception("Fail to read Image_Export_Directory");

	uintptr_t EAT = moduleBase + Image_Export_Directory.AddressOfFunctions;
	uintptr_t ENT = moduleBase + Image_Export_Directory.AddressOfNames;
	uintptr_t EOT = moduleBase + Image_Export_Directory.AddressOfNameOrdinals;

	PLOG_DEBUG << "EAT: 0x" << std::hex << EAT;
	PLOG_DEBUG << "ENT: 0x" << std::hex << ENT;
	PLOG_DEBUG << "EOT: 0x" << std::hex << EOT;
	PLOG_DEBUG << "Image_Export_Directory.NumberOfNames: " << Image_Export_Directory.NumberOfNames;

	WORD ordinal = 0;
	SIZE_T len_buf = funcName.size() + 1;
	char* temp_buf = new char[len_buf];
	PLOG_DEBUG << "strlen of " << funcName << " is " << len_buf;

	for (size_t i = 0; i < Image_Export_Directory.NumberOfNames; i++)
	{
		uint32_t tempRvaString = 0;

		if (!ReadProcessMemory(mcc.get(), reinterpret_cast<LPCVOID>(ENT + (i * sizeof(uint32_t))), &tempRvaString, sizeof(uint32_t), &lpNumberOfBytesRead))
			throw std::exception(std::format("Fail to read tempRvaString of entry at {}", i).c_str());

		if (!ReadProcessMemory(mcc.get(), reinterpret_cast<LPCVOID>(moduleBase + tempRvaString), temp_buf, len_buf, &lpNumberOfBytesRead))
			throw std::exception(std::format("Fail to read function name of entry at {}", i).c_str());

		PLOG_DEBUG << "found name: " << temp_buf;

		if (lstrcmpiA(funcName.c_str(), temp_buf) == 0)
		{
			if (!ReadProcessMemory(mcc.get(), reinterpret_cast<LPCVOID>(EOT + (i * sizeof(WORD))), &ordinal, sizeof(WORD), &lpNumberOfBytesRead))
				throw std::exception(std::format("Fail to look up ordinal of entry at {}", i).c_str());

			uintptr_t temp_rva_func = 0;

			if (!ReadProcessMemory(mcc.get(), reinterpret_cast<LPCVOID>(EAT + (ordinal * sizeof(uint32_t))), &temp_rva_func, sizeof(uint32_t), &lpNumberOfBytesRead))
				throw std::exception(std::format("Fail to look up address of func of entry at {}", i).c_str());

			delete[] temp_buf;
			return moduleBase + temp_rva_func;
		}
	}
	delete[] temp_buf;
	throw std::exception(std::format("Failed to find function of name: {}", funcName).c_str());

}

HMODULE InjectCEER(DWORD pid, std::string dllFilePath)
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
	return GetCEERModuleHandle(pid);
}


void SendInitCommand(HMODULE dll, DWORD pid, std::string injectorDirectory)
{
	struct InitParameter
	{
		char injectorPath[MAX_PATH];
	};

	PLOG_INFO << "Sending startup command to new CEER";
	auto startupFunc = findFunction("Startup", dll, pid);

	HandlePtr mcc(OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, TRUE, pid));
	if (!mcc.get()) throw MissingPermissionException(std::format("SendInitCommand: Couldn't open MCC with createRemoteThread permissions: {}", GetLastError()).c_str());

	// Setup init param
	if (injectorDirectory.size() >= MAX_PATH) throw std::exception("injector Path too large. Try running from somewhere with a shorter filePath");
	InitParameter initParam;
	strncpy_s<MAX_PATH>(initParam.injectorPath, injectorDirectory.c_str(), injectorDirectory.size());

	// Allocate some memory on the target process, enough to store the init parameter
	auto initAlloc = VirtualAllocEx(mcc.get(), 0, sizeof(initParam), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (!initAlloc) throw std::exception("Failed to allocate memory in MCC for dll path");

	// Write the dll filepath string to allocated memory
	DWORD oldProtect;
	DWORD dontcare;
	size_t bytesWritten;
	if (!VirtualProtectEx(mcc.get(), initAlloc, sizeof(initParam), PAGE_READWRITE, &oldProtect))
		throw std::exception(std::format("Failed to unprotect initAlloc: {}", GetLastError()).c_str());
	if (!WriteProcessMemory(mcc.get(), initAlloc, &initParam, sizeof(initParam), &bytesWritten))
		throw std::exception(std::format("Failed to write initAlloc: {}", GetLastError()).c_str());
	if (!VirtualProtectEx(mcc.get(), initAlloc, sizeof(initParam), oldProtect, &dontcare))
		throw std::exception(std::format("Failed to reprotect initAlloc: {}", GetLastError()).c_str());
	if (bytesWritten != sizeof(initParam))
		throw std::exception(std::format("Failed to completely write initAlloc: {}", GetLastError()).c_str());

	// Call startup
	auto tHandle = CreateRemoteThread(mcc.get(), NULL, NULL, (LPTHREAD_START_ROUTINE)startupFunc, initAlloc, NULL, NULL);
	if (!tHandle) throw std::exception(std::format("Failed to create remote thread in MCC process: {}", GetLastError()).c_str());

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
}
