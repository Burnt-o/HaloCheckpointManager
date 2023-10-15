#include "pch.h"
#include "ModalDialogRenderer.h"
#include "imgui.h"
#include "imgui_stdlib.h"
#include "GlobalKill.h"
template<typename dialogReturnType>
class IModalDialog
{
protected:
	std::string mDialogTitle;
	bool isOpen = false;
	bool needToBeginDialog = false;

public:
	IModalDialog(std::string dialogTitle) : mDialogTitle(dialogTitle) 
	{
	}


	void beginDialog() 
	{
		needToBeginDialog = true; // call to openPopup needs to happen in imgui frame (ie in render())
		isOpen = true;
	} 
	dialogReturnType currentReturnValue;
	bool isDialogOpen() { return isOpen; }
	virtual void render() = 0;
	virtual ~IModalDialog() = default;
};

class CheckpointDumpNameDialog : public IModalDialog<std::tuple<bool, std::string>>
{
public:
	CheckpointDumpNameDialog() : IModalDialog("Name dumped checkpoint") {}
	virtual void render()
	{
		if (needToBeginDialog)
		{
			PLOG_DEBUG << "opening popup " << mDialogTitle;
			ImGui::OpenPopup(mDialogTitle.c_str());
			needToBeginDialog = false;
		}

		if (ImGui::BeginPopupModal(mDialogTitle.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::InputText("Name: ", &std::get<std::string>(currentReturnValue));
			if (ImGui::Button("Accept"))
			{
				PLOG_DEBUG << "closing CheckpointDumpNameDialog with Accept";
				std::get<bool>(currentReturnValue) = true;
				isOpen = false;
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::Button("Cancel"))
			{
				PLOG_DEBUG << "closing CheckpointDumpNameDialog with Cancel";
				std::get<bool>(currentReturnValue) = false;
				isOpen = false;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}
};


class ModalDialogRenderer::ModalDialogRendererImpl
{
private:
	CheckpointDumpNameDialog checkpointDumpNameDialog;

		ScopedCallback<RenderEvent> mImGuiRenderCallbackHandle;
		void onImGuiRenderEvent(Vec2 screenSize)
		{
			checkpointDumpNameDialog.render();
		}


		gsl::not_null<std::shared_ptr< FreeMCCCursor>> mFreeMCCCursor;

public:
	std::tuple<bool, std::string> showCheckpointDumpNameDialog(std::string defaultName)
	{
		auto scopedFreeCursor = mFreeMCCCursor->scopedRequest(nameof(showCheckpointDumpNameDialog));
		checkpointDumpNameDialog.beginDialog();
		while (!GlobalKill::isKillSet() && checkpointDumpNameDialog.isDialogOpen()) { Sleep(10); }
		PLOG_DEBUG << "showCheckpointDumpNameDialog returning " << std::get<bool>(checkpointDumpNameDialog.currentReturnValue) << ", " << std::get<std::string>(checkpointDumpNameDialog.currentReturnValue);
		return checkpointDumpNameDialog.currentReturnValue;
	}
	bool showCheckpointInjectWrongLevelWarningDialog(std::string expectedLevel, std::string observedLevel)
	{
		throw HCMRuntimeException("not impl yet");
	}

	ModalDialogRendererImpl(std::shared_ptr<RenderEvent> pRenderEvent, std::shared_ptr< FreeMCCCursor> freeCursor)
		: mImGuiRenderCallbackHandle(pRenderEvent, [this](Vec2 screenSize) {onImGuiRenderEvent(screenSize); }),
		mFreeMCCCursor(freeCursor)
	{
	}
};


ModalDialogRenderer::ModalDialogRenderer(std::shared_ptr<RenderEvent> pRenderEvent, std::shared_ptr< FreeMCCCursor> freeCursor)
	: pimpl(std::make_unique<ModalDialogRendererImpl>(pRenderEvent, freeCursor)) {}

ModalDialogRenderer::~ModalDialogRenderer() = default;



std::tuple<bool, std::string> ModalDialogRenderer::showCheckpointDumpNameDialog(std::string defaultValue) { return pimpl->showCheckpointDumpNameDialog(defaultValue); }
bool ModalDialogRenderer::showCheckpointInjectWrongLevelWarningDialog(std::string expectedLevel, std::string observedLevel) { return pimpl->showCheckpointInjectWrongLevelWarningDialog(expectedLevel, observedLevel); }