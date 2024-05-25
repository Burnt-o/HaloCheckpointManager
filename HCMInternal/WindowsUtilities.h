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

float degreesToRadians(float degrees);


void move_towards(float& value, float target, float step);


HMODULE GetCurrentModule();

// I'm sick of isBadReadPtr sucking. trying this approach instead
// https://stackoverflow.com/a/65369674/23053739

// Check memory address access
const DWORD dwForbiddenArea = PAGE_GUARD | PAGE_NOACCESS;
const DWORD dwReadRights = PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY;
const DWORD dwWriteRights = PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY;

template<DWORD dwAccessRights>
bool CheckAccess(const void* const pAddress, size_t nSize)
{
	if (!pAddress || !nSize)
	{
		return false;
	}

	MEMORY_BASIC_INFORMATION sMBI;
	bool bRet = false;

	UINT_PTR pCurrentAddress = UINT_PTR(pAddress);
	UINT_PTR pEndAdress = pCurrentAddress + (nSize - 1);

	do
	{
		ZeroMemory(&sMBI, sizeof(sMBI));
		VirtualQuery(LPCVOID(pCurrentAddress), &sMBI, sizeof(sMBI));

		// THIS HERE IS SLOW AS FUCK
		bRet = (sMBI.State & MEM_COMMIT) // memory allocated and
			&& !(sMBI.Protect & dwForbiddenArea) // access to page allowed and
			&& (sMBI.Protect & dwAccessRights); // the required rights

		pCurrentAddress = (UINT_PTR(sMBI.BaseAddress) + sMBI.RegionSize);
	} while (bRet && pCurrentAddress <= pEndAdress);

	return bRet;
}




// Turns out this is extremely unperformant
//#define IsBadWritePtr(p,n) (!CheckAccess<dwWriteRights>(p,n))
//#define IsBadReadPtr(p,n) (!CheckAccess<dwReadRights>(p,n))
//#define IsBadStringPtrW(p,n) (!CheckAccess<dwReadRights>(p,n*2))

// convert a range of values [istart...istop] to another range [ostart...ostop]
float remapf(float value, float istart, float istop, float ostart, float ostop);


template <typename T>
std::string to_string_with_precision(const T a_value, const int n)
{
	std::ostringstream out;
	out.precision(n);
	out << std::fixed << a_value;
	return std::move(out).str();
}

// downloads text content of url to local file. Throws HCMInit on fail
void downloadFileTo(std::string_view url, std::string_view pathToFile);

std::string readFileContents(std::string_view pathToFile);

// std::fmodf but returns wrapped around value for negative inputs of a.
// eg where std::fmodf(-1, 6) would return -1, this would instead return 5.
float flooredModulo(float a, float n);