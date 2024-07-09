#pragma once
#include "pch.h"
#include <d3d11.h>
class TextureResource
{
private:
	int m_height;
	int m_width;
	ID3D11Resource* m_texture;
	ID3D11ShaderResourceView* m_textureView;

	friend class TextureFactory;
	TextureResource(int height,
		int width,
		ID3D11Resource* texture,
		ID3D11ShaderResourceView* textureView)
		:  m_height(height), m_width(width), m_texture(texture), m_textureView(textureView) {}

public:

	// only move, no copy
	TextureResource(const TextureResource&) = delete;
	TextureResource& operator=(const TextureResource&) = delete;
	TextureResource(TextureResource&&) = default;
	TextureResource& operator=(TextureResource&&) = default;

	// null
	explicit TextureResource() = delete;

	~TextureResource()
	{
		if (m_texture)
			m_texture->Release();

		if (m_textureView)
			m_textureView->Release();
	}

	const int getHeight() const { return m_height; }
	const int getWidth() const { return m_width; }
	ID3D11Resource* getTexture() const { return m_texture; }
	ID3D11ShaderResourceView* getTextureView() const { return m_textureView; }


};
