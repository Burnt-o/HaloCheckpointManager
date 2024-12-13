#pragma once
#include "IModalDialog.h"
#include "ViewAngleLineList.h"
#include "RuntimeExceptionHandler.h"

// largely a copy of  EditOrAddWaypointDialog


class EditOrAddViewAngleLineDialog : public IModalDialogReturner<std::optional<ViewAngleLine>> // returns std::nullopt if user cancels
{
private:
	std::optional<SubpixelID> mPlayerAngle;
	std::shared_ptr<RuntimeExceptionHandler> mRuntimeExceptionHandler;
	const ImGuiColorEditFlags editFlags = ImGuiColorEditFlags_Float | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_NoInputs;

public:
	EditOrAddViewAngleLineDialog(std::shared_ptr<RuntimeExceptionHandler> runtimeExceptionHandler, std::string dialogTitle, ViewAngleLine defaultViewAngleLine, std::optional<SubpixelID> playerAngle)
		: IModalDialogReturner(dialogTitle, defaultViewAngleLine),
		mPlayerAngle(playerAngle),
		mRuntimeExceptionHandler(runtimeExceptionHandler)
	{
	}


	void renderGlobalOption(bool& useGlobal, SimpleMath::Vector4& customColor, std::string optionName)
	{
		ImGui::Checkbox(std::format("Use Global {}", optionName).c_str(), &useGlobal);
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) { ImGui::SetTooltip(std::format("Use the global setting for {} or customise it per waypoint", optionName).c_str()); }

		{
			ImGui::BeginDisabled(useGlobal);

			ImGui::ColorEdit4(std::format("{}", optionName).c_str(), &customColor.x, editFlags);
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) { ImGui::SetTooltip(std::format("What colour should {} for this waypoint be?", optionName).c_str()); }

