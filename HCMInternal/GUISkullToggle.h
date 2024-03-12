#pragma once
#include "IGUIElement.h"
#include "SettingsStateAndEvents.h"
// Very similiar to GUIHeading but with less visual contrast and a parameter to add a left margin to it and it's child elements
template<bool startsOpen>
class GUISkullToggle : public IGUIElement {

private:
	std::string mHeadingText;
	bool headingOpen = false;
	float mLeftMargin;
	std::weak_ptr<SettingsStateAndEvents> mSettingsWeak;
public:


	GUISkullToggle(GameState implGame, ToolTipCollection tooltip, std::string headingText, std::shared_ptr<SettingsStateAndEvents> settings, float leftMargin = 20.f)
		: IGUIElement(implGame, std::nullopt, tooltip), mLeftMargin(leftMargin), mHeadingText(headingText), mSettingsWeak(settings)
	{
		PLOG_VERBOSE << "Constructing GUISkullToggle, name: " << getName();



		this->currentHeight = GUIFrameHeightWithSpacing;

	}

	void render(HotkeyRenderer& hotkeyRenderer) override
	{

		auto mSettings = mSettingsWeak.lock();
		if (!mSettings) { PLOG_ERROR << "bad mSettings weak ptr"; return; }
		
		ImGui::BeginChild(mHeadingText.c_str(), { 500 - mLeftMargin, currentHeight - GUISpacing });


		constexpr ImGuiTreeNodeFlags treeFlags = startsOpen ? ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_DefaultOpen : ImGuiTreeNodeFlags_FramePadding;
		headingOpen = ImGui::TreeNodeEx(mHeadingText.c_str(), treeFlags);
		renderTooltip();
		DEBUG_GUI_HEIGHT;
		if (headingOpen)
		{

			currentHeight = GUIFrameHeightWithSpacing;

			// tell optional cheat service to update bit bools
			mSettings->updateSkullBitBoolCollectionEvent->operator()(mImplGame);

			
			if (mSettings->skullBitBoolCollectionInUse) // await update if necessary
			{
				mSettings->skullBitBoolCollectionInUse.wait(true);
			}
			
			// lock it
			ScopedAtomicBool lock(mSettings->skullBitBoolCollectionInUse);


			if (mSettings->skullBitBoolCollection.empty())
			{
				float thisElementHeight = GUIFrameHeightWithSpacing;
				currentHeight += thisElementHeight; // height of the error message
				ImGui::Text("Null skull pointers! See log");
			}
			else
			{
				// render each available skull toggle 
				for (auto& [skullEnumKey, bitBoolPointer] : mSettings->skullBitBoolCollection)
				{
					float thisElementHeight = GUIFrameHeightWithSpacing;
					currentHeight += thisElementHeight; // height of the checkbox

					ImGui::Dummy({ mLeftMargin, thisElementHeight < 3 ? 0 : thisElementHeight - GUISpacing }); // left margin
					ImGui::SameLine();

					// TODO: render hotkey (need enum map of skullEnum -> hotkeyEnum)

					bool tempSkullValue = bitBoolPointer.operator const bool();
					if (ImGui::Checkbox(magic_enum::enum_name(skullEnumKey).data(), &tempSkullValue)) // TODO: replace enum_name with display name via map
					{
						bitBoolPointer.set(tempSkullValue);
					}

					// TODO: tool tip (need map of skullEnum -> toolTip)
				}
			}
			


			ImGui::TreePop();
		}
		else
		{
			currentHeight = GUIFrameHeightWithSpacing;
		}
		ImGui::EndChild();


	}

	std::string_view getName() override { return mHeadingText; }

};