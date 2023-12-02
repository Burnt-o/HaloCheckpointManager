#pragma once
#include "IGUIElement.h"
#include "SettingsStateAndEvents.h"

template <typename E, float itemWidth>
class GUIComboEnumWithChildren : public IGUIElement {

private:
	std::string mLabelText;
	std::weak_ptr<Setting<int>> mOptionEnumWeak;
	std::vector<std::thread> mUpdateSettingThreads;
	static constexpr std::array<std::string_view, magic_enum::enum_count<E>()> itemLabels = magic_enum::enum_names<E>();
	std::string itemLabelsMashedTogether;
	float mLeftMargin;
	std::vector<std::vector<std::shared_ptr<IGUIElement>>> mChildElementsPerItem;

public:


	GUIComboEnumWithChildren(GameState implGame, ToolTipCollection tooltip, std::string labelText, std::shared_ptr<Setting<int>> optionEnum, std::vector<std::vector<std::optional<std::shared_ptr<IGUIElement>>>> childElementsPerItem, float leftMargin = 20.f)
		: IGUIElement(implGame, std::nullopt, tooltip), mLabelText(labelText), mOptionEnumWeak(optionEnum), mLeftMargin(leftMargin)
	{
		if (mLabelText.empty()) throw HCMInitException("Cannot have empty label (needs label for imgui ID system, use ## for invisible labels)");
		PLOG_VERBOSE << "Constructing GUIComboEnumWithChildren, name: " << getName();
		PLOG_DEBUG << "GUIComboEnumWithChildren.getOptionName: " << std::hex << mOptionEnumWeak.lock()->getOptionName();

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


		assert(childElementsPerItem.size() <= itemLabels.size() && "Too many items in GUIComboEnumWithChildren childElementsPerItem vector!");



		for (auto maybeChildElements : childElementsPerItem)
		{
			std::vector<std::shared_ptr<IGUIElement>> childElements;
			for (auto maybeElement : maybeChildElements)
			{
				if (maybeElement.has_value()) childElements.push_back(maybeElement.value());
			}
			mChildElementsPerItem.push_back(childElements);
		}


		PLOG_DEBUG << "GUIComboEnumWithChildren itemLabelsMashedTogether: " << std::endl << debugString;
	}

	void render(HotkeyRenderer& hotkeyRenderer) override
	{


		auto mOptionEnum = mOptionEnumWeak.lock();
		if (!mOptionEnum)
		{
			PLOG_ERROR << "bad mOptionEnum weakptr when rendering " << getName();
			return;
		}

		ImGui::SetNextItemWidth(itemWidth);
		const char* combo_preview_value = itemLabels.at(mOptionEnum->GetValueDisplay()).data();
		if (ImGui::BeginCombo(mLabelText.c_str(), combo_preview_value))
		{
			for (int n = 0; n < itemLabels.size(); n++)
			{
				const bool is_selected = (mOptionEnum->GetValueDisplay() == n);
				if (ImGui::Selectable(itemLabels.at(n).data(), is_selected))
				{
					mOptionEnum->GetValueDisplay() = n;
					PLOG_VERBOSE << "GUIComboEnumWithChildren (" << getName() << ") firing toggle event, new value: " << magic_enum::enum_name<E>((E)n);
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


		currentHeight = GUIFrameHeightWithSpacing;

		// render children
		int currentSelection = mOptionEnum->GetValue();
		if (mChildElementsPerItem.size() - 1 < currentSelection)
		{
			// this selection never had children so we're done
			return;
		}

		auto& currentSelectionChildElements = mChildElementsPerItem.at(currentSelection);


		for (auto& element : currentSelectionChildElements)
		{
			auto thisElementHeight = element->getCurrentHeight();
			currentHeight += thisElementHeight;

			ImGui::Dummy({ mLeftMargin, thisElementHeight < 3 ? 0 : thisElementHeight - GUISpacing }); // left margin
			ImGui::SameLine();
			element->render(hotkeyRenderer);
		}

	}

	~GUIComboEnumWithChildren()
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