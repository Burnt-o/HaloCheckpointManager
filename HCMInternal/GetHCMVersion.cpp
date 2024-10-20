#include "pch.h"
#include "GetHCMVersion.h"
#include "GlobalKill.h"
#include "boost\scope_exit.hpp"


// https://learn.microsoft.com/en-us/windows/win32/menurc/vs-versioninfo


std::expected<VersionInfo, std::string> getHCMVersion()
{
	// https://learn.microsoft.com/en-us/windows/win32/menurc/versioninfo-resource

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
			HIWORD(lpFixedInfo->dwFileVersionMS),
			LOWORD(lpFixedInfo->dwFileVersionMS),
			HIWORD(lpFixedInfo->dwFileVersionLS),
			LOWORD(lpFixedInfo->dwFileVersionLS)
		);
	}
	else
	{
		return std::unexpected(std::format("No value available for verion info FIXEDFILEINFO data", GetLastError()));
	}





}
