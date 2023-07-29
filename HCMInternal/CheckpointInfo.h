#pragma once
#include "rpc\msgpack.hpp"
struct checkpointInjectInfoInternal // used by rpc/interproc to get info about what checkpoint to inject from HCMExternal
{
	char checkpointFilePath[260]; // so we can read data from the file to inject it
	char levelCode[260]; // so we can check if the current level matches the checkpoints level (game may crash otherwise)
	int64_t difficulty; // ditto
	char version[260]; // ditto
	bool requestFailed = true; // set false if request fulfilled successfully
	MSGPACK_DEFINE_ARRAY(checkpointFilePath, levelCode, difficulty, version, requestFailed);
};

struct checkpointDumpInfoInternal // used by rpc/interproc to get info about what dumpfolder and game
{
	char dumpFolderPath[260]; // where to dump the file
	bool requestFailed = true; // set false if request fulfilled successfully
	MSGPACK_DEFINE_ARRAY(dumpFolderPath, requestFailed);
};