#pragma once
#include "IGUIElement.h"
#include "SettingsStateAndEvents.h"

class GUIInputString : public IGUIElement
{

private:
	std::string mLabelText;
	std::string mHotkeyButtonLabelId = std::format("..###{}", getName());
	std::weak_ptr<Setting<std::string>> mCommandStringWeak;
	std::optional<std::shared_ptr<ActionEvent>> mEventToFire;
	std::vector<std::thread> mFireEventThreads;

public:

	GUIInputString(GameState implGame, std::string labelText, std::weak_ptr<Setting<std::string>> commandString, std::optional<std::shared_ptr<ActionEvent>> eventToFire = std::nullopt)
		: IGUIElement(implGame, std::nullopt), mCommandStringWeak(commandString), mLabelText(labelText), mEventToFire(eventToFire)
	{
		if (mLabelText.empty()) throw HCMInitException("Cannot have empty label (needs label for imgui ID system, use ## for invisible labels)");
		PLOG_VERBOSE << "Constructing GUIInputString, name: " << getName();
		this->currentHeight = GUIFrameHeightWithSpacing;
	}

	void render(HotkeyRenderer& hotkeyRenderer) override
	{


		auto mCommandString = mCommandStringWeak.lock();
		if (!mCommandString)
		{
			PLOG_ERROR << "bad mSettings weakptr when rendering " << getName();
			return;
		}


		//ImGui::Text(mLabelText.c_str());
		//ImGui::SameLine();

		ImGui::SetNextItemWidth(100);
		if (ImGui::InputText(mLabelText.c_str(), &mCommandString->GetValueDisplay()))
		{
			mCommandString->UpdateValueWithInput();
			PLOG_VERBOSE << "GUIInputString firing event, command: " << mCommandString->GetValue();
		}
		DEBUG_GUI_HEIGHT;
		if (mEventToFire.has_value())
		{
			ImGui::SameLine();
			if (ImGui::Button(std::format("Send##{}", mLabelText).c_str()))
			{
				mCommandString->UpdateValueWithInput();
				PLOG_VERBOSE << "GUIInputString firing send event, command: " << mCommandString->GetValue();
				auto& newThread = mFireEventThreads.emplace_back(std::thread([mEvent = mEventToFire.value()]() {mEvent->operator()(); }));
				newThread.detach();
			}
		}


	}
	~GUIInputString()
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