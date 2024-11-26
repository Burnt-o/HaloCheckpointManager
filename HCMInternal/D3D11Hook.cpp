#include "pch.h"
#include "D3D11Hook.h"

#include "GlobalKill.h"
#include <dxgi.h>
#pragma comment(lib, "dxgi")

#include "Renderer2D.h"
#include "Lapua.h"
#include "safetyhook.hpp"



D3D11Hook* D3D11Hook::instance = nullptr;
SimpleMath::Vector2 D3D11Hook::mScreenSize{1920, 1080};
SimpleMath::Vector2 D3D11Hook::mScreenCenter{960, 540};

struct rgba {
	float r, g, b, a;
	rgba(float ir, float ig, float ib, float ia) : r(ir), g(ig), b(ib), a(ia) {}
};


enum class IDXGISwapChainVMT {
	QueryInterface,
	AddRef,
	Release,
	SetPrivateData,
	SetPrivateDataInterface,
	GetPrivateData,
	GetParent,
	GetDevice,
	Present,
	GetBuffer,
	SetFullscreenState,
	GetFullscreenState,
	GetDesc,
	ResizeBuffers,
	ResizeTarget,
	GetContainingOutput,
	GetFrameStatistics,
	GetLastPresentCount,
};


const std::map<D3D_DRIVER_TYPE, std::string> driverToString
{
	{ D3D_DRIVER_TYPE_HARDWARE, "D3D_DRIVER_TYPE_HARDWARE"},
	{ D3D_DRIVER_TYPE_WARP, "D3D_DRIVER_TYPE_WARP" },
	{ D3D_DRIVER_TYPE_REFERENCE, "D3D_DRIVER_TYPE_REFERENCE" },
	{ D3D_DRIVER_TYPE_SOFTWARE, "D3D_DRIVER_TYPE_SOFTWARE" },
	{ D3D_DRIVER_TYPE_UNKNOWN, "D3D_DRIVER_TYPE_UNKNOWN" },

};


