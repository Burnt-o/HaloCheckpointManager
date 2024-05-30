#pragma once
#include "IGUIElement.h"
#include "SettingsStateAndEvents.h"

template<int lineCount, float lineWidth>
class GUIInputMultilineString : public IGUIElement
{

private:
	std::string mLabelText;
	std::string mHotkeyButtonLabelId = std::format("..###{}", getName());
	std::weak_ptr<BinarySetting<std::string>> mCommandStringWeak;
	std::optional<std::shared_ptr<ActionEvent>> mEventToFire;
	std::vector<std::thread> mFireEventThreads;

public:

	GUIInputMultilineString(GameState implGame, ToolTipCollection tooltip, std::string labelText, std::weak_ptr<BinarySetting<std::string>> commandString, std::optional<std::shared_ptr<ActionEvent>> eventToFire = std::nullopt)
		: IGUIElement(implGame, std::nullopt, tooltip), mCommandStringWeak(commandString), mLabelText(labelText), mEventToFire(eventToFire)
	{
		if (mLabelText.empty()) throw HCMInitException("Cannot have empty label (needs label for imgui ID system, use ## for invisible labels)");
		PLOG_VERBOSE << "Constructing GUIInputMultilineString, name: " << getName();
		this->currentHeight = GUIFrameHeightWithSpacing + // for label
			(lineCount * GUIFrameHeight) + GUISpacing + // for multiline
			eventToFire.has_value() ? GUIFrameHeightWithSpacing : 0; // for update button
	}

	void render(HotkeyRenderer& hotkeyRenderer) override
	{


		auto mCommandString = mCommandStringWeak.lock();
		if (!mCommandString)
		{
			PLOG_ERROR << "bad mSettings weakptr when rendering " << getName();
			return;
		}

		//TODO: use begin group here to align width betwene text and text input and button
		ImGui::Text(mLabelText.c_str());
		

		if (ImGui::InputTextMultiline(std::format("##{}Input", mLabelText).c_str(), &mCommandString->GetValueDisplay(), ImVec2(lineWidth, lineCount * ImGui::GetTextLineHeight())))
		{
			mCommandString->UpdateValueWithInput();
			PLOG_VERBOSE << "GUIInputMultilineString firing event, command: " << mCommandString->GetValue();
		}
		renderTooltip();
		DEBUG_GUI_HEIGHT;
		if (mEventToFire.has_value())
		{
			if (ImGui::Button(std::format("Update##{}", mLabelText).c_str()))
			{
				mCommandString->UpdateValueWithInput();
				PLOG_VERBOSE << "GUIInputMultilineString firing send event, command: " << mCommandString->GetValue();
				auto& newThread = mFireEventThreads.emplace_back(std::thread([mEvent = mEventToFire.value()]() {mEvent->operator()(); }));
				newThread.detach();
			}
			renderTooltip();
		}


	}
	~GUIInputMultilineString()
	{
		for (auto& thread : mFireEventThreads)
		{
			if (thread.joinable())
			{
				PLOG_DEBUG << getName() << "joining mFireEventThread";
				thread.join();
				PLOG_DEBUG << getName() << "FireEventThread finished";
			}

		}
	}


	std::string_view getName() override { return mLabelText; }

};