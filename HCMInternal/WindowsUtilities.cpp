#include "pch.h"
#include "WindowsUtilities.h"

std::wstring str_to_wstr(const std::string str)
{
	int wchars_num = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
	wchar_t* wStr = new wchar_t[wchars_num];
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, wStr, wchars_num);
	return std::wstring(wStr);
}

std::string wstr_to_str(const std::wstring wstr)
{
	int chars_num = WideCharToMultiByte(CP_UTF8, WC_NO_BEST_FIT_CHARS, wstr.c_str(), -1, NULL, 0, NULL, NULL);

	char* str = new char[chars_num];
	WideCharToMultiByte(CP_UTF8, WC_NO_BEST_FIT_CHARS, wstr.c_str(), -1, str, chars_num, NULL, NULL);
	return std::string(str);
}

void patch_pointer(void* dest_address, uintptr_t new_address)
{
	unsigned long old_protection, unused;
	//give that address read and write permissions and store the old permissions at oldProtection
	VirtualProtect(dest_address, 8, PAGE_EXECUTE_READWRITE, &old_protection);

	//write the memory into the program and overwrite previous value
	std::memcpy(dest_address, &new_address, 8);

	//reset the permissions of the address back to oldProtection after writting memory
	VirtualProtect(dest_address, 8, old_protection, &unused);
}

void patch_memory(void* dest_address, void* src_address, size_t size)
{
	unsigned long old_protection, unused;
	//give that address read and write permissions and store the old permissions at oldProtection
	VirtualProtect(dest_address, size, PAGE_EXECUTE_READWRITE, &old_protection);

	//write the memory into the program and overwrite previous value
	std::memcpy(dest_address, src_address, size);

	//reset the permissions of the address back to oldProtection after writting memory
	VirtualProtect(dest_address, size, old_protection, &unused);
}


std::string GetMCCExePath() 
{
	CHAR buffer[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");
	return std::string(std::string(buffer).substr(0, pos) + "\\");
}



std::string ResurrectException()
{
try {
	throw;
}
catch (const std::exception& e) {
	return e.what();
}
catch (...) {
	return "Ünknown exception!";
}
}

std::string getShortName(std::string in)
{
	std::string out{ in.front() };
	in.erase(0, 1); // remove front char

	std::erase_if(in, [](char x) { return !isupper(x); });
	out += in;
	return out;
}



VersionInfo getFileVersion(const char* filename)
{
	DWORD dwHandle, size = GetFileVersionInfoSizeA(filename, &dwHandle);
	if (size == 0)
	{
		throw HCMInitException(std::format("fileInfoVersionSize was zero, error: {}", GetLastError()));

	}

	std::vector<char> buffer;
	buffer.reserve(size);


	if (!GetFileVersionInfoA(filename, dwHandle, size, buffer.data()))
	{
		throw HCMInitException(std::format("GetFileVersionInfoA failed, error: {}", GetLastError()));
	}

	VS_FIXEDFILEINFO* pvi;
	size = sizeof(VS_FIXEDFILEINFO);
	if (!VerQueryValueA(buffer.data(), "\\", (LPVOID*)&pvi, (unsigned int*)&size))
	{
		throw HCMInitException(std::format("VerQueryValueA failed, error: {}", GetLastError()));
	}

	return VersionInfo{ pvi->dwProductVersionMS >> 16,
		pvi->dwFileVersionMS & 0xFFFF,
			pvi->dwFileVersionLS >> 16,
			pvi->dwFileVersionLS & 0xFFFF
	};


}


bool fileExists(std::wstring path)
{
	return fileExists(wstr_to_str(path));
}

bool fileExists(std::string path)
{
	FILE* pfile = nullptr;
	fopen_s(&pfile, path.c_str(), "r");
	if (pfile != NULL) {
		fclose(pfile);
		return true;
	}
	else {
		return false;
	}
}