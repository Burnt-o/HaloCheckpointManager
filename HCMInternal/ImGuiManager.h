#pragma once
#include "D3D11Hook.h"

// imgui
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_stdlib.h"
#include "MultilevelPointer.h"
// todo move stuff to impl

// Singleton: on construction, just subscribes to D3D11Hook's presentHookEvent. 
// The callback function will initialize ImGui resources (and WndPrc hook) and invoke an ImGuiRenderEvent for other classes to listen to.
// Destruction releases ImGui resources and unsubscribes to presentHookEvent.
class ImGuiManager : public std::enable_shared_from_this<ImGuiManager>
{
private:
	std::weak_ptr<D3D11Hook> m_d3d;



	static ImGuiManager* instance; 	// Private Singleton instance so static hooks/callbacks can access
	static inline std::mutex mDestructionGuard{};

	// WndProc hook, we use SetWindowLongPtr to set it up
	static LRESULT __stdcall mNewWndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);  // Handles ImGui input
	static WNDPROC mOldWndProc; // original wndProc for input we don't care about
	HWND m_windowHandle = nullptr; // Needed for creating wndProc hook, will get from SwapChain description

	// D3D11Hook->presentHookEvent handle
	ScopedCallback<eventpp::CallbackList<void(ID3D11Device*, ID3D11DeviceContext*, IDXGISwapChain*, ID3D11RenderTargetView*)>> presentEventCallback;
	eventpp::CallbackList<void(ID3D11Device*, ID3D11DeviceContext*, IDXGISwapChain*, ID3D11RenderTargetView*)>::Handle presentEventCallbackTest;
	// What we run when presentHookEvent is invoked
	void onPresentHookEvent(ID3D11Device*, ID3D11DeviceContext*, IDXGISwapChain*, ID3D11RenderTargetView*);

	// initialise resources in the first onPresentHookEvent
	void initializeImGuiResources(ID3D11Device*, ID3D11DeviceContext*, IDXGISwapChain*, ID3D11RenderTargetView*);
	bool m_isImguiInitialized = false;

	// Actual ImGui resources
	ImFont* mRescalableMonospacedFont = nullptr; // initialised in initializeImGuiResources, used in background overlays


	eventpp::ScopedRemover<eventpp::CallbackList<void(ID3D11Device*, ID3D11DeviceContext*, IDXGISwapChain*, ID3D11RenderTargetView*)>> testPresentCallback;

	void lapuaTest(SimpleMath::Vector2 ss);
public:

	explicit ImGuiManager(std::shared_ptr<D3D11Hook> d3d, std::shared_ptr<eventpp::CallbackList<void(ID3D11Device*, ID3D11DeviceContext*, IDXGISwapChain*, ID3D11RenderTargetView*)>> presentEvent) 
		: m_d3d(d3d),
		presentEventCallback(presentEvent, [this](ID3D11Device* a, ID3D11DeviceContext* b, IDXGISwapChain* c, ID3D11RenderTargetView* d) { onPresentHookEvent(a, b, c, d); })
	{
		if (instance != nullptr)
		{
			throw HCMInitException("Cannot have more than one ImGuiManager");
		}
		instance = this;



	}



	~ImGuiManager(); // Destructor releases imgui resources and unsubscribes from PresentHookEvent



	// Our own events we will invoke in onPresentHookEvent
	// Things that want to render w/ ImGui will listen to this
	std::shared_ptr<RenderEvent> BackgroundRenderEvent = std::make_shared<RenderEvent>(); // for overlays
	std::shared_ptr<RenderEvent> MidgroundRenderEvent = std::make_shared<RenderEvent>(); // for main gui / messages
	std::shared_ptr<RenderEvent> ForegroundRenderEvent = std::make_shared<RenderEvent>(); // for modal dialogues / popups



	// Banned operations for singleton
	ImGuiManager(const ImGuiManager& arg) = delete; // Copy constructor
	ImGuiManager(const ImGuiManager&& arg) = delete;  // Move constructor
	ImGuiManager& operator=(const ImGuiManager& arg) = delete; // Assignment operator
	ImGuiManager& operator=(const ImGuiManager&& arg) = delete; // Move operator

	SimpleMath::Vector2 getScreenSize() {
		 if (m_d3d.expired()) return { 0, 0 };
		 return m_d3d.lock()->getScreenSize(); }
	SimpleMath::Vector2 getScreenCenter() {
		 if (m_d3d.expired()) return { 0, 0 };
		 return m_d3d.lock()->getScreenCenter();
	 }

};

