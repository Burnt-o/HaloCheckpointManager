//#pragma once
//#include "IGUIElement.h"
//
//#include "SettingsStateAndEvents.h"
//class GUIConsoleCommand : public IGUIElement {
//
//private:
//	std::string mLabelText;
//	std::shared_ptr<ActionEvent> mEventToFire;
//	std::string mHotkeyButtonLabelId = std::format("..###{}", getName());
//	std::weak_ptr<BinarySetting<std::string>> mCommandStringWeak;
//	std::vector<std::thread> mFireEventThreads;
//
//public:
//
//	GUIConsoleCommand(GameState implGame, std::string labelText, std::weak_ptr<BinarySetting<std::string>> commandString, std::shared_ptr<ActionEvent> eventToFire)
//		: IGUIElement(implGame, std::nullopt), mCommandStringWeak(commandString), mEventToFire(eventToFire), mLabelText(labelText)
//	{
//		if (mLabelText.empty()) throw HCMInitException("Cannot have empty label (needs label for imgui ID system, use ## for invisible labels)");
//		PLOG_VERBOSE << "Constructing GUIConsoleCommand, name: " << getName();
//		PLOG_DEBUG << "&mEventToFire: " << std::hex << &mEventToFire;
//		this->currentHeight = GUIFrameHeightWithSpacing;
//	}
//
//	void render(HotkeyRenderer& hotkeyRenderer) override
//	{
//		
//
//		auto mCommandString = mCommandStringWeak.lock();
//		if (!mCommandString)
//		{
//			PLOG_ERROR << "bad mSettings weakptr when rendering " << getName();
//			return;
//		}
//
//
//		ImGui::Text(mLabelText.c_str());
//		ImGui::SameLine();
//
//		ImGui::SetNextItemWidth(100);
//		if (ImGui::InputText(std::format("##{}", mLabelText).c_str(), &mCommandString->GetValueDisplay(), ImGuiInputTextFlags_EnterReturnsTrue))
//		{
//			mCommandString->UpdateValueWithInput();
//			PLOG_VERBOSE << "GUIConsoleCommand firing event, command: " << mCommandString->GetValue();
//
//			auto& newThread = mFireEventThreads.emplace_back(std::thread([mEvent = mEventToFire]() {mEvent->operator()(); }));
//			newThread.detach();
//		}
//		DEBUG_GUI_HEIGHT;
//		ImGui::SameLine();
//		if (ImGui::Button(std::format("Send##{}", mLabelText).c_str()))
//		{
//			mCommandString->UpdateValueWithInput();
//			PLOG_VERBOSE << "GUIConsoleCommand firing event, command: " << mCommandString->GetValue();
//			auto& newThread = mFireEventThreads.emplace_back(std::thread([mEvent = mEventToFire]() {mEvent->operator()(); }));
//			newThread.detach();
//		}
//
//
//	}
//	~GUIConsoleCommand()
//	{
//		for (auto& thread : mFireEventThreads)
//		{
//			if (thread.joinable())
//			{
//				PLOG_DEBUG << getName() << "joining mFireEventThread";
//				thread.join();
//				PLOG_DEBUG << getName() << "FireEventThread finished";
//			}
//
//		}
//	}
//
//
//	std::string_view getName() override { return mLabelText; }
//
//};