void D3D11Hook::CreateDummySwapchain(IDXGISwapChain*& pDummySwapchain, ID3D11Device*& pDummyDevice)
{

	std::vector<std::string> errorCodes;

#define logSwapChainFailure(x) PLOG_ERROR << x; errorCodes.push_back(x)

	D3D_FEATURE_LEVEL featLevel;
	DXGI_SWAP_CHAIN_DESC sd{ 0 };
	sd.BufferCount = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.Height = 800;
	sd.BufferDesc.Width = 600;
	sd.BufferDesc.RefreshRate = { 60, 1 };
	sd.OutputWindow = GetForegroundWindow();
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;



	// the following attempts at D3D11CreateDeviceAndSwapChain differ in the driver_type param
	// https://learn.microsoft.com/en-us/windows/win32/api/d3dcommon/ne-d3dcommon-d3d_driver_type



	// use D3D_DRIVER_TYPE_HARDWARE
	// works for me
	{
		auto driverToTry = D3D_DRIVER_TYPE_HARDWARE;
		PLOG_DEBUG << "Attempting to create dummy swap chain with driver type: " << driverToString.at(driverToTry);
		HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, driverToTry, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &sd, &pDummySwapchain, &pDummyDevice, &featLevel, nullptr);
		if (SUCCEEDED(hr))
		{
			PLOG_INFO << "Succesfully created dummy swapchain with driver type " << driverToString.at(driverToTry);
			return;
		}
		else
		{
			logSwapChainFailure(std::format("6: failed to create dummy d3d device and swapchain with driver type {}, error: {:x}", driverToString.at(driverToTry), (ULONG)hr));
		}
	}

	// use D3D_DRIVER_TYPE_WARP
	// Seems to be the most consistent
	{
		auto driverToTry = D3D_DRIVER_TYPE_WARP;
		PLOG_DEBUG << "Attempting to create dummy swap chain with driver type: " << driverToString.at(driverToTry);
		HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, driverToTry, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &sd, &pDummySwapchain, &pDummyDevice, &featLevel, nullptr);
		if (SUCCEEDED(hr))
		{
			PLOG_INFO << "Succesfully created dummy swapchain with driver type " << driverToString.at(driverToTry);
			return;
		}
		else
		{
			logSwapChainFailure(std::format("6: failed to create dummy d3d device and swapchain with driver type {}, error: {:x}", driverToString.at(driverToTry), (ULONG)hr));
		}
	}

	// use D3D_DRIVER_TYPE_REFERENCE. The original code I used.
	// works for me but not gronchy
	{
		auto driverToTry = D3D_DRIVER_TYPE_REFERENCE;
		PLOG_DEBUG << "Attempting to create dummy swap chain with driver type: " << driverToString.at(driverToTry);
		HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, driverToTry, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &sd, &pDummySwapchain, &pDummyDevice, &featLevel, nullptr);
		if (SUCCEEDED(hr))
		{
			PLOG_INFO << "Succesfully created dummy swapchain with driver type " << driverToString.at(driverToTry);
			return;
		}
		else
		{
			logSwapChainFailure(std::format("6: failed to create dummy d3d device and swapchain with driver type {}, error: {:x}", driverToString.at(driverToTry), (ULONG)hr));
		}
	}



	// use D3D_DRIVER_TYPE_SOFTWARE
	// does not work for me (E_INVALIDARG)
	{
		auto driverToTry = D3D_DRIVER_TYPE_SOFTWARE;
		PLOG_DEBUG << "Attempting to create dummy swap chain with driver type: " << driverToString.at(driverToTry);
		HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, driverToTry, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &sd, &pDummySwapchain, &pDummyDevice, &featLevel, nullptr);
		if (SUCCEEDED(hr))
		{
			PLOG_INFO << "Succesfully created dummy swapchain with driver type " << driverToString.at(driverToTry);
			return;
		}
		else
		{
			logSwapChainFailure(std::format("6: failed to create dummy d3d device and swapchain with driver type {}, error: {:x}", driverToString.at(driverToTry), (ULONG)hr));
		}
	}


	// the next call seems to need us to set the adapter parameter ourselves instead of using null ptr

#define UseDXGI1 TRUE

#if UseDXGI1 == TRUE
#define IDXGIFactoryA IDXGIFactory1
#define CreateDXGIFactoryA CreateDXGIFactory1
#define IDXGIAdapterA IDXGIAdapter1
#define EnumAdaptersA EnumAdapters1
#else
#define IDXGIFactoryA IDXGIFactory
#define CreateDXGIFactoryA CreateDXGIFactory
#define IDXGIAdapterA IDXGIAdapter
#define EnumAdaptersA EnumAdapters
#endif

	IDXGIFactoryA* pFactory;
	HRESULT fhr = CreateDXGIFactoryA(__uuidof(IDXGIFactoryA), (void**)(&pFactory));

	std::vector <IDXGIAdapterA*> vAdapters;
	if (fhr != S_OK)
	{
		PLOG_ERROR << "Failed to create DXGIFactory: " << std::hex << (ULONG)fhr;
	}
	else
	{
		UINT i = 0;
		IDXGIAdapterA* pAdapter;
		while (pFactory->EnumAdaptersA(i, &pAdapter) != DXGI_ERROR_NOT_FOUND)
		{
			vAdapters.push_back(pAdapter);
			++i;
		}
		PLOG_DEBUG << "default adapter: " << std::hex << vAdapters.front();
		PLOG_DEBUG << "adapter count: " << vAdapters.size();

	}

	// use D3D_DRIVER_TYPE_UNKNOWN
	// works if I pass it the adapter manually
	// tries each adapter
	PLOG_DEBUG << "Attempting to create dummy swapchain with driver type " << driverToString.at(D3D_DRIVER_TYPE_UNKNOWN);
	for (auto adapter : vAdapters)
	{
		PLOG_INFO << "Attemtping D3D11CreateDeviceAndSwapChain with adapter: " << std::hex << adapter;

		HRESULT hr = D3D11CreateDeviceAndSwapChain(adapter, D3D_DRIVER_TYPE_UNKNOWN, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &sd, &pDummySwapchain, &pDummyDevice, &featLevel, nullptr);
		if (SUCCEEDED(hr))
		{
			PLOG_INFO << "Succesfully created dummy swapchain with " << driverToString.at(D3D_DRIVER_TYPE_UNKNOWN);
			return;
		}
		else
		{
			logSwapChainFailure(std::format("4: failed to create dummy d3d device and swapchain with driver type {}, error: {:x}", driverToString.at(D3D_DRIVER_TYPE_UNKNOWN), (ULONG)hr));
		}

	}



	// If execution got here, none of the createdevice calls succeeded. Throw an exception and log the error codes

	std::string resultsString;
	for (auto error : errorCodes)
	{
		resultsString += error + "\n";
	}
	throw HCMInitException(resultsString);


}


