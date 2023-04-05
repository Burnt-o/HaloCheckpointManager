#pragma once
#include "includes.h"
//#include <Windows.h>
#include "hookd3d.h"
#include <d3d11.h>
#include <dxgi.h>
#include <iostream>
//#include "imgui/imgui.h"

#include <thread>
#include <chrono>
#include "messages.h"
//#include <memory.h>
#include "TriggerData.h"
#include <d3dx11.h>
#include "3Drender.h"

//#include "FW1FontWrapper/FW1FontWrapper.h"






// Forward Declares
extern "C" __declspec(dllexport) BOOL WINAPI IsHookEnabled();
extern "C" __declspec(dllexport) BOOL WINAPI EnableOverlay(UINT64 * PtrToPresentPtr);
extern "C" __declspec(dllexport) BOOL WINAPI DisableOverlay(UINT64 * PtrToPresentPtr);
bool InitializeD3Ddevice(IDXGISwapChain* pSwapChain);
LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
extern "C" __declspec(dllexport) HRESULT __stdcall hkPresent(IDXGISwapChain * pSwapChain, UINT SyncInterval, UINT Flags);
extern "C" __declspec(dllexport) HRESULT __stdcall hkResizeBuffers(IDXGISwapChain * pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void ReleaseD3Ddevice();


// Define the d3d11 Present and ResizeBuffer functions that we will be hooking
typedef HRESULT(__stdcall* Present) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
typedef HRESULT(__stdcall* ResizeBuffers)(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);

// Global Variables
bool g_hookEnabled = false; // Control - true when games PresentPtr has been redirected to hkPresent (ie the below vars shouldn't be null)
Present g_oPresent = nullptr; // Pointer to the games original Present function
ResizeBuffers g_oResizeBuffers = nullptr; // Pointer to the games original ResizeBuffers function

bool g_D3DdeviceInitialized = false; // Control - true if d3d device initialized by hkPresent (ie the below vars shouldn't be null)
HWND g_window = nullptr;
ID3D11Device* g_pDevice = nullptr;
ID3D11DeviceContext* g_pContext = nullptr;
ID3D11RenderTargetView* g_pMainRenderTargetView = nullptr;
WNDPROC g_oWndProc = nullptr;
void* lastViewport = nullptr;

float g_screenWidth = 1920.f;
float g_screenHeight = 1080.f;
float g_aspectRatio = g_screenWidth / g_screenHeight;

bool pauseForInitRelease = false;
//
ImFont* g_defaultFont = nullptr; // ImGui font, initialized by hookd3d and used by draw
//
//
//







// Passed a pointer to the games Ptr to the original Present function. ResizeBuffers at +0x28 from that.
extern "C" __declspec(dllexport) BOOL WINAPI EnableOverlay(UINT64 * PtrToPresentPtr)
{



	std::cout << "EnableOverlay called." << std::endl;
	// Need to wait for init
	if (!g_hcmInitialised) { std::cout << "g_hcmInitialised was false so bailing." << std::endl; return FALSE; }

	// Hook already enabled
	if (g_hookEnabled) { std::cout << "g_hookEnabled was true so hook was already enabled" << std::endl; return TRUE; }

	if (IsBadReadPtr(PtrToPresentPtr, 8))
	{
		std::cout << "PtrToPresentPtr passed to EnableOverlay was invalid 1: " << PtrToPresentPtr << std::endl;
		return FALSE;
	}

	// Get the games Present pointer from passed parameter
	UINT64* presentPtr = reinterpret_cast<UINT64*>(*PtrToPresentPtr);
	std::cout << "presentPtr: " << presentPtr << std::endl;

	// Check if it's valid. If it's valid, ResizeBuffers will be too.
	if (IsBadReadPtr(presentPtr, 8))
	{
		std::cout << "PtrToPresentPtr passed to EnableOverlay was invalid 2: " << PtrToPresentPtr << std::endl;
		return FALSE;
	}

	std::cout << "PresentPtr is at location: " << *presentPtr << std::endl;
	UINT64 currentPresentPtr = *presentPtr; // Value of the games present pointer - before hooking, it will be the games original Present function
	std::cout << "Value of currentPresentPtr is " << currentPresentPtr << std::endl;

	// Check if the value of the games present pointer is already equal to hkPresent - in which case we're already done
	if (currentPresentPtr == (UINT64)hkPresent)
	{
		std::cout << "Hook already appears to be enabled, bailing" << std::endl;
		return TRUE;
	}

	// Get the games ResizeBuffers pointer too, it's 0x28 ahead of the Present pointer
	UINT64* ResizePtr = reinterpret_cast<UINT64*>(*PtrToPresentPtr + 0x28);
	std::cout << "ResizePtr is at location" << *ResizePtr << std::endl;
	UINT64 currentResizePtr = *ResizePtr;
	std::cout << "Value of currentResizePtr is " << currentResizePtr << std::endl;

	// Store the current value of the games Present Pointer as oPresent so hkPresent can passthrough to it (and so we can undo the hook later)
	g_oPresent = (Present)currentPresentPtr;
	std::cout << "oPresent stored: " << (UINT64)g_oPresent << std::endl;

	// Redirect currentPresentPtr to hkPresent
	std::cout << "Setting presentPtr to hkPresent, newval: " << (UINT64)hkPresent << std::endl;
	UINT64 hkPresentPtr = (UINT64)hkPresent;
	DWORD dwNewProtect, dwOldProtect;
	VirtualProtect(presentPtr, 8, PAGE_EXECUTE_READWRITE, &dwNewProtect);
	memcpy(presentPtr, &hkPresentPtr, 8);
	VirtualProtect(presentPtr, 8, dwNewProtect, &dwOldProtect);


	// Check if ResizeBuffers already hooked - if not, also store oResizeBuffers and redirect games ResizeBuffersPtr to hkResizeBuffers
	if (currentResizePtr != (UINT64)hkResizeBuffers)
	{
		// Store current value
		g_oResizeBuffers = (ResizeBuffers)currentResizePtr;

		// Redirect currentResizePtr to hkResizeBuffers
		std::cout << "Setting ResizePtr to hkResizeBuffers, newval: " << (UINT64)hkResizeBuffers << std::endl;
		UINT64 hkResizeBuffersPtr = (UINT64)hkResizeBuffers;
		DWORD dwNewProtect, dwOldProtect;
		VirtualProtect(ResizePtr, 8, PAGE_EXECUTE_READWRITE, &dwNewProtect);
		memcpy(ResizePtr, &hkResizeBuffersPtr, 8);
		VirtualProtect(ResizePtr, 8, dwNewProtect, &dwOldProtect);
	}
	g_hookEnabled = true;
	return TRUE;

}


extern "C" __declspec(dllexport) BOOL WINAPI DisableOverlay(UINT64 * PtrToPresentPtr)
{
	pauseForInitRelease = true;
	std::this_thread::sleep_for(std::chrono::milliseconds(10)); // wait for hkPresent to finish
	//if oPresent is null then hook was never applied in the first place 
	if (g_oPresent == nullptr) { return TRUE; }

	// Get the current value of the games presentPtr
	UINT64* presentPtr = reinterpret_cast<UINT64*>(*PtrToPresentPtr);
	if (IsBadReadPtr(presentPtr, 8))
	{
		std::cout << "PtrToPresentPtr passed to DisableOverlay was invalid: " << PtrToPresentPtr << std::endl;
		return FALSE;
	}
	std::cout << "PresentPtr is at location" << *presentPtr << std::endl;

	UINT64 currentPresentPtr = *presentPtr;
	std::cout << "games present pointer is pointer to: " << presentPtr << std::endl;

	// Original value of Present which we stored when hook was enabled
	UINT64 oPresent = (UINT64)g_oPresent;
	std::cout << "original present address: " << oPresent << std::endl;


	//Redirect the games presentPtr to again point to the original Present function
	if (currentPresentPtr != oPresent)
	{
		DWORD dwNewProtect, dwOldProtect;
		VirtualProtect(presentPtr, 8, PAGE_EXECUTE_READWRITE, &dwNewProtect);
		memcpy(presentPtr, &oPresent, 8);
		VirtualProtect(presentPtr, 8, dwNewProtect, &dwOldProtect);
	}


	// 	// Get the games ResizeBuffers pointer too, it's 0x28 ahead of the Present pointer
	UINT64* ResizePtr = reinterpret_cast<UINT64*>(*PtrToPresentPtr + 0x28);
	std::cout << "ResizePtr is at location" << *ResizePtr << std::endl;

	UINT64 currentResizePtr = *ResizePtr;
	std::cout << "Value of currentResizePtr is " << currentResizePtr << std::endl;

	UINT64 oResizeBuffers = (UINT64)g_oResizeBuffers;
	std::cout << "original present address: " << oPresent << std::endl;

		//Redirect the games presentPtr to again point to the original Present function
	if (currentResizePtr != oResizeBuffers)
	{
		DWORD dwNewProtect, dwOldProtect;
		VirtualProtect(ResizePtr, 8, PAGE_EXECUTE_READWRITE, &dwNewProtect);
		memcpy(ResizePtr, &g_oResizeBuffers, 8);
		VirtualProtect(ResizePtr, 8, dwNewProtect, &dwOldProtect);
	}

	// Need to release d3d device resources
	ReleaseD3Ddevice();





	
	//Nullify the g_oPresent so it's ready for enabling hook again
	g_oPresent = nullptr;
	g_hookEnabled = false;
	pauseForInitRelease = false;
	return TRUE;
}



void ReleaseD3Ddevice()
{
	
	if (g_pDevice != NULL)
	{
		/*renderer->~Renderer();*/
		g_pDevice->Release();
		g_pContext->Release();
		g_pMainRenderTargetView->Release();
	}
	g_D3DdeviceInitialized = false;
}


bool InitializeD3Ddevice(IDXGISwapChain* pSwapChain)
{
	pauseForInitRelease = true;
	std::this_thread::sleep_for(std::chrono::milliseconds(10)); // wait for hkPresent to finish
	std::cout << "Initializing D3Ddevice" << std::endl;
	if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&g_pDevice)))
	{
		std::cout << "Succesfully got the id of the d3d11 device: " << g_pDevice << std::endl;

		// Get Device Context
		g_pDevice->GetImmediateContext(&g_pContext); 
		std::cout << "Got the device context: " << g_pContext << std::endl;

		// Use swap chain description to get MCC window handle
		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		pSwapChain->GetDesc(&swapChainDesc);
		g_window = swapChainDesc.OutputWindow; 
		std::cout << "Got MCC's window handle: " << g_window << std::endl;

		g_screenHeight = swapChainDesc.BufferDesc.Height;
		g_screenWidth = swapChainDesc.BufferDesc.Width;
		g_aspectRatio = g_screenWidth / g_screenHeight;


		// Use backBuffer to get MainRenderTargetView
		ID3D11Texture2D* pBackBuffer;
		pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
		g_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_pMainRenderTargetView);
		pBackBuffer->Release();
		std::cout << "Got the MainRenderTargetView: " << g_pMainRenderTargetView << std::endl;

		// Get window callback function
		if (g_oWndProc == nullptr) // We only need to do this once, doing it a second time causes a crash
		{
			g_oWndProc = (WNDPROC)SetWindowLongPtr(g_window, GWLP_WNDPROC, (LONG_PTR)WndProc);
			std::cout << "Got the window callback: " << g_oWndProc << std::endl;
		}
		else { std::cout << "Skipped getting window callback, already have it: " << g_oWndProc << std::endl; }




		 //Setup ImGui stuff
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;

		if (!ImGui_ImplWin32_Init(g_window)) {
			std::cout << "ImGui_ImplWin32_Init failed w/ " << g_window << std::endl;
			return false;
		};
		if (!ImGui_ImplDX11_Init(g_pDevice, g_pContext)) {
			std::cout << "ImGui_ImplDX11_Init failed w/ " << g_pDevice << ", " << g_pContext << std::endl;
			return false;
		};
		
		ImGui_ImplDX11_NewFrame(); // need to get a new frame to load default font
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		ImGui::EndFrame();
		g_defaultFont = ImGui::GetIO().Fonts->Fonts[0]; // this is crashing. Fonts[0] is invalid?
		std::cout << "Initialized ImGui." << std::endl;

		//std::cout << "Setting up renderer: " << std::endl;
		//// Setup renderer
		////renderer = std::make_unique<Renderer>(g_pDevice, L"Consolas");
		//renderer = new Renderer(g_pDevice, (float)swapChainDesc.BufferDesc.Width, (float)swapChainDesc.BufferDesc.Height, L"Consolas");
		//std::cout << "Renderer setup done! " << std::endl;
		//D3D11_VIEWPORT viewport{};
		//UINT numViewports = 1;
		//g_pContext->RSGetViewports(&numViewports, &viewport);
		//std::cout << "address of viewport: " << std::hex << &viewport << std::endl;
		//lastViewport = &viewport;

		
		std::cout << "Succesfully initialised d3d hook." << std::endl;
		pauseForInitRelease = false;
		return true;
	}
	else
		return false;

}




LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{

	if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProc(g_oWndProc, hWnd, uMsg, wParam, lParam);
}






int testForViewportChange = 0;





extern "C" __declspec(dllexport) HRESULT __stdcall hkPresent(IDXGISwapChain * pSwapChain, UINT SyncInterval, UINT Flags)
{

	if (pauseForInitRelease) return g_oPresent(pSwapChain, SyncInterval, Flags);


	if (g_oPresent == NULL)
	{
		// We have a serious problem
		std::cout << "g_oPresent was null when calling hkPresent. Game is about to crash" << std::endl;
	}

	// Do we need to initialize the d3d device? If so, initialize it!
	if (!g_D3DdeviceInitialized)
	{

		if (InitializeD3Ddevice(pSwapChain)) // Success!
		{
			g_D3DdeviceInitialized = true;
		}
		else // Failed to initialize! Release resources then return oPresent, we'll try again next time
		{
			std::cout << "ERROR INITIALIZING D3D DEVICE" << std::endl;
			// Need to release d3d device resources
			if (g_pDevice != NULL)
			{
				g_pDevice->Release();
				g_pContext->Release();
			}
			return g_oPresent(pSwapChain, SyncInterval, Flags);
		}

	}

	// Setup ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();


	// I don't 100% understand what this does, but it must be done before we try to render
	g_pContext->OMSetRenderTargets(1, &g_pMainRenderTargetView, NULL);

	// Time to draw stuff!
	ImGui::GetOverlayDrawList()->AddText(ImVec2(10, 10), ImGui::ColorConvertFloat4ToU32(ImVec4(1, 0, 1, 1)), ("Test string"));
	//TestRenderBoxWithImgui();

	//PrintActiveMessages();
	//PrintTemporaryMessages();
	//DrawDisplayInfo();
	render3D();

	// Finish ImGui frame
	ImGui::EndFrame();
	ImGui::Render();
	

	// Render it !
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	

	// Call original present
	return g_oPresent(pSwapChain, SyncInterval, Flags);
}



