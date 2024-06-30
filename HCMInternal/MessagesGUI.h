#pragma once
#include "pch.h"
#include "IAnchorPoint.h"
#include "IMessagesGUI.h"
#include "SettingsStateAndEvents.h"
// Displays temporary messages to the user, below the main GUI. Messages fade over time then disappear.

struct temporaryMessage
{
	std::string message;
	std::chrono::steady_clock::time_point timeStamp;
	long long lineCount;
	float messageAge;
};


class MessagesGUI : public std::enable_shared_from_this<MessagesGUI>, public IMessagesGUI
{
private:
	std::mutex mDestructionGuard{};
	// render event
	ScopedCallback<RenderEvent> mRenderEventCallback;

	// What we run when ImGuiManager ImGuiRenderEvent is invoked
	void onImGuiRenderEvent(SimpleMath::Vector2 screenSize);

	// data
	std::vector <temporaryMessage> mMessages;
	std::optional<std::weak_ptr<IAnchorPoint>> mAnchorPoint = std::nullopt;
	std::optional < std::weak_ptr<SettingsStateAndEvents>> mSettingsWeak;
	SimpleMath::Vector2 mAnchorOffset;
	static inline std::mutex addMessageMutex{};

	std::optional<ScopedCallback<eventpp::CallbackList<void(float&)>>> fontSizeChangedCallback;
	std::optional<ScopedCallback<eventpp::CallbackList<void(SimpleMath::Vector4&)>>> fontColorChangedCallback;
	float fontSize = 1.f;
	SimpleMath::Vector4 fontColor{1.f, 0.2f, 0.0f, 1.f};
	// funcs
	void iterateMessages();
	void drawMessage(const temporaryMessage& message, const SimpleMath::Vector2& position);

protected:
	virtual void addMessage(std::string message) override;

public:



	explicit MessagesGUI(SimpleMath::Vector2 anchorOffset, std::shared_ptr<RenderEvent> renderEvent)
		:  mAnchorOffset(anchorOffset), mRenderEventCallback(renderEvent, [this](SimpleMath::Vector2 a) {onImGuiRenderEvent(a); })
	{
	}

	~MessagesGUI()
	{
		mRenderEventCallback.removeCallback(); // no new callback invocation
		std::unique_lock<std::mutex> lock(mDestructionGuard); // block until callbacks finish executing
	}

	void setSettings(std::shared_ptr<SettingsStateAndEvents> settings) {
		mSettingsWeak = settings; 
		fontSizeChangedCallback = ScopedCallback<eventpp::CallbackList<void(float&)>>(settings->messagesFontSize->valueChangedEvent, [this](auto& n) { fontSize = n; });
		fontColorChangedCallback = ScopedCallback<eventpp::CallbackList<void(SimpleMath::Vector4&)>>(settings->messagesFontColor->valueChangedEvent, [this](auto& n) { fontColor = n; });

		fontSize = settings->messagesFontSize->GetValue();
		fontColor = settings->messagesFontColor->GetValue();
	}



	virtual void setAnchorPoint(std::weak_ptr<IAnchorPoint> anchorPoint) override { mAnchorPoint = anchorPoint; }
};

