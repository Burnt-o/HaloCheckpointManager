#pragma once
enum class MCCProcessType
{
	Steam,
	WinStore
};

class IGetMCCVersion
{
public:
	virtual VersionInfo getMCCVersion() = 0;
	virtual std::string_view getMCCVersionAsString() = 0;
	virtual MCCProcessType getMCCProcessType() = 0;
	virtual ~IGetMCCVersion() = default;
};