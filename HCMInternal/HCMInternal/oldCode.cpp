#include "includes.h"
#include <string>
#include <iostream>
#include <vector>
#include <chrono>
#include <ctime>   
#include <d3dx11.h>
#include <sstream>
#include <iomanip>
#include <winbase.h>


// old includes.h
typedef HRESULT(__stdcall* Present) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
//typedef HRESULT(__stdcall* ResizeBuffers)(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
////typedef LRESULT(CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);
//


typedef HRESULT(__stdcall* ResizeBuffers)(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
//HRESULT hkResizeBuffers(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
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
bool overlayForcefullyDisabled = true;


int ScreenWidth = 10;
int ScreenHeight = 10;
bool DefaultFontInit;
ImFont* DefaultFont;


bool g_DisplayInfoFlag = false;



typedef struct
{
	DWORD R;
	DWORD G;
	DWORD B;
	DWORD A;
}RGBA;
RGBA red = { 255,0,0,255 };
RGBA green = { 0,255,0,255 };

typedef struct
{
	UINT64 DisplayInfoDetour;
	int ScreenX;
	int ScreenY;
	int SignificantDigits;
	float FontSize;

}DisplayInfoArguments;
DisplayInfoArguments g_DisplayInfoInfo = { 0, 0, 0, 0, 0 };



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


static void DrawDisplayInfo(RGBA* color, int outlineWidth, float outlineStrength)
{
	std::string currentMessageText = "Error getting player data: ";

		if (g_DisplayInfoInfo.DisplayInfoDetour == NULL)
		{
			currentMessageText = currentMessageText + "\nDisplayInfoDetour was null.";
			std::cout << currentMessageText;
		}
		else
		{
			float* InfoPtr = reinterpret_cast<float*>(g_DisplayInfoInfo.DisplayInfoDetour);

			//read info from the pointer
			float xPos = *InfoPtr;
			InfoPtr++;
			float yPos = *InfoPtr;
			InfoPtr++;
			float zPos = *InfoPtr;
			InfoPtr++;
			float xVel = *InfoPtr;
			InfoPtr++;
			float yVel = *InfoPtr;
			InfoPtr++;
			float zVel = *InfoPtr;
			InfoPtr++;
			float health = *InfoPtr;
			InfoPtr++;
			float shields = *InfoPtr;
			InfoPtr++;
			float viewX = *InfoPtr;
			InfoPtr++;
			float viewY = *InfoPtr;

			//derived values
			float hold = (xVel * xVel) + (yVel * yVel);
			float xyVel = sqrt(hold);
			float xyzVel = sqrt(hold + (zVel * zVel)); 

			int precision = g_DisplayInfoInfo.SignificantDigits;

			//start writing the display info string
			std::stringstream stream;
			stream << "Player Data: \n";
			stream << "Position: ";
			stream << std::fixed << std::setprecision(precision) << xPos << ", ";
			stream << std::fixed << std::setprecision(precision) << yPos << ", ";
			stream << std::fixed << std::setprecision(precision) << zPos << "\n";

			stream << "Velocity: ";
			stream << std::fixed << std::setprecision(precision) << xVel << ", ";
			stream << std::fixed << std::setprecision(precision) << yVel << ", ";
			stream << std::fixed << std::setprecision(precision) << zVel << "\n";
			stream << "Total XY Velocity: ";
			stream << std::fixed << std::setprecision(precision) << xyVel << "\n";
			stream << "Total XYZ Velocity: ";
			stream << std::fixed << std::setprecision(precision) << xyzVel << "\n";
			stream << "Health: ";
			stream << std::fixed << std::setprecision(precision) << health << ", ";
			stream << std::fixed << std::setprecision(precision) << shields << "\n";
			stream << "View Angle: ";
			stream << std::fixed << std::setprecision(precision) << viewX << ", ";
			stream << std::fixed << std::setprecision(precision) << viewY << "\n";

			currentMessageText = stream.str();
		}


		int x = g_DisplayInfoInfo.ScreenX;
		int y = g_DisplayInfoInfo.ScreenY;
		int fontsize = g_DisplayInfoInfo.FontSize;

		if (!DefaultFontInit)
		{
			DefaultFont = ImGui::GetIO().Fonts->Fonts[0];
			DefaultFontInit = true;
		}

				// Add to the draw list
		if (outlineWidth > 0 && outlineStrength >= 0 && outlineStrength <= 1)
		{
			ImGui::GetOverlayDrawList()->AddText(DefaultFont, fontsize, ImVec2(x, y - outlineWidth), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, outlineStrength )), currentMessageText.c_str());
			ImGui::GetOverlayDrawList()->AddText(DefaultFont, fontsize, ImVec2(x, y + outlineWidth), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, outlineStrength )), currentMessageText.c_str());
			ImGui::GetOverlayDrawList()->AddText(DefaultFont, fontsize, ImVec2(x - outlineWidth, y), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, outlineStrength )), currentMessageText.c_str());
			ImGui::GetOverlayDrawList()->AddText(DefaultFont, fontsize, ImVec2(x + outlineWidth, y), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, outlineStrength )), currentMessageText.c_str());
		}

		ImGui::GetOverlayDrawList()->AddText(DefaultFont, fontsize, ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, (color->A / 255.0))), currentMessageText.c_str());


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

