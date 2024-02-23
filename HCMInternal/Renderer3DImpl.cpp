#include "pch.h"
#include "Renderer3DImpl.h"


/*

See:

Renderer3DImpl_WorldToScreen.cpp
Renderer3DImpl_Text.cpp
Renderer3DImpl_Sprites.cpp

for the rest of the implementation

*/


 template<GameState::Value mGame>
 Renderer3DImpl<mGame>::Renderer3DImpl(GameState game, IDIContainer& dicon)
	 : settingsWeak(dicon.Resolve<SettingsStateAndEvents>()),
	 mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
	 messagesGUIWeak(dicon.Resolve<IMessagesGUI>()),
	 runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
	 getGameCameraDataWeak(resolveDependentCheat(GetGameCameraData))
 {
	 //TODO
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
 }




 // explicit template instantiation
 template class Renderer3DImpl<GameState::Value::Halo1>;
 template class Renderer3DImpl<GameState::Value::Halo2>;
 template class Renderer3DImpl<GameState::Value::Halo3>;
 template class Renderer3DImpl<GameState::Value::Halo3ODST>;
 template class Renderer3DImpl<GameState::Value::HaloReach>;
 template class Renderer3DImpl<GameState::Value::Halo4>;