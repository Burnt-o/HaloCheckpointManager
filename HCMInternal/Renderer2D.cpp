#include "pch.h"
#include "Renderer2D.h"

#include "GlobalKill.h"

#include "directxtk\SpriteBatch.h"
#include "directxtk\DDSTextureLoader.h"
#include "directxtk\CommonStates.h"


ID3D11Resource* texture;
ID3D11ShaderResourceView* textureView;

// Simple helper function to load an image into a DX11 texture with common settings
void LoadTextureFromMemory(int ID, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, int* out_width, int* out_height)
{

	BOOL bRtn;
	LPVOID lpRes;
	HRSRC hResInfo;
	HGLOBAL hRes;
	// Find the resource. 

	hResInfo = FindResourceA(GlobalKill::HCMInternalModuleHandle, MAKEINTRESOURCEA(ID), "DDS");
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


	// Load from memory into a raw RGBA buffer
	int image_width = 0;
	int image_height = 0;
	//unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
	//unsigned char* image_data = stbi_load_from_memory((stbi_uc*)lpRes, hSize, &image_width, &image_height, NULL, 4);

	///if (!image_data) throw HCMRuntimeException("image_data null");

	// i prob want to use this instead 
//https://learn.microsoft.com/en-us/windows/win32/direct3d11/d3dx11createtexturefrommemory
	// or actually this https://github.com/microsoft/DirectXTK/wiki/WICTextureLoader
	// no actually this https://github.com/microsoft/DirectXTK/wiki/DDSTextureLoader

	DDS_ALPHA_MODE alpha_mode = DDS_ALPHA_MODE_CUSTOM;
	auto hr = CreateDDSTextureFromMemory(pDevice, (uint8_t*)lpRes, (size_t)hSize, &texture, &textureView);

	if (hr != 0) throw HCMRuntimeException(std::format("Failed to load texture from memory, error: {}", hr));
	////https://learn.microsoft.com/en-us/windows/win32/direct3d11/d3d11-graphics-reference-returnvalues



	*out_width = image_width;
	*out_height = image_height;
	
	
	// Free the resource
	FreeResource(hRes);

}



//ID3D11ShaderResourceView* my_texture = NULL;

std::unique_ptr<SpriteBatch> spriteBatch;
std::unique_ptr<CommonStates> commonStates;
RECT sourceRect;
void Renderer2D::inititialise(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	try
	{
		int my_image_width = 0;
		int my_image_height = 0;
		LoadTextureFromMemory(107, pDevice, pDeviceContext, &my_image_width, &my_image_height);

		PLOG_DEBUG << "image_width: " << my_image_width;
		PLOG_DEBUG << "image_height: " << my_image_height;

		sourceRect.right = 1920;
		sourceRect.bottom = 1080;


		spriteBatch = std::make_unique<SpriteBatch>(pDeviceContext);
		commonStates = std::make_unique<CommonStates>(pDevice);

	}
	catch (HCMRuntimeException ex)
	{
		PLOG_ERROR << "MandatoryRender::inititialise failed: " << ex.what() << std::endl << ex.source() << std::endl << ex.trace();
		good = false;
	}


}

void Renderer2D::render(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, SimpleMath::Vector2 ss, ID3D11RenderTargetView* m_pMainRenderTargetView)
{
	if (!init)
	{
		inititialise(pDevice, pDeviceContext);
		init = true;
	}

	if (good)
	{
		LOG_ONCE(PLOG_DEBUG << "Successful 2d renderer init, rendering!");



		UINT viewportCount = 1;
		D3D11_VIEWPORT vp;
		pDeviceContext->RSGetViewports(&viewportCount, &vp);


		if (viewportCount == 0)
		{
			vp.TopLeftX = 0;
			vp.TopLeftY = 0;
			vp.Width = ss.x;
			vp.Height = ss.y;
			vp.MinDepth = 0.f;
			vp.MaxDepth = 1.f;
			pDeviceContext->RSSetViewports(1, &vp);
		}




		// https://github.com/microsoft/DirectXTK/wiki/SpriteBatch

		pDeviceContext->OMSetRenderTargets(1, &m_pMainRenderTargetView, NULL);

		spriteBatch->SetViewport(vp);

		static float transparency = 0.1f;
		if (GetKeyState('7') & 0x8000)
		{
			transparency = transparency * 1.5f;
			PLOG_DEBUG << transparency;
			Sleep(30);
		}

		if (GetKeyState('8') & 0x8000)
		{
			transparency = transparency * 0.66f;
			PLOG_DEBUG << transparency;
			Sleep(30);

		}


		DirectX::FXMVECTOR tintColour {1.f, 1.f, 1.f, 0.0024f};
		spriteBatch->Begin(SpriteSortMode_Immediate, commonStates->NonPremultiplied(), nullptr, nullptr, nullptr);
		//spriteBatch->Begin();
		RECT surface{ 0,0, ss.x, ss.y };
		spriteBatch->Draw(textureView, surface, tintColour);

		//const XMFLOAT2 zero = { 0,0 };
		//SpriteEffects effects = SpriteEffects_None;
		//spriteBatch->Draw(textureView, surface, &sourceRect, tintColour, 0.0f, zero, effects, 1.f);



		spriteBatch->End();
		

	}
}