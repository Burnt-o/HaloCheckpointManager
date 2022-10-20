#include "includes.h"
#include <string>
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Present oPresent;
HWND window = NULL;
WNDPROC oWndProc;
ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
ID3D11RenderTargetView* mainRenderTargetView;
std::string textToPrint = "HaloCheckpointManager Hooked! \nCould put game info here \nlike player coordinates, health etc";


typedef struct
{
	DWORD R;
	DWORD G;
	DWORD B;
	DWORD A;
}RGBA;
RGBA red = { 255,0,0,255 };
RGBA green = { 0,255,0,255 };

static void MyDrawOutlinedText(int x, int y, RGBA* color, std::string text, int outlineWidth, float outlineStrength)
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


extern "C" __declspec(dllexport) void ChangeDisplayText(const TCHAR* pChars)
{

	std::string inputText(pChars);
	textToPrint = inputText;
}

extern "C" __declspec(dllexport) int IsTextDisplaying()
{
	if (textToPrint == "hi")
	{
		return 2;
	}
	else
	{
		return 1;
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
HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	if (!init)
	{
		if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)& pDevice)))
		{
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
			init = true;
		}

		else
			return oPresent(pSwapChain, SyncInterval, Flags);
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	MyDrawOutlinedText(10, 10, &green, textToPrint, 1, 0.5);

	//ImGui::GetOverlayDrawList()->AddText(textPosition, textColor, textContent.c_str());

	/*ImGui::Text("HELLO BURNT, HERE'S A FUNNY NUMBER: %d", 69);
	ImGui::Begin("ImGui Window");
	ImGui::End();*/

	ImGui::Render();

	pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	return oPresent(pSwapChain, SyncInterval, Flags);
}




DWORD WINAPI MainThread(void* pHandle)
{

	if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
	{
		kiero::bind(8, (void**)&oPresent, hkPresent);

		while (!GetAsyncKeyState(VK_END));
	}


	kiero::shutdown();
	//FreeLibrary((HMODULE)pHandle);
	

	return TRUE;
}



BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved)
{

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
			textToPrint = "hi";
			CreateThread(nullptr, 0, MainThread, hMod, 0, nullptr);
		}
		
		break;
	case DLL_PROCESS_DETACH:
		kiero::shutdown();
		break;
	}
	return TRUE;
}


