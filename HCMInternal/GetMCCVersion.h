#pragma once
#include "IGetMCCVersion.h"





class GetMCCVersion : public IGetMCCVersion
{
private:
	const VersionInfo mccVersion;
	const std::string mccVersionString;
	const MCCProcessType mccVersionType;
	const std::string mccVersionTypeString;

	// called by constructor
	VersionInfo evalVersion();
	std::string versionToString(VersionInfo in);
	MCCProcessType evalVersionType();
	std::string processToString(MCCProcessType in);
public:
	GetMCCVersion() : mccVersion(evalVersion()), mccVersionString(versionToString(mccVersion)), mccVersionType(evalVersionType()), mccVersionTypeString(processToString(mccVersionType)) {};
	virtual VersionInfo getMCCVersion() override { return mccVersion; };
	virtual std::string_view getMCCVersionAsString() override { return mccVersionString; };
	virtual MCCProcessType getMCCProcessType() override { return mccVersionType; };
	virtual std::string_view getMCCProcessTypeAsString() override { return mccVersionTypeString; };
};

