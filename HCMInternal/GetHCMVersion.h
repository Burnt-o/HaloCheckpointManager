#pragma once
#include "pch.h"
#include "VersionInfo.h"

// Uses GetResource on HCMInternal.rc to get the FileVersion
std::expected<VersionInfo, std::string> getHCMVersion();