#include "includes.h"
#include <string>
#include <iostream>
#include <vector>
#include <chrono>
#include <ctime>   
#include <d3dx11.h>





typedef HRESULT(__stdcall* ResizeBuffers)(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Present oPresent;

ResizeBuffers oResizeBuffers;
HWND window = NULL;
WNDPROC oWndProc;
ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
ID3D11RenderTargetView* mainRenderTargetView;
std::string textToPrint = "HaloCheckpointManager Hooked! \nCould put game info here \nlike player coordinates, health etc";
bool test = false;


typedef struct
{
	DWORD R;
	DWORD G;
	DWORD B;
	DWORD A;
}RGBA;
RGBA red = { 255,0,0,255 };
RGBA green = { 0,255,0,255 };




static void DrawPersistentMessages(int x, int y, RGBA* color, std::string text, int outlineWidth, float outlineStrength)
{
	//outlineWidth values that aren't 1 look terrible.. need to look into how to just blur text
	if (outlineWidth > 0 && outlineStrength >= 0 && outlineStrength <= 1)
	{
			ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y - outlineWidth), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, outlineStrength)), text.c_str());
			ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y + outlineWidth), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, outlineStrength)), text.c_str());
			ImGui::GetOverlayDrawList()->AddText(ImVec2(x - outlineWidth, y), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, outlineStrength)), text.c_str());
			ImGui::GetOverlayDrawList()->AddText(ImVec2(x + outlineWidth, y), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, outlineStrength)), text.c_str());
	}
	
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), text.c_str());
}


class TemporaryMessage
{
public:
	std::string messageText;
	std::chrono::steady_clock::time_point messageTimestamp;
};

std::vector<TemporaryMessage> temporaryMessages;
double messageExpiryTime_ms = 3000;
static void DrawTemporaryMessages(int x, int y, RGBA* color, int outlineWidth, float outlineStrength)
{

	//iterate over temporaryMessages map
	std::vector<TemporaryMessage>::iterator it;
	auto curr_time = std::chrono::high_resolution_clock::now();

	//create a copy of temporaryMessages map as we will want to make changes to it

	int numberOfValidMessages = 0;
	int verticalSeperation = 20;
	for (it = temporaryMessages.begin(); it != temporaryMessages.end();)
	{
		std::string currentMessageText = it->messageText;
		std::chrono::steady_clock::time_point currentMessageTime = it->messageTimestamp;

		//calculate how long it's been (in milliseconds
		double difference = std::chrono::duration<double, std::milli>(curr_time - currentMessageTime).count();

		// if larger than messageExpiryTime, delete from temporaryMessages & continue loop
		if (difference > messageExpiryTime_ms)
		{
			std::cout << "\nErasing old message.";
				it = temporaryMessages.erase(it);
			continue;
		}

		

		// calculate fade value of text
		float opacityValue;
		double half_messageExpiryTime_ms = (messageExpiryTime_ms / 2);
		if (difference < half_messageExpiryTime_ms)
		{
			opacityValue = 1;
		}
		else
		{
			opacityValue = 1 - ((difference - half_messageExpiryTime_ms) / half_messageExpiryTime_ms);
		}

//		std::cout << "\nThisMessage opacity: " << opacityValue;

		// Add to the draw list
		int placeY = y + (numberOfValidMessages * verticalSeperation);
		if (outlineWidth > 0 && outlineStrength >= 0 && outlineStrength <= 1)
		{
			ImGui::GetOverlayDrawList()->AddText(ImVec2(x, placeY - outlineWidth), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, outlineStrength * opacityValue)), currentMessageText.c_str());
			ImGui::GetOverlayDrawList()->AddText(ImVec2(x, placeY + outlineWidth), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, outlineStrength * opacityValue)), currentMessageText.c_str());
			ImGui::GetOverlayDrawList()->AddText(ImVec2(x - outlineWidth, placeY), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, outlineStrength * opacityValue)), currentMessageText.c_str());
			ImGui::GetOverlayDrawList()->AddText(ImVec2(x + outlineWidth, placeY), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, outlineStrength * opacityValue)), currentMessageText.c_str());
		}

		ImGui::GetOverlayDrawList()->AddText(ImVec2(x, placeY), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, (color->A / 255.0) * opacityValue)), currentMessageText.c_str());
		numberOfValidMessages++;
		it++;
	}


}




void InitImGui()
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(pDevice, pContext);
}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

bool init = false;
extern "C" __declspec(dllexport) HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	

	if (!init)
	{
		std::cout << "\nAttempting to init d3d device - Love, hkPresent.";
		if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)& pDevice)))
		{
			std::cout << "\nSuccesfully got the id of the d3d11 device.";
			pDevice->GetImmediateContext(&pContext);
			DXGI_SWAP_CHAIN_DESC sd;
			pSwapChain->GetDesc(&sd);
			window = sd.OutputWindow;
			ID3D11Texture2D* pBackBuffer;
			pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)& pBackBuffer);
			pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
			pBackBuffer->Release();
			oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
			InitImGui();
			std::cout << "\nSuccesfully initialised d3d hook.";
			init = true;
		}

		else
			return oPresent(pSwapChain, SyncInterval, Flags);
	}





	

	if (mainRenderTargetView == nullptr) {
		std::cout << "\nmainRenderTargetView was null! Attempting to create a new one.";
		ID3D11Texture2D* pBackBuffer;
		pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
		pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
		
		pBackBuffer->Release();
	}


	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	DrawPersistentMessages(10, 10, &green, textToPrint, 1, 0.5);
	DrawTemporaryMessages(10, 30, &green, 1, 0.5);


	ImGui::EndFrame();
	ImGui::Render();

	pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());




	return oPresent(pSwapChain, SyncInterval, Flags);
}

