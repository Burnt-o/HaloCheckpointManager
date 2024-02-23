#include "pch.h"
#include "SpriteResource.h"
#include "GlobalKill.h"
#include "directxtk\SpriteBatch.h"
#include "directxtk\DDSTextureLoader.h"
#include "directxtk\CommonStates.h"

SpriteResource::SpriteResource(int resourceID, ID3D11Device* pDevice)
{
	this->resourceID = resourceID;

	BOOL bRtn;
	LPVOID lpRes;
	HRSRC hResInfo;
	HGLOBAL hRes;

	// Find the resource. 
	hResInfo = FindResourceA(GlobalKill::HCMInternalModuleHandle, MAKEINTRESOURCEA(resourceID), "DDS");
	if (hResInfo == NULL)
	{
		throw HCMRuntimeException("hResInfo null");
	}


	// Load the resource. 
	hRes = LoadResource(GlobalKill::HCMInternalModuleHandle, hResInfo);
	if (hRes == NULL)
	{
		throw HCMRuntimeException("hRes null");
	}


	// Lock the resource
	lpRes = LockResource(hRes);

	if (lpRes == NULL)
	{
		FreeResource(hRes);
		throw HCMRuntimeException("lpRes null");
	}

	auto hSize = SizeofResource(GlobalKill::HCMInternalModuleHandle, hResInfo);


	DDS_ALPHA_MODE alpha_mode = DDS_ALPHA_MODE_CUSTOM;
	auto hr = CreateDDSTextureFromMemory(pDevice, (uint8_t*)lpRes, (size_t)hSize, &this->texture, &this->textureView);

	if (hr != 0) throw HCMRuntimeException(std::format("Failed to load texture from memory, error: {}", hr));
	// https://learn.microsoft.com/en-us/windows/win32/direct3d11/d3d11-graphics-reference-returnvalues




	 // check height/width of loaded texture

	ID3D11Texture2D* texture2d = nullptr;
	HRESULT hrtexture = this->texture->QueryInterface(&texture2d);

	if (SUCCEEDED(hrtexture))
	{
		D3D11_TEXTURE2D_DESC desc;
		texture2d->GetDesc(&desc);
		this->width = static_cast<float>(desc.Width);
		this->height = static_cast<float>(desc.Height);
	}
	else
	{
		throw HCMRuntimeException(std::format("Could not evaluate dimensions of loaded sprite: error: {}", hrtexture));
	}
	//safe_release(texture2d);
	//safe_release(this->texture);


	// Free the resource
	FreeResource(hRes);

	PLOG_DEBUG << "Succesfully constructed sprite resource!";
	PLOG_DEBUG << "Resource ID: " << resourceID;
	PLOG_DEBUG << "Width/Height: " << this->width << "/" << this->height;
}

SpriteResource::~SpriteResource()
{
	if (texture)
	{
		texture->Release();
	}

	if (textureView)
	{
		textureView->Release();
	}
}