D3D11Hook::D3D11Hook(std::weak_ptr<PointerDataStore> pointerDataStore)
	: pointerDataStoreWeak(pointerDataStore)
{

	if (instance != nullptr)
	{
		throw HCMInitException("Cannot have more than one D3D11Hook");
	}
	instance = this;







}





void D3D11Hook::initializeD3Ddevice(IDXGISwapChain* pSwapChain)
{

	PLOG_DEBUG << "Initializing D3Ddevice" << std::endl;

	if (!SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&m_pDevice)))
	{
		throw HCMInitException(std::format("Failed to get D3D11Device, pSwapChain: {:x}", (uint64_t)pSwapChain).c_str());

	}

	// Get Device Context
	m_pDevice->GetImmediateContext(&m_pDeviceContext);
	if (!m_pDeviceContext)
	{
		throw HCMInitException("Failed to get DeviceContext");
	}



	// Use backBuffer to get MainRenderTargetView
	ID3D11Texture2D* pBackBuffer;
	pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	if (!pBackBuffer) throw HCMInitException("Failed to get BackBuffer");

	// Store the windows size
	D3D11_TEXTURE2D_DESC desc;
	pBackBuffer->GetDesc(&desc);
	PLOG_INFO << "Initializing screen size: " << desc.Width << ", " << desc.Height;
	mScreenSize = { ((float)desc.Width), ((float)desc.Height) };
	mScreenCenter = mScreenSize / 2;

#ifdef HCM_DEBUG
	DXGI_SWAP_CHAIN_DESC sd;
	pSwapChain->GetDesc(&sd);

	PLOG_DEBUG << "swapchain image format: " << sd.BufferDesc.Format;
	// result: DXGI_FORMAT_R8G8B8A8_UNORM
#endif

	PLOG_INFO << "Creating render target view";
	auto CreateRenderTargetViewResult = m_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pMainRenderTargetView);
	PLOG_DEBUG << "CreateRenderTargetViewResult: " << CreateRenderTargetViewResult;
	pBackBuffer->Release();
	if (CreateRenderTargetViewResult || !m_pMainRenderTargetView) throw HCMInitException(std::format("Failed to get MainRenderTargetView, error code: {}", CreateRenderTargetViewResult));




}

