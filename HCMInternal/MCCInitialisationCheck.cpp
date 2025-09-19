#include "pch.h"
#include "MCCInitialisationCheck.h"
#include "GetMCCVersion.h"

std::optional<uintptr_t> cachedInitFlagAddress = std::nullopt;


std::expected<uintptr_t, std::string> getInitFlagAddress(HMODULE dllHandle)
{

	const std::map< std::string, uintptr_t> steamOffsetMap =
	{
		{ "1.3528.0.0", 0x4000B10 },
		{ "1.3385.0.0", 0x3FFCA18 },
		{ "1.3272.0.0", 0x3FFDA28 },
		{ "1.2904.0.0", 0x3F7B848 },
		{ "1.2645.0.0", 0x3B80E20 },
		{ "1.2448.0.0", 0x3A24EF0 }
	};

	const uintptr_t winstoreOffset = 0x3E4EF78;

	std::unique_ptr<GetMCCVersion> getMCCVer = std::make_unique<GetMCCVersion>();

	uintptr_t initFlagOffset;

	if (getMCCVer->getMCCProcessType() == MCCProcessType::Steam)
	{
		std::string versionStr = getMCCVer->getMCCVersionAsString().data();
		if (!steamOffsetMap.contains(versionStr))
			return std::unexpected(std::format("MCC version not found in map: {}", versionStr));
		else
			initFlagOffset = steamOffsetMap.at(versionStr);
	}
	else
	{
		initFlagOffset = winstoreOffset;
	}

	auto baseAddress = GetModuleHandle(NULL);
	if (!baseAddress)
		return std::unexpected(std::format("Could not resolve mcc base address: error code {}", GetLastError()));

	return ((uintptr_t)baseAddress) + initFlagOffset;

}


std::expected<bool, std::string> MCCInitialisationCheck(HMODULE dllHandle)
{
	if (cachedInitFlagAddress.has_value() == false)
	{
		auto initFlagAddy = getInitFlagAddress(dllHandle);
		if (!initFlagAddy)
			return std::unexpected(initFlagAddy.error());

		cachedInitFlagAddress = initFlagAddy.value();
	}

	char* pInitFlag = (char*)cachedInitFlagAddress.value();

	if (IsBadReadPtr(pInitFlag, sizeof(char)))
		return std::unexpected(std::format("Bad read of init flag at {:X}", cachedInitFlagAddress.value()));

	char initFlag = *pInitFlag;

	if (initFlag == (char)0)
		return false;
	else if (initFlag == (char)1)
		return true;
	else
		return std::unexpected(std::format("Invalid flag value: {:X}", (int)initFlag));


}
