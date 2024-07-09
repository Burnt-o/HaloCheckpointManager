#include "pch.h"
#include "Renderer3DImpl.h"
#include "PointerDataStore.h"
#include "TextureFactory.h"
/*

See:

Renderer3DImpl_WorldToScreen.cpp
Renderer3DImpl_Text.cpp
Renderer3DImpl_Sprites.cpp
Renderer3DImpl_updateGameCamera.cpp

for the rest of the implementation

*/


 template<GameState::Value mGame>
 Renderer3DImpl<mGame>::Renderer3DImpl(GameState game, IDIContainer& dicon)
	 : settingsWeak(dicon.Resolve<SettingsStateAndEvents>()),
	 mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
	 messagesGUIWeak(dicon.Resolve<IMessagesGUI>()),
	 runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
	 getGameCameraDataWeak(resolveDependentCheat(GetGameCameraData)),
	 mGameStateChangedCallback(dicon.Resolve<IMCCStateHook>().lock()->getMCCStateChangedEvent(), [this](const MCCState& s) {onGameStateChanged(s); })
{
	 try
	 {
		 verticalFOVPointer = dicon.Resolve< PointerDataStore>().lock()->getData<std::shared_ptr<MultilevelPointer>>(nameof(verticalFOVPointer), game);
	 }
	 catch (HCMInitException ex)
	 {
		 PLOG_ERROR << "Could not resolve verticalFOVPointer, that's okay we'll just use some bad math to guess it. Error: " << ex.what();
	 }

	 // update vfov cache if already in game
	 onGameStateChanged(mccStateHookWeak.lock()->getCurrentMCCState());
 }

 template<GameState::Value mGame>
 Renderer3DImpl<mGame>::~Renderer3DImpl()
 {
	 PLOG_DEBUG << "~Renderer3DImpl for game: " << magic_enum::enum_name(mGame);
 }

 template<GameState::Value mGame>
 void Renderer3DImpl<mGame>::initialise()
 {
	 spriteBatch = std::make_unique<SpriteBatch>(this->pDeviceContext);
	 commonStates = std::make_unique<CommonStates>(this->pDevice);
	 primitiveDrawer = std::make_unique<PrimitiveBatch<VertexPosition>>(this->pDeviceContext);
	 unitSphere = GeometricPrimitive::CreateSphere(this->pDeviceContext);
	 primitiveBatchEffect = std::make_unique<BasicEffect>(this->pDevice);


	 // setup depth stencil state.
	 // https://stackoverflow.com/questions/43815438/direct3d-11-depth-stencil-alpha-blending-issue
	 D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc;
	 ZeroMemory(&depthStencilStateDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	 depthStencilStateDesc.DepthEnable = TRUE;
	 depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	 depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_LESS;
	 depthStencilStateDesc.StencilEnable = FALSE;

	 auto hr = this->pDevice->CreateDepthStencilState(&depthStencilStateDesc, &this->m_depthStencilState);
	 if (SUCCEEDED(hr) == false)
		 throw HCMInitException(std::format("Failed to create depth stencil state, error code: {}", hr));



	 void const* shaderByteCode;
	 size_t byteCodeLength;


	 // https://github.com/microsoft/DirectXTK/blob/main/Src/GeometricPrimitive.cpp#L100


	 primitiveBatchEffect->SetTextureEnabled(true);
	 primitiveBatchEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

		this->pDevice->CreateInputLayout(VertexPosition::InputElements,
			VertexPosition::InputElementCount,
			shaderByteCode,
			byteCodeLength,
			inputLayoutTextured.ReleaseAndGetAddressOf());


		primitiveBatchEffect->SetTextureEnabled(false);
		primitiveBatchEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);
		this->pDevice->CreateInputLayout(VertexPosition::InputElements,
			VertexPosition::InputElementCount,
			shaderByteCode,
			byteCodeLength,
			inputLayoutUntextured.ReleaseAndGetAddressOf());



	// setup textures
	std::map<TextureEnum, int> textureResourceIDs = // maps HCMInternal.rc id's to texture enums for init
	{
		{TextureEnum::Crosshair, 110},
		{TextureEnum::SplotchyPattern, 111},
	};

	for (auto& [texEnum, resourceID] : textureResourceIDs)
	{
		auto texture = TextureFactory::LoadTextureMemory(resourceID, this->pDevice);
		if (!texture)
		{
			throw HCMInitException(std::format("Failed to load texture of enum: {}, resource ID: {}, error: {}",
				magic_enum::enum_name(texEnum), resourceID, texture.error().what()));
		}


		textureResources.insert(std::pair<TextureEnum, std::unique_ptr<TextureResource>>(texEnum, std::move(texture.value())));
	}



 }


 // cache verticalFOV if we have it
 template<GameState::Value mGame>
 void  Renderer3DImpl<mGame>::onGameStateChanged(const MCCState& newMCCState)
 {
	 PLOG_DEBUG << "updating Renderer3DImpl vertical FOV cache for " << ((GameState)mGame).toString();
	 pVerticalFOVCached = nullptr;
	 haveShownError = false;
	 try
	 {
		 lockOrThrow(mccStateHookWeak, mccStateHook);
		 if (mccStateHook->isGameCurrentlyPlaying(mGame) && verticalFOVPointer.has_value())
		 {
			 uintptr_t ptr;
			 if (verticalFOVPointer.value()->resolve(&ptr))
			 {
				 pVerticalFOVCached = (float*)ptr;
			 }
			 else
			 {
				 PLOG_ERROR << "error caching vertical FOV pointer: " << MultilevelPointer::GetLastError();
			 }
		 }
	 }
	 catch (HCMRuntimeException ex)
	 {
		 PLOG_ERROR << "error caching vertical FOV pointer: " << ex.what();
	 }

 }


 // TODO: depth stencil view needs to be recreated every time screen size changes!
 template<GameState::Value mGame>
