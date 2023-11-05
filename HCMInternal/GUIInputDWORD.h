#pragma once
#include "IGUIElement.h"
#include "SettingsStateAndEvents.h"

template <bool asHexadecimal>
class GUIInputDWORD : public IGUIElement
{

private:
	std::string mLabelText;
	std::string mHotkeyButtonLabelId = std::format("..###{}", getName());
	std::weak_ptr<Setting<uint32_t>> mOptionDWORDWeak;
	std::optional<std::shared_ptr<ActionEvent>> mEventToFire;
	std::vector<std::thread> mFireEventThreads;

public:

	GUIInputDWORD(GameState implGame, ToolTipCollection tooltip, std::string labelText, std::weak_ptr<Setting<uint32_t>> optionDWORD, std::optional<std::shared_ptr<ActionEvent>> eventToFire = std::nullopt)
		: IGUIElement(implGame, std::nullopt, tooltip), mOptionDWORDWeak(optionDWORD), mLabelText(labelText), mEventToFire(eventToFire)
	{
		if (mLabelText.empty()) throw HCMInitException("Cannot have empty label (needs label for imgui ID system, use ## for invisible labels)");
		PLOG_VERBOSE << "Constructing GUIInputDWORD, name: " << getName();
		this->currentHeight = GUIFrameHeightWithSpacing;
	}

	void render(HotkeyRenderer& hotkeyRenderer) override
	{


		auto mOptionDWORD = mOptionDWORDWeak.lock();
		if (!mOptionDWORD)
		{
			PLOG_ERROR << "bad mSettings weakptr when rendering " << getName();
			return;
		}


		ImGui::SetNextItemWidth(100);

		if (ImGui::InputScalar(mLabelText.c_str(), ImGuiDataType_U32, &mOptionDWORD->GetValueDisplay(), NULL, NULL, "%04X"))
		{
				mOptionDWORD->UpdateValueWithInput();
				PLOG_VERBOSE << "GUIInputDWORD firing input event, value: " << std::hex << mOptionDWORD->GetValue();
		}
		renderTooltip();
		DEBUG_GUI_HEIGHT;
		if (mEventToFire.has_value())
		{
			ImGui::SameLine();
			if (ImGui::Button(std::format("Send##{}", mLabelText).c_str()))
			{
				mOptionDWORD->UpdateValueWithInput();
				PLOG_VERBOSE << "GUIInputDWORD firing send event, value: " << std::hex << mOptionDWORD->GetValue();
				auto& newThread = mFireEventThreads.emplace_back(std::thread([mEvent = mEventToFire.value()]() {mEvent->operator()(); }));
				newThread.detach();
			}
			renderTooltip();
		}



	}
	~GUIInputDWORD()
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