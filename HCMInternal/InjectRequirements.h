#pragma once

struct CheckpointInfo
{
	int fileLength;
	int offsetLevelCode;
	int offsetDifficulty;
	int offsetVersion;
};

struct BSPdata
{
	int offset, length;
};

struct SHAdata
{
	int offset, length;
};

struct PreserveLocations
{
	std::map<int, std::vector<byte>> locations;
};

struct InjectRequirements
{
	bool singleCheckpoint, preserveLocations, SHA, BSP, integrityCheck;
};