// static 
HRESULT D3D11Hook::newDX11Present(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{


	ScopedAtomicBool lock(presentHookRunning);
	//std::unique_lock<std::mutex> lock(mDestructionGuard); // Protects against D3D11Hook singleton destruction while hooks are executing
	LOG_ONCE(PLOG_DEBUG << "D3D11Hook::newDX11Present");
	auto d3d = instance;
	if (!d3d)
	{
		PLOG_FATAL << "d3d instance was null at newDX11Present";
		return 0;
	}

	//auto guard = d3d->shared_from_this();




	if (!d3d->isD3DdeviceInitialized)
	{
		PLOG_DEBUG << "Initializing d3d device @ present";
		try
		{
			d3d->initializeD3Ddevice(pSwapChain);
			d3d->isD3DdeviceInitialized = true;
			PLOG_DEBUG << "D3D device initialized t. newDX11Present";
			// fire a resizeborders event
			d3d->resizeBuffersHookEvent->operator()(mScreenSize);
			PLOG_DEBUG << "resizeBuffersHookEvent fired";
		}
		catch (HCMInitException& ex)
		{
			PLOG_FATAL << "Failed to initialize d3d device, info: " << std::endl
				<< ex.what() << std::endl
				<< "HCM will now automatically close down";
			GlobalKill::killMe();

			// Call original present
			return d3d->m_pOriginalPresent(pSwapChain, SyncInterval, Flags);


		}

	}
	LOG_ONCE(PLOG_VERBOSE << "invoking mandatoryPresentHookEvent callback");
	// Invoke the callback


	if (!d3d->dxgiInternalPresentHook || d3d->dxgiInternalPresentHook->isHookInstalled() == false || Renderer2D::good == false)
	{


		LOG_ONCE(PLOG_VERBOSE << "invoking mainPresentHookEvent callback via vmt");
		d3d->presentHookEvent->operator()(d3d->m_pDevice, d3d->m_pDeviceContext, pSwapChain, d3d->m_pMainRenderTargetView);
		Lapua::shouldRunForBypass = false;

		if (Lapua::shouldRunForWatermark)
		{
			if (Renderer2D::good == false)
			{
				Lapua::lapuaGood = false;
			}
			else
			{
				Renderer2D::render(d3d->m_pDevice, d3d->m_pDeviceContext, mScreenSize, d3d->m_pMainRenderTargetView);
			}
		}

	}
	else
	{
		Lapua::shouldRunForBypass = true;
		Renderer2D::render(d3d->m_pDevice, d3d->m_pDeviceContext, mScreenSize, d3d->m_pMainRenderTargetView);
	}



	LOG_ONCE(PLOG_VERBOSE << "calling original present function");
	// Call original present

	auto hr = d3d->m_pOriginalPresent(pSwapChain, SyncInterval, Flags);

	return hr;

}

HRESULT D3D11Hook::newDX11PresentOBSBypass(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	LOG_ONCE(PLOGV << "newDX11PresentOBSBypass");

	// DON'T LOCK! already locked by present!
	//ScopedAtomicBool lock(presentHookRunning);
	auto d3d = instance;
	if (!d3d)
	{
		PLOG_FATAL << "d3d instance was null at newDX11PresentOBSBypass";
		return 0;
	}

	LOG_ONCE(PLOG_VERBOSE << "invoking mainPresentHookEvent callback via inline obs bypass");
	d3d->presentHookEvent->operator()(d3d->m_pDevice, d3d->m_pDeviceContext, pSwapChain, d3d->m_pMainRenderTargetView);
	auto hr = d3d->dxgiInternalPresentHook->getInlineHook().call<HRESULT, IDXGISwapChain*, UINT, UINT>(pSwapChain, SyncInterval, Flags);

	return hr;
}

// static
HRESULT D3D11Hook::newDX11ResizeBuffers(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
{
	ScopedAtomicBool lock(presentHookRunning);
	//std::unique_lock<std::mutex> lock(mDestructionGuard); // Protects against D3D11Hook singleton destruction while hooks are executing
	auto d3d = instance;
	if (!d3d)
	{
		PLOG_FATAL << "d3d instance was null at newDX11ResizeBuffers";
	}

	//auto guard = d3d->shared_from_this();

	if (!d3d->isD3DdeviceInitialized)
	{
		PLOG_DEBUG << "Initializing d3d device @ resizeBuffers";
		try
		{
			d3d->initializeD3Ddevice(pSwapChain);
			d3d->isD3DdeviceInitialized = true;
			PLOG_DEBUG << "D3D device initialized t. newDX11ResizeBuffers";
		}
		catch (HCMInitException& ex)
		{
			PLOG_FATAL << "Failed to initialize d3d device, info: " << std::endl
				<< ex.what() << std::endl
				<< "HCM will now automatically close down";
			GlobalKill::killMe();

			// Call original resize buffers
			return d3d->m_pOriginalResizeBuffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);


		}

	}



	// Need to release mainRenderTargetView before calling ResizeBuffers
	if (d3d->m_pMainRenderTargetView != nullptr)
	{
		d3d->m_pDeviceContext->OMSetRenderTargets(0, 0, 0);
		d3d->m_pMainRenderTargetView->Release();
	}

	// Call original ResizeBuffers
	HRESULT hr = d3d->m_pOriginalResizeBuffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);


	// Resetup the mainRenderTargetView
	ID3D11Texture2D* pBackBuffer;
	pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
	if (!pBackBuffer) throw HCMInitException("Failed to get BackBuffer");

	d3d->m_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &d3d->m_pMainRenderTargetView);
	pBackBuffer->Release();
	if (!d3d->m_pMainRenderTargetView) throw HCMInitException("Failed to get MainRenderTargetView");

	// Grab the new windows size
	mScreenSize = { (float)Width, (float)Height };
	mScreenCenter = mScreenSize / 2;

	// fire screen resize event
	d3d->resizeBuffersHookEvent->operator()(mScreenSize);

	return hr;
}

