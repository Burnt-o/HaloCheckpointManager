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

// slightly modified from https://stackoverflow.com/questions/1387064/how-to-get-the-error-message-from-the-error-code-returned-by-getlasterror/45565001#45565001
std::string GetErrorMessage(DWORD dwErrorCode)
{
    LPTSTR psz{ nullptr };
    const DWORD cchMsg = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM
        | FORMAT_MESSAGE_IGNORE_INSERTS
        | FORMAT_MESSAGE_ALLOCATE_BUFFER,
        NULL, // (not used with FORMAT_MESSAGE_FROM_SYSTEM)
        dwErrorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPTSTR>(&psz),
        0,
        NULL);
    if (cchMsg > 0)
    {
        // Assign buffer to smart pointer with custom deleter so that memory gets released
        // in case String's c'tor throws an exception.
        auto deleter = [](void* p) { ::LocalFree(p); };
        std::unique_ptr<TCHAR, decltype(deleter)> ptrBuffer(psz, deleter);

        std::string str;

    #ifndef UNICODE
        str = ptrBuffer.get();
    #else
        std::wstring wStr = ptrBuffer.get();
        str = wstr_to_str(wStr);
    #endif

        return str;
    }
    else
    {
        return std::format("Unknown error code: {}", dwErrorCode);
    }
}