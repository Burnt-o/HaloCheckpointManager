#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "IRenderer3D.h"




#define Render3DEvent eventpp::CallbackList<void(IRenderer3D*)>

// Fires a 3DRender event once per frame, passing a IRenderer3D by ref to subscribers.
// Will not fire event if the event has no subscribers (skips expensive camera calculations).

class Render3DEventProvider : public IOptionalCheat {
private:
	ScopedCallback<RenderEvent> foregroundRenderEventCallback; // fired by ImGuiManager
	void onForegroundRenderEvent(const SimpleMath::Vector2& screensize);

	std::unique_ptr<IRenderer3D> p3DRenderer;
public:


	// Consumers should avoid subscribing to the render3DEvent unless they actually need to (since event provider will skip expensively updating camera data every frame IF no one is subscribed)
	std::shared_ptr<Render3DEvent> render3DEvent = std::make_shared<eventpp::CallbackList<void(IRenderer3D*)>>();
	Render3DEventProvider(GameState game, IDIContainer& dicon);
	~Render3DEventProvider();

	std::string_view getName() override { return nameof(Render3DEventProvider); }


};