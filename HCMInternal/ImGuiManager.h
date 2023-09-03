#pragma once
#include "D3D11Hook.h"

// imgui
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_stdlib.h"

// Singleton: on construction, just subscribes to D3D11Hook's presentHookEvent. 
// The callback function will initialize ImGui resources (and WndPrc hook) and invoke an ImGuiRenderEvent for other classes to listen to.
// Destruction releases ImGui resources and unsubscribes to presentHookEvent.
class ImGuiManager
{
private:
	static ImGuiManager* instance; 	// Private Singleton instance so static hooks/callbacks can access
	std::mutex mDestructionGuard; // Protects against Singleton destruction while callbacks are executing

	// WndProc hook, we use SetWindowLongPtr to set it up
	static LRESULT __stdcall mNewWndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);  // Handles ImGui input
	static WNDPROC mOldWndProc; // original wndProc for input we don't care about
	HWND m_windowHandle = nullptr; // Needed for creating wndProc hook, will get from SwapChain description

	// D3D11Hook Event reference and our handle to the append so we can remove it in destructor
	eventpp::CallbackList<void(ID3D11Device*, ID3D11DeviceContext*, IDXGISwapChain*, ID3D11RenderTargetView*)>& pPresentHookEvent;
	eventpp::CallbackList<void(ID3D11Device*, ID3D11DeviceContext*, IDXGISwapChain*, ID3D11RenderTargetView*)>::Handle mCallbackHandle;


	// What we run when D3D11Hook PresentHookEvent is invoked
	static void onPresentHookEvent(ID3D11Device*, ID3D11DeviceContext*, IDXGISwapChain*, ID3D11RenderTargetView*);

	// initialise resources in the first onPresentHookEvent
	void initializeImGuiResources(ID3D11Device*, ID3D11DeviceContext*, IDXGISwapChain*, ID3D11RenderTargetView*);
	bool m_isImguiInitialized = false;

	// Actual ImGui resources
	ImFont* mDefaultFont = nullptr; // unused for now. In case we wanted to change the default font (or it's size)




public:
	// Gets passed D3D11Hook PresentHookEvent reference so we can subscribe and unsubscribe
	explicit ImGuiManager(eventpp::CallbackList<void(ID3D11Device*, ID3D11DeviceContext*, IDXGISwapChain*, ID3D11RenderTargetView*)>& pEvent) : pPresentHookEvent(pEvent)
	{
		if (instance != nullptr)
		{
			throw HCMInitException("Cannot have more than one ImGuiManager");
		}
		instance = this;
		mCallbackHandle = pPresentHookEvent.append(onPresentHookEvent);
	}
	~ImGuiManager(); // Destructor releases imgui resources and unsubscribes from PresentHookEvent

	// Our own events we will invoke in onPresentHookEvent
	// Things that want to render w/ ImGui will listen to this
	ActionEvent BackgroundRenderEvent; // for overlays
	ActionEvent MidgroundRenderEvent; // for main gui / messages
	ActionEvent ForegroundRenderEvent; // for modal dialogues / popups



	// Banned operations for singleton
	ImGuiManager(const ImGuiManager& arg) = delete; // Copy constructor
	ImGuiManager(const ImGuiManager&& arg) = delete;  // Move constructor
	ImGuiManager& operator=(const ImGuiManager& arg) = delete; // Assignment operator
	ImGuiManager& operator=(const ImGuiManager&& arg) = delete; // Move operator

	static ImVec2 getScreenSize() { return D3D11Hook::getScreenSize(); }
	static ImVec2 getScreenCenter() { return D3D11Hook::getScreenCenter(); }
	static void debugInput();

};

