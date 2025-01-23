#pragma once
#include "ModuleHook.h"
#include "ModuleHookManager.h"
#include "PointerDataStore.h"

// imgui
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_stdlib.h"
// directx 11
#include <d3d11.h>

// TODO: move shit to impl so we can hind all the headers

// Define the DX functions we're going to hook
typedef HRESULT __stdcall DX11Present(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
typedef HRESULT __stdcall DX11ResizeBuffers(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);

//SCALAR TO ImVec4 OPERATIONS
inline ImVec4 operator + (const ImVec4& v1, float s) { return ImVec4(v1.x + s, v1.y + s, v1.z + s, v1.w + s); }
inline ImVec4 operator - (const ImVec4& v1, float s) { return ImVec4(v1.x - s, v1.y - s, v1.z - s, v1.w - s); }
inline ImVec4 operator * (const ImVec4& v1, float s) { return ImVec4(v1.x * s, v1.y * s, v1.z * s, v1.w * s); }
inline ImVec4 operator / (const ImVec4& v1, float s) { return ImVec4(v1.x / s, v1.y / s, v1.z / s, v1.w / s); }


//ImVec2 TO ImVec4 OPERATIONS
inline ImVec4 operator + (const ImVec4& v1, const ImVec4& v2) { return ImVec4(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w); }
inline ImVec4 operator - (const ImVec4& v1, const ImVec4& v2) { return ImVec4(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w); }
inline ImVec4 operator * (const ImVec4& v1, const ImVec4& v2) { return ImVec4(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w); }
inline ImVec4 operator / (const ImVec4& v1, const ImVec4& v2) { return ImVec4(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z, v1.w / v2.w); }


// Singleton: on construction, attaches hooks (VMT hook so our stuff shows up in OBS etc) to the games Present and ResizeBuffers function.
// The hooks will invoke presentHookEvent for other classes to listen to.
// On destruction, hooks will be unattached.


class D3D11Hook : public std::enable_shared_from_this<D3D11Hook>
{
private:
	static D3D11Hook* instance; 	// Private Singleton instance so static hooks/callbacks can access
	//static inline std::mutex mDestructionGuard{}; // Protects against Singleton destruction while hooks are executing
	static inline std::atomic_bool presentHookRunning = false;
	// Our hook functions
	static DX11Present newDX11Present;
	static DX11ResizeBuffers newDX11ResizeBuffers;

	static DX11Present newDX11PresentOBSBypass;
	static DX11Present mandatoryManualRender;

	std::weak_ptr<PointerDataStore> pointerDataStoreWeak; // not required but used to attempt to resolve vmt entries without needing a dummy swapchain

	safetyhook::VmtHook DXGIHook;
	safetyhook::VmHook presentHook; //  present hook
	safetyhook::VmHook resizebuffersHook; // resizeBuffers hook
	std::shared_ptr<ModuleInlineHook> OBSPresentHook; // obs hook for obs bypass


	// D3D data
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pDeviceContext = nullptr;
	ID3D11RenderTargetView* m_pMainRenderTargetView = nullptr;

	void CreateDummySwapchain(IDXGISwapChain*& pDummySwapchain, ID3D11Device*& pDummyDevice);
	void initializeD3Ddevice(IDXGISwapChain*);
	bool isD3DdeviceInitialized = false;

	static SimpleMath::Vector2 mScreenSize;
	static SimpleMath::Vector2 mScreenCenter;

public:

	explicit D3D11Hook(std::weak_ptr<PointerDataStore> pointerDataStore);
	~D3D11Hook();


	// Callback for present rendering
	std::shared_ptr<eventpp::CallbackList<void(ID3D11Device*, ID3D11DeviceContext*, IDXGISwapChain*, ID3D11RenderTargetView*)>> presentHookEvent = std::make_shared<eventpp::CallbackList<void(ID3D11Device*, ID3D11DeviceContext*, IDXGISwapChain*, ID3D11RenderTargetView*)>>();



	// Callback for window resize
	std::shared_ptr<eventpp::CallbackList<void(SimpleMath::Vector2 newScreenSize)>> resizeBuffersHookEvent = std::make_shared<eventpp::CallbackList<void(SimpleMath::Vector2 newScreenSize)>>();

	// Banned operations for singleton
	D3D11Hook(const D3D11Hook& arg) = delete; // Copy constructor
	D3D11Hook(const D3D11Hook&& arg) = delete;  // Move constructor
	D3D11Hook& operator=(const D3D11Hook& arg) = delete; // Assignment operator
	D3D11Hook& operator=(const D3D11Hook&& arg) = delete; // Move operator

	SimpleMath::Vector2 getScreenSize() { return mScreenSize; }
	SimpleMath::Vector2 getScreenCenter() { return mScreenCenter; }

	void beginHook();

	void setOBSBypass(bool enabled);
};
