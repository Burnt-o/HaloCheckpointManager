#pragma once
#include "pch.h"
struct VersionInfo
{
public:
	// Data members
	DWORD major, minor, build, revision;


	std::expected<VersionInfo, std::string> fromString(std::string versionInfoString)
	{
		VersionInfo out;
		if (sscanf_s(versionInfoString.c_str(), "%d.%d.%d.%d", &out.major, &out.minor, &out.build, &out.revision) != 4)
			return std::unexpected("Failed to parse version string");
		else
			return out;
	}

	constexpr int compare(const VersionInfo& other) const noexcept
	{
        if (major != other.major) {
            return major - other.major;
        }

        if (minor != other.minor) {
            return minor - other.minor;
        }

        if (build != other.build) {
            return build - other.build;
        }

		return revision - other.revision;
    
	}


	friend std::ostream& operator << (std::ostream& o, VersionInfo const& a) noexcept
	{
		o << std::format("{}.{}.{}.{}", a.major, a.minor, a.build, a.revision);
		return o;
	}

	constexpr std::strong_ordering operator<=>(const VersionInfo& other) const noexcept
	{
		int compVal = compare(other);
		if (compVal == 0)
			return std::strong_ordering::equal;
		if (compVal > 0)
			return std::strong_ordering::greater;
		return std::strong_ordering::less;
	}

	bool operator==(const VersionInfo& other) const noexcept {
		return major == other.major && minor == other.minor && build == other.build && revision == other.revision;
	}

	operator std::string() const noexcept
	{
		std::stringstream ss;
		ss << *this; // dereference or you'll just output the memory address of this
		return ss.str(); 
	}

};

