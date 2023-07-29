#pragma once


struct offsetLengthPair
{
	int64_t offset, length;
};

struct PreserveLocations
{
	std::map<int64_t, std::vector<byte>> locations;
};

struct InjectRequirements
{
	bool singleCheckpoint, preserveLocations, SHA, BSP;
};

