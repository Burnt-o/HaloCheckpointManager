#pragma once
#include "pch.h"
#include "IAnchorPoint.h"
#include "IMessagesGUI.h"
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
	void onImGuiRenderEvent(Vec2 screenSize);

	// data
	std::vector <temporaryMessage> mMessages;
	std::shared_ptr<IAnchorPoint> mAnchorPoint;
	Vec2 mAnchorOffset;
	// funcs
	void iterateMessages();
	void drawMessage(const temporaryMessage& message, const Vec2& position);
public:

	virtual void addMessage(std::string message) override;

	explicit MessagesGUI( Vec2 anchorOffset, std::shared_ptr<RenderEvent> renderEvent)
		:  mAnchorOffset(anchorOffset), mRenderEventCallback(renderEvent, [this](Vec2 a) {onImGuiRenderEvent(a); })
	{
	}

	~MessagesGUI()
	{
		std::unique_lock<std::mutex> lock(mDestructionGuard);
	}

	//explicit MessagesGUI(Vec2 anchorOffset)
	//	: mAnchorOffset(anchorOffset)
	//{
	//}

	//void setRenderEvent(std::shared_ptr<RenderEvent> renderEvent)
	//{
	//	auto sharedThis = shared_from_this();
	//	mRenderEventCallback.make( renderEvent, [this, sharedThis](Vec2 a) {onImGuiRenderEvent(a); } );
	//	
	//}

	virtual void setAnchorPoint(std::shared_ptr<IAnchorPoint> anchorPoint) override { mAnchorPoint = anchorPoint; }
};

