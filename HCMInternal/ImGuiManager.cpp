#include "pch.h"
#include "ImGuiManager.h"
#include "GlobalKill.h"

ImGuiManager* ImGuiManager::instance = nullptr;

WNDPROC ImGuiManager::mOldWndProc = nullptr;
IMGUI_IMPL_API LRESULT  ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT __stdcall ImGuiManager::mNewWndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//https://www.unknowncheats.me/forum/2488829-post5.html
	ImGuiIO& io = ImGui::GetIO();
	LRESULT res = ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);

	if (io.WantCaptureMouse == false)
	{
		// ImGui didn't handle the click so let MCC do it
		return CallWindowProc(ImGuiManager::mOldWndProc, hWnd, uMsg, wParam, lParam);
	}
	else
	{
		switch (uMsg)
		{
			// Certain messages we want to let MCC know about too
		case WM_ACTIVATE:
		case WM_ACTIVATEAPP:
		case WM_NCACTIVATE:
			return CallWindowProc(ImGuiManager::mOldWndProc, hWnd, uMsg, wParam, lParam);
			break;
		default:
			return true; // otherwise we just tell MCC to not worry about it
		}
	}





}

void ImGuiManager::initializeImGuiResources(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, IDXGISwapChain* pSwapChain, ID3D11RenderTargetView* pMainRenderTargetView)
{
	PLOG_VERBOSE << "initializeImGuiResources";
	// Use swap chain description to get MCC window handle
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	pSwapChain->GetDesc(&swapChainDesc);
	if (!&swapChainDesc) throw HCMInitException("Failed to get swap chain description");
	m_windowHandle = swapChainDesc.OutputWindow;

	// Setup the imgui WndProc callback
	mOldWndProc = (WNDPROC)SetWindowLongPtrW(m_windowHandle, GWLP_WNDPROC, (LONG_PTR)&mNewWndProc);


	// Setup ImGui stuff
	PLOG_DEBUG << "Initializing ImGui";
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;

	if (!ImGui_ImplWin32_Init(m_windowHandle))
	{
		throw HCMInitException(std::format("ImGui_ImplWin32_Init failed w/ {} ", (uint64_t)m_windowHandle).c_str());
	};
	if (!ImGui_ImplDX11_Init(pDevice, pDeviceContext))
	{
		throw HCMInitException(std::format("ImGui_ImplDX11_Init failed w/ {}, {} ", (uint64_t)pDevice, (uint64_t)pDeviceContext).c_str());
	};

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	ImGuiStyle* style = &ImGui::GetStyle();
	style->FrameRounding = 0;
	style->WindowBorderSize = 0;
	style->WindowRounding = 0;
	style->FrameBorderSize = 1;

	// Colors
	style->Colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.04f, 0.08f, 1.00f);
	style->Colors[ImGuiCol_ChildBg] = ImVec4(0.12f, 0.06f, 0.12f, 0.50f);
	style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.90f, 0.90f, 1.00f);
	style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.29f, 0.29f, 1.00f);

	style->Colors[ImGuiCol_Border] = ImVec4(0.40f, 0.50f, 0.50f, 0.38f);
	style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);

	style->Colors[ImGuiCol_FrameBg] = ImVec4(0.35f, 0.09f, 0.12f, 1.00f);
	//style->Colors[ImGuiCol_FrameBg] = ImVec4(0.95f, 0.99f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.55f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.81f, 0.23f, 0.29f, 1.00f);

	style->Colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 0.9f, 0.6f, 0.7f);
	style->Colors[ImGuiCol_Button] = ImVec4(0.30f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.50f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.75f, 0.23f, 0.29f, 1.00f);

	style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.75f, 0.43f);

	style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.90f, 0.70f, 0.73f, 0.31f);
	style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.05f, 0.07f, 1.00f);

	style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.f, 0.f, 0.f, 0.f);
	style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.35f, 0.09f, 0.12f, 0.7f);

	style->Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.1f, 0.00f, 0.1f, 0.3f);

	style->IndentSpacing = 10.f;




}


ImGuiManager::~ImGuiManager()
{
	PLOG_DEBUG << "~ImGuiManager()";
	std::unique_lock<std::mutex> lock(mDestructionGuard);
		// restore the original wndProc
	if (mOldWndProc)
	{
		SetWindowLongPtrW(m_windowHandle, GWLP_WNDPROC, (LONG_PTR)mOldWndProc);
		mOldWndProc = nullptr;
	}

	// Cleanup
	if (m_isImguiInitialized)
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		m_isImguiInitialized = false;
	}

	instance = nullptr;
}

void ImGuiManager::onPresentHookEvent(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, IDXGISwapChain* pSwapChain, ID3D11RenderTargetView* pMainRenderTargetView)
{
	LOG_ONCE(PLOG_DEBUG << "ImGuiManager::onPresentHookEvent running");
	std::unique_lock<std::mutex> lock(mDestructionGuard);
#pragma region init


	if (!instance->m_isImguiInitialized)
	{
		PLOG_INFO << "Initializing ImGuiManager";
		try
		{
			instance->initializeImGuiResources(pDevice, pDeviceContext, pSwapChain, pMainRenderTargetView);
			instance->m_isImguiInitialized = true;
		}
		catch (HCMInitException& ex)
		{
			PLOG_FATAL << "Failed to initialize ImGui, info: " << std::endl
				<< ex.what() << std::endl
				<< "HCM will now automatically close down";
			GlobalKill::killMe();
			return;
		}
	}
#pragma endregion init

	//TODO: check if this is necessary
	MSG msg;
	while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);

	}


	// Start ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// I don't 100% understand what this does, but it must be done before we try to render
	pDeviceContext->OMSetRenderTargets(1, &pMainRenderTargetView, NULL);
	auto screenSize = getScreenSize();
	// invoke callback of anything that wants to render with ImGui
	auto x = *BackgroundRenderEvent.get();
	x(screenSize);
	BackgroundRenderEvent->operator()(screenSize);
	MidgroundRenderEvent->operator()(screenSize);
	ForegroundRenderEvent->operator()(screenSize);

	// Finish ImGui frame
	ImGui::EndFrame();
	ImGui::Render();

	// Render it !
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}


