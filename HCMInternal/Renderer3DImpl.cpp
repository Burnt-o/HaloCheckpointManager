#include "pch.h"
#include "Renderer3DImpl.h"
#include "PointerDataStore.h"
#include "LoadTexture.h"
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
	 unitCube = GeometricPrimitive::CreateCube(this->pDeviceContext);
	 unitCubeInverse = GeometricPrimitive::CreateCube(this->pDeviceContext, 1.f, false); // rhcoords = flip faces
	 primitiveBatchEffect = std::make_unique<BasicEffect>(this->pDevice);

	 //basicEffect->SetVertexColorEnabled(true);
	 //primitiveBatchEffect->SetAmbientLightColor(SimpleMath::Vector4{1.f, 1.f, 1.f, 0.2f});
	 //basicEffect->DisableSpecular();
	 //basicEffect->SetEmissiveColor(SimpleMath::Vector4{1.f, 1.f, 1.f, 0.2f});

	 // how to change lighting on geometricprimitive?
	 // would probably need to create on basicEffect for it ugh
	 // plus another for the patterened texture version.. ugh


	 void const* shaderByteCode;
	 size_t byteCodeLength;

	 primitiveBatchEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);


		 this->pDevice->CreateInputLayout(VertexPosition::InputElements,
			 VertexPosition::InputElementCount,
			 shaderByteCode,
			 byteCodeLength,
			 inputLayout.ReleaseAndGetAddressOf());



	// setup hatched texture of triggers
		 auto loadedTexture = LoadTextureFromMemory(111, this->pDevice);

		 if (loadedTexture)
			 patternedTexture = loadedTexture.value().textureView;
		 else
			 PLOG_ERROR << "Failed to load texture: " << loadedTexture.error().what();

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


 // explicit template instantiation
 template class Renderer3DImpl<GameState::Value::Halo1>;
 template class Renderer3DImpl<GameState::Value::Halo2>;
 template class Renderer3DImpl<GameState::Value::Halo3>;
 template class Renderer3DImpl<GameState::Value::Halo3ODST>;
 template class Renderer3DImpl<GameState::Value::HaloReach>;
 template class Renderer3DImpl<GameState::Value::Halo4>;