#include "pch.h"
#include "Render3DEventProvider.h"
#include "Renderer3DImpl.h"




void Render3DEventProvider::onDirectXRenderEvent(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, SimpleMath::Vector2 screenSize, ID3D11RenderTargetView* pMainRenderTargetView)
{

	ScopedAtomicBool lock(currentlyRendering);

	// only fire 3D renderer event if anyone is actually subscribed to it - because updating the camera data is expensive
	if (render3DEvent.get()->empty() == false) 
	{


#define log_null_and_throw(x) if (!x) { auto s = std::format("null pointer to {}", nameof(x)); PLOG_ERROR << s; throw HCMRuntimeException(s); }
		log_null_and_throw(pDevice);
		log_null_and_throw(pDeviceContext);
		log_null_and_throw(pMainRenderTargetView);
		log_null_and_throw(p3DRenderer);
		log_null_and_throw(render3DEvent);

		if (p3DRenderer->updateCameraData(pDevice, pDeviceContext, screenSize, pMainRenderTargetView))
		{
			render3DEvent->operator()(p3DRenderer.get());
		}
		
	}
}

Render3DEventProvider::Render3DEventProvider(GameState gameImpl, IDIContainer& dicon)
	: directXRenderEventCallback(dicon.Resolve<DirectXRenderEvent>().lock(), [this](ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, SimpleMath::Vector2 screenSize, ID3D11RenderTargetView* pMainRenderTargetView) { onDirectXRenderEvent(pDevice, pDeviceContext, screenSize, pMainRenderTargetView); })
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
	if (currentlyRendering)
	{
		currentlyRendering.wait(true);
	}
	// quickly manually kill the callback before it gets called again
	directXRenderEventCallback.removeCallback();

	PLOG_VERBOSE << "~" << getName();
}