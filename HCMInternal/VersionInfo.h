#pragma once
struct VersionInfo
{
	DWORD major, minor, build, revision;
	friend std::ostream& operator << (std::ostream& o, VersionInfo const& a)
	{
		o << std::format("{}.{}.{}.{}", a.major, a.minor, a.build, a.revision);
		return o;
	}

	bool operator == (const VersionInfo& other)
	{
		return major == other.major && minor == other.minor && build == other.build && revision == other.revision;
	}
};