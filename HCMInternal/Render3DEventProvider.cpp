#include "pch.h"
#include "Render3DEventProvider.h"
#include "Renderer3DImpl.h"




void Render3DEventProvider::onForegroundRenderEvent(const SimpleMath::Vector2& screensize)
{
	// only fire 3D renderer event if anyone is actually subscribed to it - because updating the camera data is expensive
	if (render3DEvent.get()->empty() == false) 
	{
		p3DRenderer->updateCameraData(screensize);
		render3DEvent->operator()(p3DRenderer.get());
	}
}

Render3DEventProvider::Render3DEventProvider(GameState gameImpl, IDIContainer& dicon)
	: foregroundRenderEventCallback(dicon.Resolve<RenderEvent>().lock(), [this](SimpleMath::Vector2 ss) { onForegroundRenderEvent(ss); })
{
	switch (gameImpl)
	{
	case GameState::Value::Halo1:
		p3DRenderer = std::make_unique<Renderer3DImpl<GameState::Value::Halo1>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo2:
		p3DRenderer = std::make_unique<Renderer3DImpl<GameState::Value::Halo2>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo3:
		p3DRenderer = std::make_unique<Renderer3DImpl<GameState::Value::Halo3>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo3ODST:
		p3DRenderer = std::make_unique<Renderer3DImpl<GameState::Value::Halo3ODST>>(gameImpl, dicon);
		break;

	case GameState::Value::HaloReach:
		p3DRenderer = std::make_unique<Renderer3DImpl<GameState::Value::HaloReach>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo4:
		p3DRenderer = std::make_unique<Renderer3DImpl<GameState::Value::Halo4>>(gameImpl, dicon);
		break;
	default:
		throw HCMInitException("not impl yet");
	}
}

Render3DEventProvider::~Render3DEventProvider()
{
	PLOG_VERBOSE << "~" << getName();
}