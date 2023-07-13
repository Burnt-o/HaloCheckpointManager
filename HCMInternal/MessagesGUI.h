#pragma once
#include "ImGuiManager.h"

struct temporaryMessage
{
	std::string message;
	std::chrono::steady_clock::time_point timeStamp;
	long long lineCount;
	float messageAge;
};

class MessagesGUI
{
private:
	static MessagesGUI* instance; // Private Singleton instance so static hooks/callbacks can access
	std::mutex mDestructionGuard; // Protects against Singleton destruction while callbacks are executing

	// ImGuiManager Event reference and our handle to the append so we can remove it in destructor
	eventpp::CallbackList<void()>& pImGuiRenderEvent;
	eventpp::CallbackList<void()>::Handle mCallbackHandle = {};

	// What we run when ImGuiManager ImGuiRenderEvent is invoked
	static void onImGuiRenderEvent();

	// data
	std::vector <temporaryMessage> messages;

	// funcs
	void iterateMessages();
	void drawMessage(const temporaryMessage& message, const ImVec2& position);
public:

	static void addMessage(std::string message);

	explicit MessagesGUI(eventpp::CallbackList<void()>& pEvent) : pImGuiRenderEvent(pEvent)
	{
		if (instance != nullptr)
		{
			throw HCMInitException("Cannot have more than one MessagesGUI");
		}
		instance = this;
		mCallbackHandle = pImGuiRenderEvent.append(onImGuiRenderEvent);
	}
	~MessagesGUI(); // release resources
};

