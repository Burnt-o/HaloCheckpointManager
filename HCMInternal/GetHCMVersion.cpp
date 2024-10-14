#include "pch.h"
#include "GetHCMVersion.h"
#include "GlobalKill.h"
#include "boost\scope_exit.hpp"

//
//// https://learn.microsoft.com/en-us/windows/win32/menurc/vs-versioninfo
//typedef struct {
//	WORD             wLength;
//	WORD             wValueLength;
//	WORD             wType;
//	WCHAR            szKey[];
//	WORD             Padding1;
//	VS_FIXEDFILEINFO Value;
//	WORD             Padding2;
//	WORD             Children;
//} VS_VERSIONINFO;

std::expected<VersionInfo, std::string> getHCMVersion()
{

	//static_assert(false && "replace with this https://gist.github.com/wbenny/479f4bf3f7853a404d9341d0e7237f8e https://github.com/Dravion/HMSInfo/blob/master/HMSInfo/versioninfo.cpp")



	auto& HCMModule = GlobalKill::HCMInternalModuleHandle;

	HRSRC hResInfo = ::FindResource(HCMModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
	if (hResInfo == NULL)
		return std::unexpected(std::format("hResInfo null, error code: {}", GetLastError()));

	// this is giving 66 instead of 68 ? 
	DWORD rSize = ::SizeofResource(HCMModule, hResInfo);
	if (rSize == 0)
		return std::unexpected("Empty resource for version info!");


	HGLOBAL hRes = LoadResource(HCMModule, hResInfo);
	if (hRes == NULL)
		return std::unexpected(std::format("hRes null, error code: {}", GetLastError()));

	BOOST_SCOPE_EXIT(&hRes) {
		FreeResource(hRes);
	} BOOST_SCOPE_EXIT_END


	LPVOID versionInfo = ::LockResource(hRes);

	if (versionInfo == nullptr)
		return std::unexpected(std::format("versionInfo null, error code: {}", GetLastError()));

	// Read the FIXEDINFO portion:   
	VS_FIXEDFILEINFO* lpFixedInfo;
	if (VerQueryValue(versionInfo, L"\\", (void**)&lpFixedInfo, (PUINT)&rSize))
	{
		// https://learn.microsoft.com/en-us/windows/win32/api/verrsrc/ns-verrsrc-vs_fixedfileinfo
		if (lpFixedInfo->dwSignature != 0xFEEF04BD)
			return std::unexpected(std::format("version info structure had invalid signature! Observed: 0x{:X}, expected: 0x{:X}", lpFixedInfo->dwSignature, 0xFEEF04BD));

		return VersionInfo(
			HIWORD(lpFixedInfo->dwFileDateMS),
			LOWORD(lpFixedInfo->dwFileDateMS),
			HIWORD(lpFixedInfo->dwFileDateLS),
			LOWORD(lpFixedInfo->dwFileDateLS)
		);
	}
	else
	{
		return std::unexpected(std::format("No value available for verion info FIXEDFILEINFO data", GetLastError()));
	}


	//if (versionInfo->wLength != sizeof(VS_VERSIONINFO))
	//	return std::unexpected(std::format("unexpected versionInfo length, observed: {}, expected: {}", versionInfo->wLength, sizeof(VS_VERSIONINFO)));

	//if (versionInfo->wValueLength == 0)
	//	return std::unexpected(std::format("No data associated with this version info structure!"));

	//if (wcscmp(&versionInfo->szKey, L"VS_VERSION_INFO") != 0)
	//	return std::unexpected(std::format("version info structure had invalid szKey!"));



	// https://learn.microsoft.com/en-us/windows/win32/menurc/versioninfo-resource


}
