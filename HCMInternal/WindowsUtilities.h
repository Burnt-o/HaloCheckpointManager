#pragma once

#include "VersionInfo.h"



// decent chunk of this nicked from Scales' MCCTAS, with slight modifications
std::wstring str_to_wstr(const std::string str);
std::string wstr_to_str(const std::wstring wstr);


void patch_pointer(void* dest_address, uintptr_t new_address);
void patch_memory(void* dest_address, void* src_address, size_t size);


std::string ResurrectException();

std::string getShortName(std::string in);

#define acronymOf(x) getShortName(#x).c_str()

std::string GetMCCExePath();


VersionInfo getFileVersion(const char* filename);

bool fileExists(std::string path);

// allows using an initializer list to construct a boost::bimap
template <typename L, typename R>
boost::bimap<L, R>
make_bimap(std::initializer_list<typename boost::bimap<L, R>::value_type> list)
{
	return boost::bimap<L, R>(list.begin(), list.end());
}



void move_towards(float& value, float target, float step);