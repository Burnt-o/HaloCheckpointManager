#include "pch.h"
#include "TextureFactory.h"
#include "GlobalKill.h"
#include "directxtk\SpriteBatch.h"
#include "directxtk\DDSTextureLoader.h"

std::expected<std::unique_ptr<TextureResource>, HCMRuntimeException> TextureFactory::LoadTextureMemory(
	int resourceID,
	ID3D11Device* pDevice)
{
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


	ID3D11Resource* texture;
	ID3D11ShaderResourceView* textureView;

	auto hr = CreateDDSTextureFromMemory(pDevice, (uint8_t*)lpRes, (size_t)hSize, &texture, &textureView);

	if (hr != 0) 
		return std::unexpected(HCMRuntimeException(std::format("CreateDDSTextureFromMemory failed, error: {}", hr)));


    
	// I mean it should be safe to directly cast it but juuuuuuust in case
	ID3D11Texture2D* tex2d;
	auto hr2 = texture->QueryInterface(IID_ID3D11Texture2D, (void**)&tex2d);
	if (hr2 != 0)
		return std::unexpected(HCMRuntimeException(std::format("ID3D11Resource->QueryInterface failed, error: {}", hr)));

	// Pull out desc so we can find the height and width
	D3D11_TEXTURE2D_DESC desc;
	tex2d->GetDesc(&desc);
	tex2d->Release();

	int height = desc.Height;
	int width = desc.Width;

	// Free the resource
	FreeResource(hRes);

	// Wrap in smart pointer
	return std::unique_ptr<TextureResource>(new TextureResource(height, width, texture, textureView));
}

std::expected<std::unique_ptr<TextureResource>, HCMRuntimeException> TextureFactory::LoadTextureFile(
	std::string filePath, // absolute path
	ID3D11Device* pDevice)
{

	if (!fileExists(filePath))
		return std::unexpected(HCMRuntimeException(std::format("Texture file didn't exist at path: {}", filePath)));

	ID3D11Resource* texture;
	ID3D11ShaderResourceView* textureView;
	auto hr = CreateDDSTextureFromFile(pDevice, str_to_wstr(filePath).c_str(), &texture, &textureView);

	if (hr != 0)
		return std::unexpected(HCMRuntimeException(std::format("CreateDDSTextureFromFile failed, error: {}", hr)));

	// I mean it should be safe to directly cast it but juuuuuuust in case
	ID3D11Texture2D* tex2d;
	auto hr2 = texture->QueryInterface(IID_ID3D11Texture2D, (void**)&tex2d);
	if (hr2 != 0)
		return std::unexpected(HCMRuntimeException(std::format("ID3D11Resource->QueryInterface failed, error: {}", hr)));

	// Pull out desc so we can find the height and width
	D3D11_TEXTURE2D_DESC desc;
	tex2d->GetDesc(&desc);
	tex2d->Release();

	int height = desc.Height;
	int width = desc.Width;

	return std::unique_ptr<TextureResource>(new TextureResource(height, width, texture, textureView));
}