void Renderer3DImpl<mGame>::createDepthStencilView(SimpleMath::Vector2 screenSize)
 {
	// create a depth stencil view
/*
Create a 2d image (texture) w/ D3D11_BIND_DEPTH_STENCIL  - needs to match resolution of screen
then making a view of that is easy pz
then bind with OMSetRenderTargets (every frame - done in updateCamera)
https://learn.microsoft.com/en-us/windows/uwp/gaming/using-depth-and-effects-on-primitives
*/

	 D3D11_TEXTURE2D_DESC depthStencilDesc;
	 depthStencilDesc.Width = screenSize.x;
	 depthStencilDesc.Height = screenSize.y;
	 depthStencilDesc.MipLevels = 1;
	 depthStencilDesc.ArraySize = 1;
	 depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	 depthStencilDesc.SampleDesc.Count = 1;
	 depthStencilDesc.SampleDesc.Quality = 0;
	 depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	 depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	 depthStencilDesc.CPUAccessFlags = 0;
	 depthStencilDesc.MiscFlags = 0;


	 auto hr1 = this->pDevice->CreateTexture2D(
		 &depthStencilDesc,
		 nullptr,
		 &this->depthStencil
	 );

	 PLOG_DEBUG << "hr1: " << hr1;
	

	 D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	 depthStencilViewDesc.Format = depthStencilDesc.Format;
	 depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	 depthStencilViewDesc.Flags = 0;
	 depthStencilViewDesc.Texture2D.MipSlice = 0;


	 auto hr2 = this->pDevice->CreateDepthStencilView(
		 this->depthStencil.Get(),
		 &depthStencilViewDesc,
		 &this->m_depthStencilView
	 );

	 PLOG_DEBUG << "hr2: " << hr2;
 }



 // explicit template instantiation
 template class Renderer3DImpl<GameState::Value::Halo1>;
 template class Renderer3DImpl<GameState::Value::Halo2>;
 template class Renderer3DImpl<GameState::Value::Halo3>;
 template class Renderer3DImpl<GameState::Value::Halo3ODST>;
 template class Renderer3DImpl<GameState::Value::HaloReach>;
 template class Renderer3DImpl<GameState::Value::Halo4>;