#include "pch.h"
#include "Renderer3DImpl.h"
#include "directxtk\SimpleMath.h"


template<GameState::Value mGame>
void Renderer3DImpl<mGame>::constructSpriteResource(int resourceID)
{
	this->spriteResourceCache.insert(std::make_pair(resourceID, std::make_unique<SpriteResource>(resourceID, this->pDevice)));
	//this->spriteBatch = std::make_unique<SpriteBatch>(this->pDeviceContext);

}

template<GameState::Value mGame>
RECTF Renderer3DImpl<mGame>::drawSpriteImpl(int spriteResourceID, SimpleMath::Vector2 screenPosition, float spriteScale, bool shouldCenter)
{
	if (this->spriteResourceCache.contains(spriteResourceID) == false)
	{
		constructSpriteResource(spriteResourceID); // throws HCMRuntimeExceptions on fail so be ready!
	}

	auto& pSpriteToDraw = this->spriteResourceCache.at(spriteResourceID);

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

	// where and how big to draw the sprite
	RECTF surfacef;
	if (shouldCenter)
	{
		float halfSpriteWidth = pSpriteToDraw->getWidth() / 2.f;
		float halfSpriteHeight = pSpriteToDraw->getHeight() / 2.f;
		surfacef = { screenPosition.x - halfSpriteWidth, screenPosition.y - halfSpriteHeight, screenPosition.x + halfSpriteWidth, screenPosition.y + halfSpriteHeight };
	}
	else
	{
		surfacef = { screenPosition.x, screenPosition.y, screenPosition.x + pSpriteToDraw->getWidth(), screenPosition.y + pSpriteToDraw->getHeight()};
	}
	
	LOG_ONCE_CAPTURE(PLOG_INFO << "Drawing sprite with ID: " << id, id = spriteResourceID);
	LOG_ONCE_CAPTURE(PLOG_INFO << "Drawing sprite with left: " << left, left = surfacef.left);
	LOG_ONCE_CAPTURE(PLOG_INFO << "Drawing sprite with top: " << top, top = surfacef.top);
	LOG_ONCE_CAPTURE(PLOG_INFO << "Drawing sprite with right: " << right, right = surfacef.right);
	LOG_ONCE_CAPTURE(PLOG_INFO << "Drawing sprite with bottom: " << bottom, bottom = surfacef.bottom);


	this->spriteBatch->Draw(pSpriteToDraw->getTextureView(), RECT{std::lroundf(surfacef.left), std::lroundf(surfacef.top), std::lroundf(surfacef.right), std::lroundf(surfacef.bottom)});

	this->spriteBatch->End();

	return surfacef;
}


template<GameState::Value mGame>
RECTF Renderer3DImpl<mGame>::drawSprite(int spriteResourceID, SimpleMath::Vector2 screenPosition, float spriteScale)
{
	return drawSpriteImpl(spriteResourceID, screenPosition, spriteScale, false);
}

template<GameState::Value mGame>
RECTF Renderer3DImpl<mGame>::drawCenteredSprite(int spriteResourceID, SimpleMath::Vector2 screenPosition, float spriteScale)
{
	return drawSpriteImpl(spriteResourceID, screenPosition, spriteScale, true);
}




// explicit template instantiation
template class Renderer3DImpl<GameState::Value::Halo1>;
template class Renderer3DImpl<GameState::Value::Halo2>;
template class Renderer3DImpl<GameState::Value::Halo3>;
template class Renderer3DImpl<GameState::Value::Halo3ODST>;
template class Renderer3DImpl<GameState::Value::HaloReach>;
template class Renderer3DImpl<GameState::Value::Halo4>;