// Safely destroys hooks
// Releases D3D resources, if we acquired them
D3D11Hook::~D3D11Hook()
{
	if (m_pOriginalPresent)
	{
		PLOG_INFO << "~D3D11Hook()";
		PLOG_INFO << "Freezing threads";
		safetyhook::freeze_and_execute([this]() { // freeze threads while we patch vmt


			PLOG_INFO << "Unpatching present pointer";
			// rewrite the pointers to go back to the original value
			patch_pointer(m_ppPresent, (uintptr_t)m_pOriginalPresent);
			// resizeBuffers too
			patch_pointer(m_ppResizeBuffers, (uintptr_t)m_pOriginalResizeBuffers);

			if (dxgiInternalPresentHook)
				dxgiInternalPresentHook.reset();

			PLOG_INFO << "Successfully unpatched present pointer!";

			}); 

	}


	if (presentHookRunning)
	{
		PLOG_INFO << "Waiting for presentHook to finish execution";
		presentHookRunning.wait(true);
	}
	instance = nullptr;

	// D3D resource releasing:
	// need to call release on the device https://learn.microsoft.com/en-us/windows/win32/api/d3d9helper/nf-d3d9helper-idirect3dswapchain9-getdevice

	safe_release(m_pDevice);
	safe_release(m_pDeviceContext);
	safe_release(m_pMainRenderTargetView);



}


