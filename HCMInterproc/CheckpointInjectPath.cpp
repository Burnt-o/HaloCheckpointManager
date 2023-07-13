#include "pch.h"
#include "CheckpointInjectPath.h"

std::string checkpointInjectPath = "";

void SetCheckpointInjectPath(char* ppath, int lpath)
{
	checkpointInjectPath = std::string(ppath, lpath);
}


std::string GetCheckpointInjectPath()
{
	return checkpointInjectPath;
}