#include "pch.h"
#include "LoadTexture.h"
#include "GlobalKill.h"
#include "directxtk\SpriteBatch.h"
#include "directxtk\DDSTextureLoader.h"

std::expected<LoadedTexture, HCMRuntimeException> LoadTextureFromMemory(
	int resourceID,
	ID3D11Device* pDevice)
{
	BOOL bRtn;
	LPVOID lpRes;
	HRSRC hResInfo;
	HGLOBAL hRes;

	// Find the resource. 
	hResInfo = FindResourceA(GlobalKill::HCMInternalModuleHandle, MAKEINTRESOURCEA(resourceID), "DDS");
	if (hResInfo == NULL)
	{
		return std::unexpected(HCMRuntimeException("Could not find resource! hResInfo null"));
	}


	// Load the resource. 
	hRes = LoadResource(GlobalKill::HCMInternalModuleHandle, hResInfo);
	if (hRes == NULL)
	{
		return std::unexpected(HCMRuntimeException("Could not load resource! hRes null"));
	}


	// Lock the resource
	lpRes = LockResource(hRes);

	if (lpRes == NULL)
	{
		FreeResource(hRes);
		return std::unexpected(HCMRuntimeException("Could not lock resource! lpRes null"));
	}

	auto hSize = SizeofResource(GlobalKill::HCMInternalModuleHandle, hResInfo);



	LoadedTexture out;
	auto hr = CreateDDSTextureFromMemory(pDevice, (uint8_t*)lpRes, (size_t)hSize, &out.texture, &out.textureView);

	if (hr != 0) 
		return std::unexpected(HCMRuntimeException(std::format("CreateDDSTextureFromMemory failed, error: {}", hr)));
	
	////https://learn.microsoft.com/en-us/windows/win32/direct3d11/d3d11-graphics-reference-returnvalues


	// Free the resource
	FreeResource(hRes);

	return out;
}

std::expected<LoadedTexture, HCMRuntimeException> LoadTextureFromFile(
	std::string filePath, // absolute path
	ID3D11Device* pDevice)
{

	if (!fileExists(filePath))
		return std::unexpected(HCMRuntimeException(std::format("Texture file didn't exist at path: {}", filePath)));

	LoadedTexture out;
	auto hr = CreateDDSTextureFromFile(pDevice, str_to_wstr(filePath).c_str(), &out.texture, &out.textureView);

	if (hr != 0)
		return std::unexpected(HCMRuntimeException(std::format("CreateDDSTextureFromFile failed, error: {}", hr)));

	return out;
}