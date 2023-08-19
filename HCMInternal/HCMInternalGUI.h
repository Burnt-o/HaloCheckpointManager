#pragma once
#include "ImGuiManager.h"
#include "HaloEnums.h"
#include "GUIElementBase.h"
// Singleton: on construction we subscribe to the ImGuiRenderEvent.
// In onImGuiRenderEvent we draw our options GUI.
// destructor just unsubscribes from ImGuiRenderEvent.

class HCMInternalGUI
{
private:
	static HCMInternalGUI* instance; // Private Singleton instance so static hooks/callbacks can access
	std::mutex mDestructionGuard; // Protects against Singleton destruction while callbacks are executing

	// ImGuiManager Event reference and our handle to the append so we can remove it in destructor
	eventpp::CallbackList<void()>& pImGuiRenderEvent;
	eventpp::CallbackList<void()>::Handle mImGuiRenderCallbackHandle = {};

	// resize Window event and handle
	eventpp::CallbackList<void(ImVec2)>& pWindowResizeEvent;
	eventpp::CallbackList<void(ImVec2)>::Handle mWindowResizeCallbackHandle = {};

	// GameStateChanged event and handle
	eventpp::CallbackList<void(GameState, std::string)>& pGameStateChangeEvent;
	eventpp::CallbackList<void(GameState, std::string)>::Handle mGameStateChangeCallbackHandle = {};

	// What we run when ImGuiManager ImGuiRenderEvent is invoked
	static void onImGuiRenderEvent();
	static void onWindowResizeEvent(ImVec2 newScreenSize);
	static void onGameStateChange(GameState, std::string);

	// initialize resources in the first onImGuiRenderEvent
	void initializeHCMInternalGUI();
	bool m_HCMInternalGUIinitialized = false;


	void primaryRender(); // Primary render function

	// sub-render functions to help my sanity
	std::vector<HCMExceptionBase> errorsToDisplay;
	void renderErrorDialog();

	// GUI data
	ImGuiWindowFlags windowFlags;
	static bool m_WindowOpen;
	ImVec2 mWindowSize{ 500, 500 };
	ImVec2 mWindowPos{ 10, 25 };


	static std::set<std::shared_ptr<GUIElementBase>> currentGameGUIElements;



public:

	// Gets passed ImGuiManager ImGuiRenderEvent reference so we can subscribe and unsubscribe
	explicit HCMInternalGUI(eventpp::CallbackList<void()>& pRenderEvent, eventpp::CallbackList<void(ImVec2)>& pResizeEvent, eventpp::CallbackList<void(GameState, std::string)>& gamestatechanged) : pImGuiRenderEvent(pRenderEvent), pWindowResizeEvent(pResizeEvent), pGameStateChangeEvent(gamestatechanged)
	{
		if (instance != nullptr)
		{
			throw HCMInitException("Cannot have more than one HCMInternalGUI");
		}
		instance = this;

		mImGuiRenderCallbackHandle = pImGuiRenderEvent.append(onImGuiRenderEvent);
		mWindowResizeCallbackHandle = pWindowResizeEvent.append(onWindowResizeEvent);
		mGameStateChangeCallbackHandle = pGameStateChangeEvent.append(onGameStateChange);
	}

	~HCMInternalGUI(); // release resources

	static void addPopupError(HCMExceptionBase error) { if (instance->errorsToDisplay.size() < 5) instance->errorsToDisplay.push_back(error); }
	static bool isWindowOpen() { return m_WindowOpen; };
	static float getHCMInternalWindowHeight() { 
		if (!instance)
		{
			PLOG_ERROR << "getHCMInternalWindowHeight when HCMInternalGUI not initiailized";
			return 0;
		}
		return instance->mWindowSize.y; };
};

// Todo: properly split the concepts of "desired window height" and "allocated window height". maybe call the former one "content height"