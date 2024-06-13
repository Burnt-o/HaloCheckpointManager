#pragma once
#include "IModalDialog.h"
#include "MCCState.h"
#include "ImGuiColorTextEdit\TextEditor.h"

class TriggerFilterStringDialog : public IModalDialogReturner<std::string>
{
private:
	std::string mOriginalValue;
	MCCState mMCCState;
	std::unique_ptr<TextEditor> textEditor;

public:
	TriggerFilterStringDialog(std::string dialogTitle, std::string defaultValue, MCCState mccState)
		: IModalDialogReturner
		(dialogTitle, defaultValue),
		mOriginalValue(defaultValue),
		mMCCState(mccState)
	{
	// setup texteditor
		textEditor = std::make_unique<TextEditor>();
		textEditor->SetText(defaultValue);
		textEditor->SetPalette(TextEditor::PaletteId::Mariana);
		textEditor->SetShowLineNumbersEnabled(false);
		textEditor->SetLanguageDefinition(TextEditor::LanguageDefinitionId::None);
		textEditor->SetShowWhitespacesEnabled(false);
	}



	virtual void renderInternal(SimpleMath::Vector2 screenSize) override
	{
		ImGui::Text("Trigger Name Filter List (exact match, semicolon seperated):");
		//ImGui::InputTextMultiline("##TriggerFilterStringDialog", &currentReturnValue);

		textEditor->Render("##TriggerFilterStringDialog", false, ImVec2(screenSize.x / 3.f, screenSize.y / 3.f), true);

		ImGui::Dummy(ImVec2(10, 10));

		if (mMCCState.currentGameState.operator GameState::Value() == GameState::Value::Halo1 
			&& mMCCState.currentLevelID == LevelID::_map_id_halo1_two_betrayals
			)
		{
			// note: techinically the mandatory triggers should include e51_a_trigger.
			// but that trigger is huge and basically impossible to miss,
			// and it's hugeness makes it hard to see some other more important triggers near it

			if (ImGui::Button("Load BOOL Triggers"))
			{
				textEditor->SetText(
					"e3_trigger; canyon3_base_trigger; canyon3_land_trigger; e6_trigger; \n"
					"pulse_1_trigger; e8_trigger; \n"
					"waypoint3_trigger; e20_a_trigger; e21_a_trigger; e21_b_trigger; \n"
					"e22_a_trigger; e30_a_trigger; jump_1_trigger; jump_2_trigger; \n"
					"jump_3_trigger; e33_a_trigger; e34_a_trigger; e37_a_trigger; \n"
					"e39_a_trigger; e40_a_trigger; e41_jump_1_trigger; e41_c_trigger; \n"
					"e43_a_trigger; e43_b_trigger; e43_c_trigger; e44_a_trigger; \n"
					"e46_a_trigger; e48_a_trigger; e50_a_trigger; e52_a_trigger; \n"
					"e52_c_trigger; pulse_2_trigger; c40_230_240_250_trigger; tunnel_trigger; \n"
					"e59_a_trigger; e59_b_trigger; e59_c_trigger; e60_a_trigger; \n"
					"e61_a_trigger; e62_a_trigger; pulse_3_trigger; \n");
			}

			if (ImGui::Button("Load BOOL+BSP Triggers"))
			{
				textEditor->SetText(
					"e3_trigger; canyon3_base_trigger; canyon3_land_trigger; e6_trigger; \n"
					"pulse_1_trigger; e8_trigger; \n"
					"waypoint3_trigger; e20_a_trigger; e21_a_trigger; e21_b_trigger; \n"
					"e22_a_trigger; e30_a_trigger; jump_1_trigger; jump_2_trigger; \n"
					"jump_3_trigger; e33_a_trigger; e34_a_trigger; e37_a_trigger; \n"
					"e39_a_trigger; e40_a_trigger; e41_jump_1_trigger; e41_c_trigger; \n"
					"e43_a_trigger; e43_b_trigger; e43_c_trigger; e44_a_trigger; \n"
					"e46_a_trigger; e48_a_trigger; e50_a_trigger; e52_a_trigger; \n"
					"e52_c_trigger; pulse_2_trigger; c40_230_240_250_trigger; tunnel_trigger; \n"
					"e59_a_trigger; e59_b_trigger; e59_c_trigger; e60_a_trigger; \n"
					"e61_a_trigger; e62_a_trigger; pulse_3_trigger; \n"
					"bsp 2,12; bsp 12,2; bsp 2,10; bsp 10,1; \n"
					"bsp 1,6; bsp 6,1; bsp 1,0; \n");
			}
		}


		if (ImGui::Button("Accept")) // no enter to accept since user might want that to create newlines in text editor
		{
			PLOG_DEBUG << "closing TriggerFilterStringDialog with Accept";
			currentReturnValue = textEditor->GetText();
			closeDialog();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel") || ImGui::IsKeyPressed(ImGuiKey_Escape))
		{
			PLOG_DEBUG << "closing TriggerFilterStringDialog with Cancel";
			currentReturnValue = mOriginalValue;
			closeDialog();
		}

	}
};