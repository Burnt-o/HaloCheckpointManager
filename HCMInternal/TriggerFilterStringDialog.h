#pragma once
#include "IModalDialog.h"
#include "MCCState.h"
#include "ImGuiColorTextEdit\TextEditor.h"

class TriggerFilterStringDialog : public IModalDialogReturner<std::string>
{
private:
	std::string mOriginalValue;
	std::unique_ptr<TextEditor> textEditor;
	std::string allTriggers;
	LevelID currentLevel; 
	std::optional<std::shared_ptr<std::map<LevelID, std::vector<std::pair<std::string, std::string>>>>> levelMapStringVector;

public:
	TriggerFilterStringDialog(std::string dialogTitle, std::string defaultValue, std::string allTriggers, LevelID currentLevel, std::optional<std::shared_ptr<std::map<LevelID, std::vector<std::pair<std::string, std::string>>>>> levelMapStringVector)
		: IModalDialogReturner
		(dialogTitle, defaultValue),
		mOriginalValue(defaultValue),
		levelMapStringVector(levelMapStringVector),
		currentLevel(currentLevel),
		allTriggers(allTriggers)
	{
	// setup texteditor
		textEditor = std::make_unique<TextEditor>();
		textEditor->SetText(defaultValue);
		textEditor->SetPalette(TextEditor::GetMarianaPalette());
		textEditor->SetShowLineNumbersEnabled(false);
		textEditor->SetShowWhitespacesEnabled(false);
	}



	virtual void renderInternal(SimpleMath::Vector2 screenSize) override
	{
		ImGui::Text("Trigger Name Filter List (exact match, semicolon seperated):");
		//ImGui::InputTextMultiline("##TriggerFilterStringDialog", &currentReturnValue);
		ImGui::PushTextWrapPos();
		textEditor->Render("##TriggerFilterStringDialog", false, ImVec2(screenSize.x / 3.f, screenSize.y / 3.f), true);
		ImGui::PopTextWrapPos();

		ImGui::Dummy(ImVec2(10, 10));

		if (levelMapStringVector.has_value() && levelMapStringVector.value()->contains(currentLevel))
		{
			for (auto& [listName, listValue] : levelMapStringVector.value()->at(currentLevel))
			{
				if (ImGui::Button(std::format("Load {} Triggers", listName).c_str()))
				{
					textEditor->SetText(listValue);
				}
			}
		}
	

		if (ImGui::Button("Load All Triggers"))
		{
			textEditor->SetText(allTriggers);
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