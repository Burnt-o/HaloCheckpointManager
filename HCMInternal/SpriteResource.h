#pragma once
#include "pch.h"
#include <d3d11.h>

// Loads dds texture from dll resources, releases when destroyed (RAII wrapper basically)
class SpriteResource
{
private:
	int resourceID;
	int height;
	int width;
	ID3D11Resource* texture;
	ID3D11ShaderResourceView* textureView;


public:
	SpriteResource(int resourceID, ID3D11Device* pDevice); // tries to load resource by id, HCMInitException on failure
	~SpriteResource(); // handles freeing the texture/texture view

	// const accessors
	int getResourceID() const { return resourceID; }
	int getHeight() const { return height; }
	int getWidth() const { return width; }
	ID3D11Resource* getTexture() const { return texture; }
	ID3D11ShaderResourceView* getTextureView() const { return textureView; }
};