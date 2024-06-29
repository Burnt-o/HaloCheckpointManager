#pragma once
#include "pch.h"
#include <d3d11.h>

struct LoadedTexture
{
	// not sure if these are owning pointers
	ID3D11Resource* texture;
	ID3D11ShaderResourceView* textureView;
};



std::expected<LoadedTexture, HCMRuntimeException> LoadTextureFromMemory(
	int resourceID,
	ID3D11Device* pDevice);

std::expected<LoadedTexture, HCMRuntimeException> LoadTextureFromFile(
	std::string filePath, // absolute path
	ID3D11Device* pDevice);