			ImGui::EndDisabled();
		}
	}

	void renderGlobalOption(bool& useGlobal, float& customScale, std::string optionName)
	{
		ImGui::Checkbox(std::format("Use Global {}", optionName).c_str(), &useGlobal);
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) { ImGui::SetTooltip(std::format("Use the global setting for {} or customise it per waypoint", optionName).c_str()); }

		{
			ImGui::BeginDisabled(useGlobal);

			ImGui::SetNextItemWidth(100.f);
			ImGui::InputFloat(std::format("{}", optionName).c_str(), &customScale);
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) { ImGui::SetTooltip(std::format("What scale (size) should {} for this waypoint be?", optionName).c_str()); }

			ImGui::EndDisabled();
		}
	}

	void renderGlobalOption(bool& useGlobal, int& customPrecision, std::string optionName)
	{
		ImGui::Checkbox(std::format("Use Global {}", optionName).c_str(), &useGlobal);
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) { ImGui::SetTooltip(std::format("Use the global setting for {} or customise it per waypoint", optionName).c_str()); }

		{
			ImGui::BeginDisabled(useGlobal);

			ImGui::SetNextItemWidth(100.f);
			ImGui::InputInt(std::format("{}", optionName).c_str(), &customPrecision);
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) { ImGui::SetTooltip("How precise (ie how many decimal points) should the decimal digit be?"); }

			ImGui::EndDisabled();
		}
	}

	virtual void renderInternal(SimpleMath::Vector2 screenSize) override
	{
		if (currentReturnValue.has_value() == false)
		{
			PLOG_ERROR << "Something went real bad if EditOrAddViewAngleLineDialog.currentReturnValue is null here";
			return;
		}
		auto& workingViewAngleLine = currentReturnValue.value();
		float angle = workingViewAngleLine.subpixelID;

		// Inputs to allow user to edit various aspects of the workingViewAngleLine

		ImGui::SetNextItemWidth(100);
		if (ImGui::InputFloat("View Angle", &angle, 0, 0, "%.7f"))
		{
			workingViewAngleLine.subpixelID = SubpixelID::fromFloat(angle);
		}

		ImGui::SetNextItemWidth(170);
		int workingSubpixel = *reinterpret_cast<int*>(&workingViewAngleLine.subpixelID);
		constexpr int maxSubpixelInt = 1078530012;
		if (ImGui::InputInt("Subpixel", &workingSubpixel))
		{
			if (workingSubpixel < 0 || workingSubpixel > maxSubpixelInt)
			{
				HCMRuntimeException ex(std::format("Invalid subpixel number! 0 < {} < {}", workingSubpixel, maxSubpixelInt));
				mRuntimeExceptionHandler->handleMessage(ex);
			}
			else
				workingViewAngleLine.subpixelID = *reinterpret_cast<SubpixelID*>(&workingSubpixel);
		}


		if (mPlayerAngle.has_value())
		{
			if (ImGui::Button("Fill with player angle"))
			{
				PLOG_DEBUG << "Filling viewangleline with player angle";
				workingViewAngleLine.subpixelID = SubpixelID::fromFloat(mPlayerAngle.value());
			}
		}

		ImGui::InputText("Label", &workingViewAngleLine.label);

		ImGui::Checkbox("Enabled", &workingViewAngleLine.viewAngleLineEnabled);
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) { ImGui::SetTooltip("Whether this ViewAngleLine is enabled (visible) or not."); }

		renderGlobalOption(workingViewAngleLine.spriteColorUseGlobal, workingViewAngleLine.spriteColor, "Line Color");

		{
			ImGui::Checkbox("Show Label", &workingViewAngleLine.showLabel);
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) { ImGui::SetTooltip("Whether to show the label (name) of this view angle"); }

			ImGui::BeginDisabled(workingViewAngleLine.showLabel == false);
			renderGlobalOption(workingViewAngleLine.labelColorUseGlobal, workingViewAngleLine.labelColor, "Label Text Color");
			renderGlobalOption(workingViewAngleLine.labelScaleUseGlobal, workingViewAngleLine.labelScale, "Label Text Scale");
			ImGui::EndDisabled();
		}

		{
			ImGui::Checkbox("Show Angle", &workingViewAngleLine.showAngle);
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) { ImGui::SetTooltip("Whether to show the angle (eg 0.1234567) of this view angle"); }

		}

		{
			ImGui::Checkbox("Show Subpixel ID", &workingViewAngleLine.showSubpixelID);
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) { ImGui::SetTooltip("Whether to show the subpixel ID of this view angle"); }

		}

		{
			ImGui::Checkbox("Show Distance Measure", &workingViewAngleLine.showDistance);
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) { ImGui::SetTooltip("Whether to show the distance measurement between the players current angle and the view angle line"); }

			ImGui::BeginDisabled(workingViewAngleLine.showDistance == false);
			ImGui::Checkbox("Measure As Subpixels", &workingViewAngleLine.measureAsSubpixels);
			renderGlobalOption(workingViewAngleLine.distanceColorUseGlobal, workingViewAngleLine.distanceColor, "Distance Text Color");
			renderGlobalOption(workingViewAngleLine.distanceScaleUseGlobal, workingViewAngleLine.distanceScale, "Distance Text Scale");
			ImGui::EndDisabled();

			ImGui::BeginDisabled(workingViewAngleLine.measureAsSubpixels == true);
			renderGlobalOption(workingViewAngleLine.distancePrecisionUseGlobal, workingViewAngleLine.distancePrecision, "Distance Text Precision");
			ImGui::EndDisabled();
		}



		// Copy to clipboard button
		if (ImGui::Button("Copy ViewAngleLine to Clipboard"))
		{
			pugi::xml_document doc;
			pugi::xml_node node = doc.append_child("viewAngleLine");
			workingViewAngleLine.serialise(node);
			std::ostringstream ss;
			node.print(ss);
			ImGui::SetClipboardText(ss.str().c_str());
		}

		// Paste from clipboard button
		if (ImGui::Button("Paste ViewAngleLine from Clipboard"))
		{
			try
			{
				pugi::xml_document doc;
				auto parseResult = doc.load_string(ImGui::GetClipboardText()); // todo check parse result
				if (!parseResult)
				{
					throw HCMSerialisationException(std::format("Error parsing clipboard text \"{}\": {}", ImGui::GetClipboardText(), parseResult.description()));
				}

				ViewAngleLine in(doc.first_child()); // constructor can throw HCMSerialisation
				workingViewAngleLine = in;
			}
			catch (HCMSerialisationException ex)
			{
				mRuntimeExceptionHandler->handleMessage(ex);
			}

		}



		// Ok button (return workingViewAngleLine)
		if (ImGui::Button("Ok") || ImGui::IsKeyPressed(ImGuiKey_Enter))
		{
			PLOG_DEBUG << "closing EditOrAddViewAngleLineDialog with OK";


			closeDialog();
		}

		// Cancel button (return std::nullopt)
		ImGui::SameLine();
		if (ImGui::Button("Cancel") || ImGui::IsKeyPressed(ImGuiKey_Escape))
		{
			PLOG_DEBUG << "closing EditOrAddViewAngleLineDialog with Cancel";
			currentReturnValue = std::nullopt;
			closeDialog();
		}



	}
};