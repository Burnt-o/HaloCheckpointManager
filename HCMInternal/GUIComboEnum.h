#pragma once
#include "IGUIElement.h"
#include "SettingsStateAndEvents.h"

template <typename E>
class GUIComboEnum : public IGUIElement {

private:
	std::string mLabelText;
	std::weak_ptr<Setting<int>> mOptionEnumWeak;
	std::vector<std::thread> mUpdateSettingThreads;
	static constexpr std::array<std::string_view, magic_enum::enum_count<E>()> itemLabels = magic_enum::enum_names<E>();
	std::string itemLabelsMashedTogether;
public:


	GUIComboEnum(GameState implGame, ToolTipCollection tooltip, std::string labelText, std::shared_ptr<Setting<int>> optionEnum)
		: IGUIElement(implGame, std::nullopt, tooltip), mLabelText(labelText), mOptionEnumWeak(optionEnum)
	{
		if (mLabelText.empty()) throw HCMInitException("Cannot have empty label (needs label for imgui ID system, use ## for invisible labels)");
		PLOG_VERBOSE << "Constructing GUIComboEnum, name: " << getName();
		PLOG_DEBUG << "GUIComboEnum.getOptionName: " << std::hex << mOptionEnumWeak.lock()->getOptionName();

		this->currentHeight = GUIFrameHeightWithSpacing;

		for (std::string_view sv : itemLabels)
		{
			itemLabelsMashedTogether.append(sv.data());
			itemLabelsMashedTogether.append("\0");
		}
		itemLabelsMashedTogether.append("\0");

		std::string debugString;
		for (auto sv : itemLabels)
		{
			debugString.append(sv.data());
			debugString.append("\\0");
		}
		debugString.append("\\0");




		PLOG_DEBUG << "GUIComboEnum itemLabelsMashedTogether: " << std::endl << debugString;
	}

	void render(HotkeyRenderer& hotkeyRenderer) override
	{


		auto mOptionEnum = mOptionEnumWeak.lock();
		if (!mOptionEnum)
		{
			PLOG_ERROR << "bad mOptionEnum weakptr when rendering " << getName();
			return;
		}

		ImGui::SetNextItemWidth(200);
		const char* combo_preview_value = itemLabels.at(mOptionEnum->GetValueDisplay()).data();
		if (ImGui::BeginCombo(mLabelText.c_str(), combo_preview_value))
		{
			for (int n = 0; n < itemLabels.size(); n++)
			{
				const bool is_selected = (mOptionEnum->GetValueDisplay() == n);
				if (ImGui::Selectable(itemLabels.at(n).data(), is_selected))
				{
					mOptionEnum->GetValueDisplay() = n;
					PLOG_VERBOSE << "GUIComboEnum (" << getName() << ") firing toggle event, new value: " << magic_enum::enum_name<E>((E)n);
					auto& newThread = mUpdateSettingThreads.emplace_back(std::thread([optionToggle = mOptionEnum]() { optionToggle->UpdateValueWithInput(); }));
					newThread.detach();
				}

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();




		}
		renderTooltip();
		DEBUG_GUI_HEIGHT;


	}

	~GUIComboEnum()
	{
		for (auto& thread : mUpdateSettingThreads)
		{
			if (thread.joinable())
			{
				PLOG_DEBUG << getName() << " joining mUpdateSettingThread";
				thread.join();
				PLOG_DEBUG << getName() << " mUpdateSettingThread finished";
			}

		}
	}

	std::string_view getName() override { return mLabelText; }

};