extern "C" __declspec(dllexport) void ChangeDisplayText(const TCHAR * pChars)
{

	std::string inputText(pChars);
	textToPrint = inputText;
}


extern "C" __declspec(dllexport) int IsTextDisplaying()
{
	if (!init)
	{
		return 2;
	}
	else
	{
		return 1;
	}

}


extern "C" __declspec(dllexport) int PrintTemporaryMessage(const TCHAR * pChars)
{
	std::string inputText(pChars);
	auto curr_time = std::chrono::high_resolution_clock::now();

	TemporaryMessage tmessage;
	tmessage.messageText = inputText;
	tmessage.messageTimestamp = curr_time;
	//messages go to the front
	temporaryMessages.insert(temporaryMessages.begin(), tmessage);

	std::cout << "\nAdded to tempMessages map, count: " << temporaryMessages.size();

	return 1;

}

HRESULT hkResizeBuffers(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
{
/*
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();*/
	
	std::cout << "\nhkResizeBuffers called";

	if (mainRenderTargetView) {
		std::cout << "\nReleasing mainRenderTargetView";
		pContext->OMSetRenderTargets(0, 0, 0);
		mainRenderTargetView->Release();
		//mainRenderTargetView = nullptr;
	}
	std::cout << "\ngetting original ResizeBuffers";
	HRESULT hr = oResizeBuffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);

	std::cout << "\n";
	std::cout << "\ngetting the new d3d device";
	ID3D11Texture2D* pBuffer;
	pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBuffer);
	// Perform error handling here!

	pDevice->CreateRenderTargetView(pBuffer, NULL, &mainRenderTargetView);
	// Perform error handling here!
	pBuffer->Release();

	std::cout << "\nSetting render target ?";
	pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);

	std::cout << "\nSetting up the viewport?";
	// Set up the viewport.
	D3D11_VIEWPORT vp;
	vp.Width = Width;
	vp.Height = Height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	pContext->RSSetViewports(1, &vp);

	return hr;
}


extern "C" __declspec(dllexport) void WINAPI RemoveHook(UINT64* PtrToPresentPtr)
{

	if (!init) { return; }

	std::cout << "\n???: " << *PtrToPresentPtr;

	UINT64 og = (UINT64) oPresent;

	UINT64* presentPtr = reinterpret_cast<UINT64*>(*PtrToPresentPtr);

	std::cout << "\ngames present pointer: " << presentPtr;
	std::cout << "\noriginal present pointer address: " << og;
	DWORD dwNewProtect, dwOldProtect;
	VirtualProtect(presentPtr, 8, PAGE_EXECUTE_READWRITE, &dwNewProtect);
	memcpy(presentPtr, &og, 8);
	VirtualProtect(presentPtr, 8, dwNewProtect, &dwOldProtect);
	//*presentPtr = og;


}

DWORD WINAPI MainThread(void* pHandle)
{





	if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
	{
		

		oPresent = (Present)kiero::getMethodsTable()[8];
		void* hkPresentPtr = (void*)hkPresent;

		std::cout << "\nhkPresentPtr" << hkPresentPtr;
		std::cout << "\noPresent" << oPresent;

		
		kiero::bind(13, (void**)&oResizeBuffers, hkResizeBuffers);


		while (!GetAsyncKeyState(VK_END));
	}


	kiero::shutdown();
	//FreeLibrary((HMODULE)pHandle);
	

	return TRUE;
}



BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved)
{

	//stdout to console (if console exists)



	LPSTR processPath = new CHAR[256];
	std::string path;
	std::string processName;
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hMod);

		// To get function pointers of the DLL in HCM, we need to load the DLL in both HCM and our target process (MCC)
		// So we add a check here if the current process is MCC, and if not skip the mainthread stuff.

		GetModuleFileName(NULL, processPath, 256);
		path = processPath;
		processName = path.substr(path.find_last_of("/\\") + 1);
		if (processName.find("MCC") != std::string::npos)
		{
			//Setup console for debugging (will remove in release)
			AllocConsole();
			FILE* pCout;
			freopen_s(&pCout, "conout$", "w", stdout);
			std::cout << "\nHCMInternal.dll injected.";
			


			textToPrint = "hi";
			CreateThread(nullptr, 0, MainThread, hMod, 0, nullptr);
		}
		
		break;
	case DLL_PROCESS_DETACH:
		kiero::shutdown();
		
		FreeConsole();
		
		break;
	}
	return TRUE;
}


