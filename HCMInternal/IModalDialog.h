#pragma once
#include "pch.h"
#include "SharedRequestProvider.h"
#include "imgui.h"
#include "imgui_stdlib.h"

class IModalDialog
{
public:
	virtual void render(SimpleMath::Vector2 screenSize) = 0;
	//virtual ~IModalDialog() = default;
};


class IModalDialogVoid : public IModalDialog
{
private:
	bool isOpen = true;
	bool emergencyClose = false;
	bool needToBeginDialog = true;
	std::optional<std::shared_ptr<SharedRequestToken>> hotkeyDisableRequest = std::nullopt;

protected:
	std::string mDialogTitle;
	//~IModalDialogVoid() {}
	//IModalDialogVoid(const IModalDialogVoid&) {}

public:
	IModalDialogVoid(std::string dialogTitle)
		: mDialogTitle(dialogTitle)
	{
	}

	void disableHotkeys(std::shared_ptr<TokenSharedRequestProvider> hotkeyDisabler)
	{
		hotkeyDisableRequest = hotkeyDisabler->makeScopedRequest();
	}

	void queueEmergencyClose()
	{
		hotkeyDisableRequest = std::nullopt;
		emergencyClose = true;
	}

	void closeDialog() // must be called from "render" within ImGui::BeginPopupModal
	{
		isOpen = false;
		hotkeyDisableRequest = std::nullopt;
		ImGui::CloseCurrentPopup();
	}


	bool isDialogOpen() { return isOpen; }


	virtual void renderInternal(SimpleMath::Vector2 screenSize) = 0;

	// wraps renderInternal to handle popup logic
	virtual void render(SimpleMath::Vector2 screenSize) final {


		if (isOpen == false)
		{
			if (hotkeyDisableRequest)
			hotkeyDisableRequest = std::nullopt;
			return;
		}


		if (needToBeginDialog)
		{
			PLOG_DEBUG << "opening modal popup: \"" << mDialogTitle << "\"";
			ImGui::OpenPopup(mDialogTitle.c_str());
			needToBeginDialog = false;
		}

		if (ImGui::BeginPopupModal(mDialogTitle.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			if (emergencyClose)
			{
				closeDialog();
				ImGui::EndPopup();
				return;
			}

			renderInternal(screenSize);

			ImGui::EndPopup();
		}

	}


};


template<typename dialogReturnType>
class IModalDialogReturner : public IModalDialogVoid
{

protected:
	dialogReturnType currentReturnValue;
	//~IModalDialogReturner() {}
	//IModalDialogReturner(const IModalDialogReturner&) {}

public:
	IModalDialogReturner(std::string dialogTitle, dialogReturnType defaultReturnValue)
		: IModalDialogVoid(dialogTitle),
		currentReturnValue(defaultReturnValue)
	{
	}


	virtual void renderInternal(SimpleMath::Vector2 screenSize) = 0;

	dialogReturnType getReturnValue() // safe to call anytime
	{
		return currentReturnValue;
	}

	
};