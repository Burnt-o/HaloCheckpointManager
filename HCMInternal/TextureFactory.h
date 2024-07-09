#pragma once
#include "pch.h"
#include "TextureResource.h"

// static factory
class TextureFactory
{
public:
	static std::expected<std::unique_ptr<TextureResource>, HCMRuntimeException> LoadTextureMemory(
		int resourceID,
		ID3D11Device* pDevice);

	static std::expected<std::unique_ptr<TextureResource>, HCMRuntimeException> LoadTextureFile(
		std::string filePath, // absolute path
		ID3D11Device* pDevice);
};

