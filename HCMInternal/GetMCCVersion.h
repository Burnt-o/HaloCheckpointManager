#pragma once
#include "IGetMCCVersion.h"





class GetMCCVersion : public IGetMCCVersion
{
private:
	const VersionInfo mccVersion;
	const std::string mccVersionString;
	const MCCProcessType mccVersionType;

	// called by constructor
	VersionInfo evalVersion();
	std::string versionToString(VersionInfo in);
	MCCProcessType evalVersionType();
public:
	GetMCCVersion() : mccVersion(evalVersion()), mccVersionString(versionToString(mccVersion)), mccVersionType(evalVersionType()) {};
	virtual VersionInfo getMCCVersion() override { return mccVersion; };
	virtual std::string_view getMCCVersionAsString() override { return mccVersionString; };
	virtual MCCProcessType getMCCProcessType() override { return mccVersionType; };
};

