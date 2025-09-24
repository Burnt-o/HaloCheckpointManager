#include "pch.h"
#include "GetMCCVersion.h"
#include <winver.h> // to get version string of MCC
VersionInfo GetMCCVersion::evalVersion()
{


    VersionInfo outCurrentMCCVersion;

    if (evalVersionType() == MCCProcessType::WinStore)
    {
        // hard coded
        outCurrentMCCVersion.major = 1;
        outCurrentMCCVersion.minor = 3528;
        outCurrentMCCVersion.build = 0;
        outCurrentMCCVersion.revision = 0;
        return outCurrentMCCVersion;
    }



    HMODULE mccProcess = GetModuleHandle(NULL);
    char mccProcessPath[MAX_PATH];
    GetModuleFileNameA(mccProcess, mccProcessPath, sizeof(mccProcessPath));

    PLOG_DEBUG << "Getting file version info of mcc at: " << mccProcessPath;
    outCurrentMCCVersion = getFileVersion(mccProcessPath);

    PLOG_DEBUG << "mccVersionInfo: " << outCurrentMCCVersion;

    if (outCurrentMCCVersion.major != 1)
    {
        std::stringstream buf;
        buf << outCurrentMCCVersion;
        throw HCMInitException(std::format("mccVersionInfo did not start with \"1.\"! Actual read version: {}", buf.str()).c_str());
    }

    return outCurrentMCCVersion;
}

std::string GetMCCVersion::versionToString(VersionInfo in)
{
    std::stringstream ss;
    ss << in;
    return ss.str();
}

MCCProcessType GetMCCVersion::evalVersionType()
{
    std::string outCurrentMCCType;
    HMODULE mccProcess = GetModuleHandle(NULL);
    char mccProcessPath[MAX_PATH];
    GetModuleFileNameA(mccProcess, mccProcessPath, sizeof(mccProcessPath));

    std::string mccName = mccProcessPath;
    mccName = mccName.substr(mccName.find_last_of("\\") + 1, mccName.size() - mccName.find_last_of("\\") - 1);

    // checks need to ignore letter case
    if (boost::iequals(mccName, "MCCWinStore-Win64-Shipping.exe"))
    {
        PLOG_DEBUG << "setting process type to WinStore";
        return MCCProcessType::WinStore;
    }
    else if (boost::iequals(mccName, "MCC-Win64-Shipping.exe"))
    {
        PLOG_DEBUG << "setting process type to Steam";
        return MCCProcessType::Steam;
    }
    else
    {
        throw HCMInitException(std::format("MCC process had the wrong name!: {}", mccName));
    }
}

std::string GetMCCVersion::processToString(MCCProcessType in)
{
    return in == MCCProcessType::Steam ? "Steam" : "WinStore";
}