bool g_hcmInitialised = false;
extern "C" __declspec(dllexport) HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{


	if (oPresent == NULL)
	{
		oPresent = (Present)kiero::getMethodsTable()[8];
	}


	if (!g_hcmInitialised)
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
			g_hcmInitialised = true;
			/*RECT gameScreenRct;
			GetWindowRect(window, &gameScreenRct);

			ScreenWidth = gameScreenRct.right - gameScreenRct.left;
			ScreenHeight = gameScreenRct.bottom - gameScreenRct.top;*/
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


	
	overlayForcefullyDisabled = false;
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	DrawPersistentMessages(10, 10, &green, textToPrint, 1, 0.5);
	DrawTemporaryMessages(10, 30, &green, 1, 0.5);
	if (g_DisplayInfoFlag) { DrawDisplayInfo(&green, 1, 0.5); }

	ImGui::EndFrame();
	ImGui::Render();

	pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());



	//crashing on calling original present. Why?
	//this function is failing? oPresent is null for Kidtrouble? But it's not null in the log.. weird
	//
	return oPresent(pSwapChain, SyncInterval, Flags);
}




extern "C" __declspec(dllexport) int IsTextDisplaying()
{
	return (g_hcmInitialised && !overlayForcefullyDisabled && textToPrint != "" && oPresent != NULL) ? 1 : 0;
}

extern "C" __declspec(dllexport) int ChangeDisplayText(const TCHAR * pChars)
{

	std::string inputText(pChars);
	textToPrint = inputText;

	return IsTextDisplaying();
}

extern "C" __declspec(dllexport) int IsOverlayHooked()
{
	if (!g_hcmInitialised || overlayForcefullyDisabled)
	{
		return 2;
	}
	else
	{
		return 1;
	}

}

extern "C" __declspec(dllexport) int EnableDisplayInfo(DisplayInfoArguments dii)
{
	g_DisplayInfoInfo.DisplayInfoDetour = dii.DisplayInfoDetour;
	g_DisplayInfoInfo.FontSize = dii.FontSize;
	g_DisplayInfoInfo.SignificantDigits = dii.SignificantDigits;
	g_DisplayInfoInfo.ScreenX = dii.ScreenX;
	g_DisplayInfoInfo.ScreenY = dii.ScreenY;
	g_DisplayInfoFlag = true;
		return 1;

}

