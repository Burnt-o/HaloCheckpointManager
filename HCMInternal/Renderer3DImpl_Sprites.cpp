#include "pch.h"
#include "Renderer3DImpl.h"
#include "directxtk\SimpleMath.h"



template<GameState::Value mGame>
RECTF Renderer3DImpl<mGame>::drawSpriteImpl(TextureEnum texture, SimpleMath::Vector2 screenPosition, float spriteScale, SimpleMath::Vector4 spriteColor, bool shouldCenter)
{

	auto& pSpriteToDraw = this->textureResources.at(texture);

	UINT viewportCount = 1;
	D3D11_VIEWPORT vp;
	pDeviceContext->RSGetViewports(&viewportCount, &vp);


	if (viewportCount == 0)
	{
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		vp.Width = this->screenSize.x;
		vp.Height = this->screenSize.y;
		vp.MinDepth = 0.f;
		vp.MaxDepth = 1.f;
		this->pDeviceContext->RSSetViewports(1, &vp);
	}



	// https://github.com/microsoft/DirectXTK/wiki/SpriteBatch

	this->pDeviceContext->OMSetRenderTargets(1, &this->pMainRenderTargetView, NULL);

	this->spriteBatch->SetViewport(vp);

	this->spriteBatch->Begin(SpriteSortMode_Immediate, this->commonStates->NonPremultiplied(), nullptr, nullptr, nullptr);


	LOG_ONCE_CAPTURE(PLOG_INFO << "Drawing sprite with texture: " << id, id = magic_enum::enum_name(texture));
	LOG_ONCE_CAPTURE(PLOG_INFO << "Drawing sprite with left: " << left, left = screenPosition.x);
	LOG_ONCE_CAPTURE(PLOG_INFO << "Drawing sprite with top: " << top, top = screenPosition.y);


	SimpleMath::Vector2 origin = shouldCenter ? (SimpleMath::Vector2{ pSpriteToDraw->getWidth() / 2.f, pSpriteToDraw->getHeight() / 2.f}) : (SimpleMath::Vector2{0, 0});

	this->spriteBatch->Draw(pSpriteToDraw->getTextureView(), screenPosition, NULL, spriteColor, 0.f, origin, spriteScale);

	this->spriteBatch->End();


	return { screenPosition.x, screenPosition.y, screenPosition.x + (pSpriteToDraw->getWidth() * spriteScale), screenPosition.y + (pSpriteToDraw->getHeight() * spriteScale) };




}


template<GameState::Value mGame>
RECTF Renderer3DImpl<mGame>::drawSprite(TextureEnum texture, SimpleMath::Vector2 screenPosition, float spriteScale, SimpleMath::Vector4 spriteColor)
{
	return drawSpriteImpl(texture, screenPosition, spriteScale, spriteColor, false);
}

template<GameState::Value mGame>
RECTF Renderer3DImpl<mGame>::drawCenteredSprite(TextureEnum texture, SimpleMath::Vector2 screenPosition, float spriteScale, SimpleMath::Vector4 spriteColor)
{
	return drawSpriteImpl(texture, screenPosition, spriteScale, spriteColor, true);
}




// explicit template instantiation
template class Renderer3DImpl<GameState::Value::Halo1>;
template class Renderer3DImpl<GameState::Value::Halo2>;
template class Renderer3DImpl<GameState::Value::Halo3>;
template class Renderer3DImpl<GameState::Value::Halo3ODST>;
template class Renderer3DImpl<GameState::Value::HaloReach>;
template class Renderer3DImpl<GameState::Value::Halo4>;