#pragma once

extern "C" _declspec(dllexport) void SetCheckpointInjectPath(char* ppath, int lpath);
extern std::string GetCheckpointInjectPath();