extern "C" __declspec(dllexport) int DisableDisplayInfo()
{
	g_DisplayInfoFlag = false;
	return 1;

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

extern "C" __declspec(dllexport) HRESULT __stdcall  hkResizeBuffers(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
{

	//ImGui_ImplDX11_Shutdown();
	//ImGui_ImplWin32_Shutdown();
	
	std::cout << "\nhkResizeBuffers called";




	if (mainRenderTargetView)
	{
		std::cout << "\nReleasing mainRenderTargetView";
		pContext->OMSetRenderTargets(0, 0, 0);
		//ID3D11RenderTargetView* nullViews[] = { nullptr };
		//pContext->OMSetRenderTargets(1, nullViews, 0);
		mainRenderTargetView->Release();
		//window = NULL;
		//pContext->ClearState();
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



	ScreenWidth = Width;
	ScreenHeight = Height;

	return hr;
}

extern "C" __declspec(dllexport) int WINAPI IsHookEnabled()
{
	//oPresent only null if hook not enabled
	return (oPresent != NULL);
}

extern "C" __declspec(dllexport) int WINAPI EnableHook(UINT64* PtrToPresentPtr)
{
	//Get current value of the games PresentPtr
	UINT64* presentPtr = reinterpret_cast<UINT64*>(*PtrToPresentPtr);
	if (IsBadReadPtr(presentPtr, 8))
	{
		std::cout << "\nPtrToPresentPtr passed to EnableHook was invalid: " << PtrToPresentPtr;
		return 0;
	}
	std::cout << "\nPresentPtr is at location: " << *presentPtr;
	UINT64 currentPresentPtr = *presentPtr;
	std::cout << "\nValue of currentPresentPtr is " << currentPresentPtr;

	//As well as resizeBuffers (+0x28 from presentPtr, this offset will never change)
	UINT64* ResizePtr = reinterpret_cast<UINT64*>(*PtrToPresentPtr + 0x28);
	std::cout << "\nResizePtr is at location" << *ResizePtr;
	UINT64 currentResizePtr = *ResizePtr;
	std::cout << "\nValue of currentResizePtr is " << currentResizePtr;

	//Check if hook already applied (if the current value of the games PresentPtr is already hkPresent)
	if (currentPresentPtr == (UINT64)hkPresent) 
	{ 
		std::cout << "\nHook already appears to be enabled, bailing";
		return 1;  
	}

	//Store the current value as oPresent so hkPresent can passthrough to it (and so we can undo the hook later)
	oPresent = (Present)currentPresentPtr;
	std::cout << "\noPresent stored: " << (UINT64)oPresent;

	//same with ResizeBuffers, but only if it's not equal to hkResizeBuffers already
	if (currentResizePtr != (UINT64)hkResizeBuffers)
	{
		//store current value
		oResizeBuffers = (ResizeBuffers)currentResizePtr;

		// redirect currentResizePtr to hkResizeBuffers
		std::cout << "\nSetting ResizePtr to hkResizeBuffers, newval: " << (UINT64)hkResizeBuffers;
		UINT64 hkResizeBuffersPtr = (UINT64)hkResizeBuffers;
		DWORD dwNewProtect, dwOldProtect;
		VirtualProtect(ResizePtr, 8, PAGE_EXECUTE_READWRITE, &dwNewProtect);
		memcpy(ResizePtr, &hkResizeBuffersPtr, 8);
		VirtualProtect(ResizePtr, 8, dwNewProtect, &dwOldProtect);
	}
	else
	{
		std::cout << "\nResizeBuffers was already hooked, so skipping that";
	}

	//finally, redirect currentPresentPtr to hkPresent
	std::cout << "\nSetting presentPtr to hkPresent, newval: " << (UINT64)hkPresent;
	UINT64 hkPresentPtr = (UINT64)hkPresent;
	DWORD dwNewProtect, dwOldProtect;
	VirtualProtect(presentPtr, 8, PAGE_EXECUTE_READWRITE, &dwNewProtect);
	memcpy(presentPtr, &hkPresentPtr, 8);
	VirtualProtect(presentPtr, 8, dwNewProtect, &dwOldProtect);

	overlayForcefullyDisabled = false;

	return 1;

}


extern "C" __declspec(dllexport) void WINAPI RemoveHook(UINT64* PtrToPresentPtr)
{

	//if (!init) { return; }

	//if oPresent is null then hook was never applied in the first place
	if (oPresent == NULL) { return; }


	std::cout << "\nPtrToPresentPtr: " << *PtrToPresentPtr;

	//original value of Present which we stored when hook was enabled
	UINT64 ogPresent = (UINT64)oPresent;

	//Get the current value of the games presentPtr
	UINT64* presentPtr = reinterpret_cast<UINT64*>(*PtrToPresentPtr);
	UINT64 currentPresentPtr = *presentPtr;
	std::cout << "\ngames present pointer: " << presentPtr;
	std::cout << "\noriginal present pointer address: " << ogPresent;

	//Redirect the games presentPtr to again point to the original Present function
	DWORD dwNewProtect, dwOldProtect;
	VirtualProtect(presentPtr, 8, PAGE_EXECUTE_READWRITE, &dwNewProtect);
	memcpy(presentPtr, &ogPresent, 8);
	VirtualProtect(presentPtr, 8, dwNewProtect, &dwOldProtect);


	//Now we won't bother to remove the ResizeBuffersHook - it has essentially no effect, and if we remove it a crash will occur since there's still some unreleased resources that I haven't identified

	//Nullify the oPresent so it's ready for enabling hook again
	oPresent = NULL;

	textToPrint = "";
	overlayForcefullyDisabled = true;

}



//DWORD WINAPI MainThread(void* pHandle)
//{
//
//
//
//
//
//	if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
//	{
//		//oResizeBuffers = (ResizeBuffers)kiero::getMethodsTable()[13];
//		//void* hkResizeBuffersPtr = (void*)hkResizeBuffers;
//		//std::cout << "\nhkResizeBuffers " << hkResizeBuffersPtr;
//		//std::cout << "\noResizeBuffers " << oResizeBuffers;
//
//		//
//
//
//
//
//
//		//oPresent = (Present)kiero::getMethodsTable()[8];
//		//void* hkPresentPtr = (void*)hkPresent;
//
//		//std::cout << "\nhkPresentPtr" << hkPresentPtr;
//		//std::cout << "\noPresent" << oPresent;
//
//
//	}
//
//
//	kiero::shutdown();
//
//	
//
//	return TRUE;
//}
//
//
//
//BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved)
//{
//
//	FILE* pCout;
//
//
//	LPSTR processPath = new CHAR[256];
//	std::string path;
//	std::string processName;
//	switch (dwReason)
//	{
//	case DLL_PROCESS_ATTACH:
//		DisableThreadLibraryCalls(hMod);
//
//		// To get function pointers of the DLL in HCM, we need to load the DLL in both HCM and our target process (MCC)
//		// So we add a check here if the current process is MCC, and if not skip the mainthread stuff.
//
//		GetModuleFileName(NULL, processPath, 256);
//		path = processPath;
//		processName = path.substr(path.find_last_of("/\\") + 1);
//
//		//force processName to lowercase so we can search for substring without caring about case
//		for (auto& c : processName)
//		{
//			c = tolower(c);
//		}
//
//		if (processName.find("mcc") != std::string::npos)
//		{
//			//Old console debugging code
//			AllocConsole();
//			freopen_s(&pCout, "conout$", "w", stdout);
//
//			//New log to file code
//			//FILE* pCout;
//			//freopen_s(&pCout, "HCMInternalLog.txt", "w", stdout);
//
//
//			std::cout << "\nHCMInternal.dll injected.";
//			std::cout << "Size of DisplayInfoInfo: " << sizeof(DisplayInfoInfo);
//			textToPrint = "HCM";
//			std::time_t curr_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
//#pragma warning(disable : 4996)
//			std::cout << "Current Time: " << std::ctime(&curr_time);
//#pragma warning(enable : 4996)
//			CreateThread(nullptr, 0, MainThread, hMod, 0, nullptr);
//		}
//		
//		break;
//	case DLL_PROCESS_DETACH:
//		std::cout << "\nDLL_PROCESS_DETACH happened";
//		kiero::shutdown();
//		fclose(stdout);
//		//FreeConsole();
//		
//
//		break;
//	}
//	return TRUE;
//}