void D3D11Hook::beginHook()
{
	PLOG_DEBUG << "beginning vmt hook";


	// set up d3d11 hook. This requires us to get the vmt of Present and ResizeBuffers functions. 
		   // There are two ways to do this: One is to just get a pointer to this from our pointer data. 
		   // But failing that, we can create a dummy swapchain and get the vmt out of that.
		   // The first option is preferred as creating (more specifically, releasing) a dummy swapchain can cause overlays like RivaTuner to crash.

	try
	{
		lockOrThrow(pointerDataStoreWeak, ptr);

		uintptr_t ppPresent;
		uintptr_t ppResizeBuffers;

		auto pppPresent = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(pppPresent));
		if (!pppPresent->resolve(&ppPresent)) throw HCMInitException(std::format("Could not resolve pppPresent: {}", MultilevelPointer::GetLastError()));

		auto pppResizeBuffers = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(pppResizeBuffers));
		if (!pppResizeBuffers->resolve(&ppResizeBuffers)) throw HCMInitException(std::format("Could not resolve pppResizeBuffers: {}", MultilevelPointer::GetLastError()));

		PLOG_INFO << "Successfully resolved multilevelpointers to present/resizeBuffer VMT entries!";
		PLOG_DEBUG << "ppPresent: " << ppPresent;
		PLOG_DEBUG << "ppResizeBuffers: " << ppResizeBuffers;

		m_ppPresent = (DX11Present**)ppPresent;
		m_ppResizeBuffers = (DX11ResizeBuffers**)ppResizeBuffers;
		m_pOriginalPresent = *m_ppPresent;
		m_pOriginalResizeBuffers = *m_ppResizeBuffers;

	}
	catch (HCMInitException ex)
	{
		PLOG_ERROR << "Could not resolve directX vmt. Using dummy swapchain instead (may cause crash with overlays eg RTSS): " << std::endl << ex.what();
	}
	catch (HCMRuntimeException ex)
	{
		PLOG_ERROR << "Failed to lock pointer Manager service somehow?! Using dummy swapchain instead (may cause crash with overlays eg RTSS): " << std::endl << ex.what();
	}



	if (m_ppPresent == nullptr || m_ppResizeBuffers == nullptr) // ie trycatch above ended up throwing
	{
		ID3D11Device* pDummyDevice = nullptr;
		IDXGISwapChain* pDummySwapchain = nullptr;

		// Create a dummy device
		CreateDummySwapchain(pDummySwapchain, pDummyDevice);

		// Get swapchain vmt
		void** pVMT = *(void***)pDummySwapchain;

		// Get Present's address out of vmt
		m_pOriginalPresent = (DX11Present*)pVMT[(UINT)IDXGISwapChainVMT::Present];
		m_ppPresent = (DX11Present**)&pVMT[(UINT)IDXGISwapChainVMT::Present];
		PLOG_INFO << "PRESENT: " << m_pOriginalPresent;
		PLOG_INFO << "PRESENT pointer: " << m_ppPresent;

		// Get resizeBuffers too
		m_pOriginalResizeBuffers = (DX11ResizeBuffers*)pVMT[(UINT)IDXGISwapChainVMT::ResizeBuffers];
		m_ppResizeBuffers = (DX11ResizeBuffers**)&pVMT[(UINT)IDXGISwapChainVMT::ResizeBuffers];
		PLOG_INFO << "RESIZEBUFFERS: " << m_pOriginalResizeBuffers;
		PLOG_INFO << "RESIZEBUFFERS pointer: " << m_ppResizeBuffers;

		// Don't need the dummy device anymore
		// it appears that releasing these can cause the game to crash when using RivaTuner.
		//safe_release(pDummySwapchain);
		//safe_release(pDummyDevice);
	}





	PLOG_DEBUG << "rewriting present pointer";


	// freeze threads while we patch vmt
	safetyhook::freeze_and_execute([this]() {

		// Rewrite the present pointer to instead point to our newPresent
		// Need access tho!
		patch_pointer(m_ppPresent, (uintptr_t)&newDX11Present);
		// resizeBuffers too
		patch_pointer(m_ppResizeBuffers, (uintptr_t)&newDX11ResizeBuffers);

		});

}




void D3D11Hook::setOBSBypass(bool enabled)
{
	PLOGV << "D3D11Hook::setOBSBypass called with value: " << enabled;
	try
	{
		if (enabled)
		{
			if (presentHookRunning)
			{
				PLOG_INFO << "Waiting for presentHook to finish execution A";
				presentHookRunning.wait(true);
			}


			lockOrThrow(pointerDataStoreWeak, ptr);
			auto dxgiInternalPresentFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(dxgiInternalPresentFunction));

			// starts attached
			//dxgiInternalPresentHook = ModuleInlineHook::make(L"dxgi.dll", dxgiInternalPresentFunction, newDX11PresentOBSBypass, true);
			dxgiInternalPresentHook = ModuleInlineHook::make(L"graphics-hook64.dll", dxgiInternalPresentFunction, newDX11PresentOBSBypass, true);
		}
		else
		{
			if (!dxgiInternalPresentHook) return; // don't need to reset if it's already reset

			if (presentHookRunning)
			{
				PLOG_INFO << "Waiting for presentHook to finish execution B";
				presentHookRunning.wait(true);
			}


			dxgiInternalPresentHook.reset();
		}

	}
	catch (HCMInitException ex)
	{
		throw HCMRuntimeException(ex);
	}
}