#pragma once
#include "pch.h"
#include "GlobalKill.h"

struct ManagedResource
{
private:
    HRSRC hResInfo;
    HGLOBAL hRes;
public:
    DWORD hSize;
    LPVOID hData;
    ManagedResource(int resource_id, std::string_view resource_type)
    {
        // Find the resource. 
        hResInfo = FindResourceA(GlobalKill::HCMInternalModuleHandle, MAKEINTRESOURCEA(resource_id), resource_type.data());
        if (hResInfo == NULL)
        {
            throw HCMInitException(std::format("Failed to find resource with ID {}, type {}, error {}", resource_id, resource_type, GetLastError()));
        }


        // Load the resource. 
        hRes = LoadResource(GlobalKill::HCMInternalModuleHandle, hResInfo);
        if (hRes == NULL)
        {
            throw HCMInitException(std::format("Failed to load resource with ID {}, type {}, error {}", resource_id, resource_type, GetLastError()));
        }


        // Lock the resource. 
        hData = LockResource(hRes);
        if (hData == NULL)
        {
            throw HCMInitException(std::format("Failed to lock resource with ID {}, type {}, error {}", resource_id, resource_type, GetLastError()));
        }

        // get the size of the resource
        auto hSize = SizeofResource(GlobalKill::HCMInternalModuleHandle, hResInfo);

        if (hSize == NULL)
        {
            throw HCMInitException(std::format("Failed to fine size of resource with ID {}, type {}, error {}", resource_id, resource_type, GetLastError()));
        }
    }

    ~ManagedResource()
    {
        // Free the resource
        if (hRes != NULL)
            FreeResource(hRes);
    }
};