extern "C" __declspec(dllexport) HRESULT __stdcall hkResizeBuffers(IDXGISwapChain * pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
{
	std::cout << "hkResizeBuffers called" << std::endl;

	// Need to release mainRenderTargetView before calling ResizeBuffers
	if (g_pMainRenderTargetView != nullptr)
	{
		std::cout << "Releasing g_pMainRenderTargetView" << std::endl;
		g_pContext->OMSetRenderTargets(0, 0, 0);
		g_pMainRenderTargetView->Release();
	}

	std::cout << "Calling original ResizeBuffers" << std::endl;
	HRESULT hr = g_oResizeBuffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags); // Will return this at the end

	// Need to resetup the g_pMainRenderTargetView
	std::cout << "Resetting up g_pMainRenderTargetView" << std::endl;
	ID3D11Texture2D* pBuffer;
	pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBuffer);
	g_pDevice->CreateRenderTargetView(pBuffer, NULL, &g_pMainRenderTargetView);
	pBuffer->Release();

	std::cout << "Setting render target" << std::endl;
	g_pContext->OMSetRenderTargets(1, &g_pMainRenderTargetView, NULL);

	std::cout << "Setting up the viewport" << std::endl;
	// Set up the viewport. I have no idea what this actually does.
	D3D11_VIEWPORT viewport;
	viewport.Width = Width;
	viewport.Height = Height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	g_pContext->RSSetViewports(1, &viewport);

	g_screenHeight = Height;
	g_screenWidth = Width;

	return hr;
}



