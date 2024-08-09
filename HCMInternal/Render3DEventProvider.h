#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "IRenderer3D.h"
#include "DirectXRenderEvent.h"
#include "IMCCStateHook.h"
#include "ObservedEvent.h"

using Render3DEvent = eventpp::CallbackList<void(GameState, IRenderer3D*)>;

// Fires a 3DRender event once per frame, passing a IRenderer3D by ref to subscribers.
// Will not fire event if the event has no subscribers (skips expensive camera calculations).

class Render3DEventProvider : public IOptionalCheat {
private:
	GameState mGame;
	ScopedCallback<DirectXRenderEvent> directXRenderEventCallback; // fired by ImGuiManager
	void onDirectXRenderEvent(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, SimpleMath::Vector2 screenSize, ID3D11RenderTargetView* pMainRenderTargetView);
	std::atomic_bool currentlyRendering = false; // locked by above event and waited for by this classes destructer


	bool gameIsValid;
	void onGameStateChanged(const MCCState& newMCCState);
	ScopedCallback<eventpp::CallbackList<void(const MCCState&)>> mGameStateChangedCallback;

	std::unique_ptr<IRenderer3D> p3DRenderer;
	
	std::shared_ptr<ObservedEvent<Render3DEvent>> mRender3DEvent = std::make_shared<ObservedEvent<Render3DEvent>>();
public:

	// Consumers should avoid subscribing to the render3DEvent unless they actually need to (since event provider will skip expensively updating camera data every frame IF no one is subscribed)
	std::shared_ptr<ObservedEvent<Render3DEvent>> getRender3DEvent() { return mRender3DEvent; }
	Render3DEventProvider(GameState game, IDIContainer& dicon);
	~Render3DEventProvider();

	std::string_view getName() override { return nameof(Render3DEventProvider); }


};