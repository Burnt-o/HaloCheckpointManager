#pragma once
#include "IModalDialog.h"


class CheckpointDumpNameDialog : public IModalDialogReturner<std::tuple<bool, std::string>>
{
public:
	CheckpointDumpNameDialog(std::string dialogTitle, std::string defaultValue) 
		: IModalDialogReturner
		(dialogTitle, std::tuple<bool, std::string>(false, defaultValue)) {}



	virtual void renderInternal(SimpleMath::Vector2 screenSize) override
	{
		
			ImGui::InputText("Name: ", &std::get<std::string>(currentReturnValue));
			if (ImGui::Button("Accept") || ImGui::IsKeyPressed(ImGuiKey_Enter))
			{
				PLOG_DEBUG << "closing CheckpointDumpNameDialog with Accept";
				std::get<bool>(currentReturnValue) = true;
				closeDialog();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel") || ImGui::IsKeyPressed(ImGuiKey_Escape))
			{
				PLOG_DEBUG << "closing CheckpointDumpNameDialog with Cancel";
				std::get<bool>(currentReturnValue) = false;
				closeDialog();
			}

	}
};