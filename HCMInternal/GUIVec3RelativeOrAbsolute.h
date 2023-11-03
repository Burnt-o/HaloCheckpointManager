//#pragma once
//#include "IGUIElement.h"
//#include "SettingsStateAndEvents.h"
//
//class GUIVec3RelativeOrAbsolute : public IGUIElement {
//
//private:
//	std::string mLabelText;
//	std::weak_ptr<Setting<SimpleMath::Vector3>> mOptionVec3Weak;
//	std::weak_ptr<Setting<bool>> mLabelToggle;
//	std::vector<std::thread> mUpdateSettingThreads;
//	bool mAbsoluteWhen;
//public:
//
//
//	GUIVec3RelativeOrAbsolute(GameState implGame, std::string labelText, std::shared_ptr<Setting<SimpleMath::Vector3>> optionVec3, std::shared_ptr<Setting<bool>> labelToggle, bool absoluteWhen)
//		: IGUIElement(implGame, std::nullopt), mLabelText(labelText), mOptionVec3Weak(optionVec3), mLabelToggle(labelToggle), mAbsoluteWhen(absoluteWhen)
//	{
//		if (mLabelText.empty()) throw HCMInitException("Cannot have empty vec3 label (needs label for imgui ID system, use ## for invisible labels)");
//		PLOG_VERBOSE << "Constructing GUIVec3RelativeOrAbsolute, name: " << getName();
//		PLOG_DEBUG << "mOptionVec3Weak.getOptionName: " << std::hex << mOptionVec3Weak.lock()->getOptionName();
//
//		this->currentHeight = GUISpacing + (GUIFrameHeightWithSpacing * 4);
//	}
//
//	void render(HotkeyRenderer& hotkeyRenderer) override
//	{
//
//
//		auto mOptionVec3 = mOptionVec3Weak.lock();
//		if (!mOptionVec3)
//		{
//			PLOG_ERROR << "bad mOptionVec3 weakptr when rendering " << getName();
//			return;
//		}
//
//
//		std::string xLabel;
//		std::string yLabel;
//		std::string zLabel;
//		std::string type;
//		if (mLabelToggle.expired() || mLabelToggle.lock()->GetValue() == mAbsoluteWhen)
//		{
//			xLabel = "x";
//			yLabel = "y";
//			zLabel = "z";
//			type = "Absolute";
//		}
//		else
//		{
//			xLabel = "Forward";
//			yLabel = "Right";
//			zLabel = "Up";
//			type = "Relative";
//		}
//
//
//
//		ImGui::BeginChild(std::format("##", mLabelText).c_str(), {0, currentHeight - GUISpacing});
//		ImGui::AlignTextToFramePadding();
//		ImGui::Text(std::format("{} {}", type, mLabelText).c_str());
//		// 3 float boxes each seperately bound to x, y, z of mOptionVec3. Each will render on a seperate horizontal line.
//		ImGui::SetNextItemWidth(100);
//		if (ImGui::InputFloat(std::format("{}##{}", xLabel, mLabelText).c_str(), &mOptionVec3->GetValueDisplay().x, 0, 0, "%.8f"))
//		{
//			PLOG_VERBOSE << "GUIVec3RelativeOrAbsolute (" << getName() << ") firing toggle event, new value: " << mOptionVec3->GetValueDisplay();
//			auto& newThread = mUpdateSettingThreads.emplace_back(std::thread([optionToggle = mOptionVec3]() { optionToggle->UpdateValueWithInput(); }));
//			newThread.detach();
//		}
//
//		ImGui::SetNextItemWidth(100);
//		if (ImGui::InputFloat(std::format("{}##{}", yLabel, mLabelText).c_str(), &mOptionVec3->GetValueDisplay().y, 0, 0, "%.8f"))
//		{
//			PLOG_VERBOSE << "GUIVec3RelativeOrAbsolute (" << getName() << ") firing toggle event, new value: " << mOptionVec3->GetValueDisplay();
//			auto& newThread = mUpdateSettingThreads.emplace_back(std::thread([optionToggle = mOptionVec3]() { optionToggle->UpdateValueWithInput(); }));
//			newThread.detach();
//		}
//
//		ImGui::SetNextItemWidth(100);
//		if (ImGui::InputFloat(std::format("{}##{}", zLabel, mLabelText).c_str(), &mOptionVec3->GetValueDisplay().z, 0, 0, "%.8f"))
//		{
//			PLOG_VERBOSE << "GUIVec3RelativeOrAbsolute (" << getName() << ") firing toggle event, new value: " << mOptionVec3->GetValueDisplay();
//			auto& newThread = mUpdateSettingThreads.emplace_back(std::thread([optionToggle = mOptionVec3]() { optionToggle->UpdateValueWithInput(); }));
//			newThread.detach();
//		}
//		ImGui::EndChild();
//		
//		DEBUG_GUI_HEIGHT;
//
//	}
//
//	~GUIVec3RelativeOrAbsolute()
//	{
//		for (auto& thread : mUpdateSettingThreads)
//		{
//			if (thread.joinable())
//			{
//				PLOG_DEBUG << getName() << " joining mUpdateSettingThread";
//				thread.join();
//				PLOG_DEBUG << getName() << " mUpdateSettingThread finished";
//			}
//
//		}
//	}
//
//	std::string_view getName() override { return mLabelText; }
//
//};