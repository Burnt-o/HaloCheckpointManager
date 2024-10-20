#include "pch.h"
#include "WindowsUtilities.h"
#include "curl/curl.h" // for downloadToFile
#include "imgui.h"
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
	DWORD size = GetFileVersionInfoSizeA(filename, NULL);
	if (size == 0)
		throw HCMInitException(std::format("fileInfoVersionSize was zero, error: {}", GetLastError()));

	std::vector<char> buffer;
	buffer.reserve(size);


	if (!GetFileVersionInfoA(filename, NULL, size, buffer.data()))
		throw HCMInitException(std::format("GetFileVersionInfoA failed, error: {}", GetLastError()));

	VS_FIXEDFILEINFO* pvi;
	size = sizeof(VS_FIXEDFILEINFO);
	if (!VerQueryValueA(buffer.data(), "\\", (LPVOID*)&pvi, (unsigned int*)&size))
	{
		throw HCMInitException(std::format("VerQueryValueA failed, error: {}", GetLastError()));
	}

	return VersionInfo{ 
		HIWORD(pvi->dwFileVersionMS),
		LOWORD(pvi->dwFileVersionMS),
		HIWORD(pvi->dwFileVersionLS),
		LOWORD(pvi->dwFileVersionLS)
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


void move_towards(float& value, float target, float step)
{
	value = target < value
		? std::max(value - step, target)
		: std::min(value + step, target);
}

float degreesToRadians(float degrees)
{
	constexpr float conv = (std::numbers::pi_v<float> / 180.f);
	return degrees * conv;
}

//https://stackoverflow.com/a/557774/23053739
HMODULE GetCurrentModule()
{ // NB: XP+ solution!
	HMODULE hModule = NULL;
	GetModuleHandleEx(
		GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
		(LPCTSTR)GetCurrentModule,
		&hModule);

	return hModule;
}



float remapf(float value, float istart, float istop, float ostart, float ostop)
{
	return ostart + (ostop - ostart) * ((value - istart) / (istop - istart));
}


#pragma region libcurl helpers
// write the data into a `std::string` rather than to a file.
std::size_t write_data(void* buf, std::size_t size, std::size_t nmemb,
	void* userp)
{
	if (auto sp = static_cast<std::string*>(userp))
	{
		sp->append(static_cast<char*>(buf), size * nmemb);
		return size * nmemb;
	}

	return 0;
}

// A deleter to use in the smart pointer for automatic cleanup
struct curl_dter {
	void operator()(CURL* curl) const
	{
		if (curl) curl_easy_cleanup(curl);
	}
};

// A smart pointer to automatically clean up out CURL session
using curl_uptr = std::unique_ptr<CURL, curl_dter>;
#pragma endregion //libcurl helpers

void downloadFileTo(std::string_view url, std::string_view pathToFile)
{

	auto curl = curl_uptr(curl_easy_init());
	// Download from the intertubes
	std::string fileContents;
	if (curl)
	{
		curl_easy_setopt(curl.get(), CURLOPT_URL, url.data()); // curl is a c library, needs c strings
		curl_easy_setopt(curl.get(), CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, &fileContents);

		CURLcode ec;
		if ((ec = curl_easy_perform(curl.get())) != CURLE_OK)
			throw HCMInitException(std::format("Failed to file from url:\n{}\nError: {}", url, curl_easy_strerror(ec)));

	}
	else
	{
		throw HCMInitException(("Failed to init curl service!"));
	}

	// Write to local file
	std::ofstream outFile(pathToFile.data());
	if (outFile.is_open())
	{
		outFile << fileContents;
		outFile.close();
		PLOG_INFO << "local file written!";
		return;
	}
	else
	{
		std::stringstream er;
		er << "Failed to write local file after downloading: " << GetLastError() << std::endl << "at: " << pathToFile;
		throw HCMInitException(er.str().c_str());
	}
}


std::string readFileContents(std::string_view pathToFile)
{
	std::ifstream inFile(pathToFile.data());
	if (inFile.is_open())
	{
		std::stringstream buffer;
		buffer << inFile.rdbuf();
		inFile.close();
		return buffer.str();
	}
	else
	{
		throw HCMInitException(std::format("Failed to read file contents at {} with error: {}", pathToFile, GetLastError()));
	}

}

std::string shortestStringRepresentation(float n) {
	std::array<char, 64> buf;
	auto result = std::to_chars(buf.data(), buf.data() + buf.size(), n, std::chars_format::fixed);
	return std::string(buf.data(), result.ptr - buf.data());
}

uint32_t Vec4ColorToU32(const SimpleMath::Vector4& color)
{
	return ImGui::ColorConvertFloat4ToU32(color);
}