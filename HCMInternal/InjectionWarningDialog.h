#pragma once
#include "IModalDialog.h"


class InjectionWarningDialog : public IModalDialogReturner<bool>
{
private:
	std::string mWarningText;
public:
	InjectionWarningDialog(std::string dialogTitle, std::string warningText) : IModalDialogReturner(dialogTitle, false),
	mWarningText(warningText){}


	virtual void renderInternal(SimpleMath::Vector2 screenSize) override
	{

			ImGui::Text(mWarningText.c_str());
			if (ImGui::Button("Continue") || ImGui::IsKeyPressed(ImGuiKey_Enter))
			{
				PLOG_DEBUG << "closing InjectionWarningDialog with continue";
				currentReturnValue = true;
				closeDialog();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel") || ImGui::IsKeyPressed(ImGuiKey_Escape))
			{
				PLOG_DEBUG << "closing InjectionWarningDialog with Cancel";
				currentReturnValue = false;
				closeDialog();
			}


		
	}
};