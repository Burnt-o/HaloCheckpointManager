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

class InjectionWarningDialog : public IModalDialog<bool>
{
private:
	std::string mText = "text not set, burnt made an oopsie";
public:
	InjectionWarningDialog() : IModalDialog("Injection warning") {}

	void setTitle(std::string dialogTitle) { mDialogTitle = dialogTitle; }
	void setText(std::string text) { mText = text; }

	virtual void render()
	{
		if (needToBeginDialog)
		{
			PLOG_DEBUG << "opening modal popup: \"" << mDialogTitle << "\"";
			ImGui::OpenPopup(mDialogTitle.c_str());
			needToBeginDialog = false;
		}

		if (ImGui::BeginPopupModal(mDialogTitle.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text(mText.c_str());
			if (ImGui::Button("Continue"))
			{
				PLOG_DEBUG << "closing InjectionWarningDialog with continue";
				currentReturnValue = true;
				isOpen = false;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				PLOG_DEBUG << "closing InjectionWarningDialog with Cancel";
				currentReturnValue = false;
				isOpen = false;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}
};

class CheckpointDumpNameDialog : public IModalDialog<std::tuple<bool, std::string>>
{
private:
public:
	CheckpointDumpNameDialog() : IModalDialog("Name dumped checkpoint") {}

	void setTitle(std::string dialogTitle) { mDialogTitle = dialogTitle; }


	virtual void render()
	{
		if (needToBeginDialog)
		{
			PLOG_DEBUG << "opening modal popup: \"" << mDialogTitle << "\"";
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
			ImGui::SameLine();
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

class FailedOptionalCheatServicesDialog : public IModalDialog<std::tuple<>>
{
private:
	std::map<GameState, std::vector<std::pair<GUIElementEnum, std::string>>> guiFailureMap;
	bool anyErrorsAtAll = false;
public:
	FailedOptionalCheatServicesDialog() : IModalDialog("Failed optional cheat services")
	{
	}

	void setGUIFailures(std::shared_ptr<GUIServiceInfo> guiFailures)
	{
		// reset guiFailureMap to empty state
		guiFailureMap =
		{
			{GameState::Value::Halo1, {} },
			{GameState::Value::Halo2, {} },
			{GameState::Value::Halo3, {} },
			{GameState::Value::Halo3ODST, {} },
			{GameState::Value::HaloReach, {} },
			{GameState::Value::Halo4, {} },
		};

		// fill up guiFailureMap
		anyErrorsAtAll = false;
		for (auto& [gameGuiPair, errorMessage] : guiFailures->getFailureMessagesMap())
		{
			anyErrorsAtAll = true;
			guiFailureMap.at(gameGuiPair.first).emplace_back(std::pair<GUIElementEnum, std::string>{ gameGuiPair.second, errorMessage });
		}
	}
	virtual void render()
	{
		if (needToBeginDialog)
		{
			PLOG_DEBUG << "opening modal popup: \"" << mDialogTitle << "\"";
			ImGui::OpenPopup(mDialogTitle.c_str());
			needToBeginDialog = false;
		}


		if (ImGui::BeginPopupModal(mDialogTitle.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{

			ImGui::Text("Services that failed to initialise are shown below, grouped by game. ");

			if (!anyErrorsAtAll)
			{
				ImGui::Text("All optional services successfully initialised!");
			}
			else
			{
				for (auto& [game, enumMessagePairVector] : guiFailureMap)
				{
					if (enumMessagePairVector.empty()) continue;

					if (ImGui::TreeNodeEx(game.toString().c_str(), ImGuiTreeNodeFlags_FramePadding))
					{
						for (auto enumMessagePair : enumMessagePairVector)
						{
							ImGui::Text(std::format("{} service failed! Error: ", magic_enum::enum_name(enumMessagePair.first)).c_str());
							ImGui::Text(enumMessagePair.second.c_str());
							ImGui::Dummy({ 2,2 }); // padding between messages
						}
						ImGui::TreePop();
					}
				}
			}

			if (ImGui::Button("Ok"))
			{
				PLOG_DEBUG << "closing FailedOptionalCheatServicesDialog with Ok";
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
	InjectionWarningDialog injectionWarningDialog;
	FailedOptionalCheatServicesDialog failedOptionalCheatServicesDialog;

		ScopedCallback<RenderEvent> mImGuiRenderCallbackHandle;
		void onImGuiRenderEvent(Vec2 screenSize)
		{
			checkpointDumpNameDialog.render();
			injectionWarningDialog.render();
			failedOptionalCheatServicesDialog.render();
		}


		std::optional<std::shared_ptr< FreeMCCCursor>> mFreeMCCCursorService;
		std::optional<std::shared_ptr< BlockGameInput>> mBlockGameInputService;
		std::optional<std::shared_ptr< PauseGame>> mPauseGameService;

		std::shared_ptr<GUIServiceInfo> guiFailures;

		std::tuple<std::unique_ptr<ScopedServiceRequest>, std::unique_ptr<ScopedServiceRequest>, std::unique_ptr<ScopedServiceRequest>> getScopedRequests()
		{
			std::unique_ptr<ScopedServiceRequest> scopedFreeCursorRequest;
			if (mFreeMCCCursorService.has_value())
			{
				PLOG_DEBUG << "ModalDialogRenderer requesting freeMCCCursor";
				scopedFreeCursorRequest = mFreeMCCCursorService.value()->scopedRequest(nameof(showCheckpointDumpNameDialog));
			}

			std::unique_ptr<ScopedServiceRequest> scopedBlockInputRequest;
			if (mBlockGameInputService.has_value())
			{
				PLOG_DEBUG << "ModalDialogRenderer requesting blockGameInput";
				scopedBlockInputRequest = mBlockGameInputService.value()->scopedRequest(nameof(showCheckpointDumpNameDialog));
			}

			std::unique_ptr<ScopedServiceRequest> scopedPauseRequest;
			if (mPauseGameService.has_value())
			{
				PLOG_DEBUG << "ModalDialogRenderer requesting pauseGame";
				scopedPauseRequest = mPauseGameService.value()->scopedRequest(nameof(showCheckpointDumpNameDialog));
			}

			return std::tuple<std::unique_ptr<ScopedServiceRequest>, std::unique_ptr<ScopedServiceRequest>, std::unique_ptr<ScopedServiceRequest>>
			{ std::move(scopedFreeCursorRequest) , std::move(scopedBlockInputRequest), std::move(scopedPauseRequest) };
		}

public:
	std::tuple<bool, std::string> showSaveDumpNameDialog(std::string dialogTitle, std::string defaultName)
	{
		PLOG_DEBUG << "beginning showSaveDumpNameDialog";
		auto scopedRequests = getScopedRequests();

		checkpointDumpNameDialog.setTitle(dialogTitle);
		std::get<std::string>(checkpointDumpNameDialog.currentReturnValue) = defaultName;
		checkpointDumpNameDialog.beginDialog();
		while (!GlobalKill::isKillSet() && checkpointDumpNameDialog.isDialogOpen()) { Sleep(10); }
		PLOG_DEBUG << "showCheckpointDumpNameDialog returning " << std::get<bool>(checkpointDumpNameDialog.currentReturnValue) << ", " << std::get<std::string>(checkpointDumpNameDialog.currentReturnValue);
		return checkpointDumpNameDialog.currentReturnValue;
	}

	bool showInjectionWarningDialog(std::string dialogTitle, std::string dialogText)
	{
		PLOG_DEBUG << "beginning showInjectionWarningDialog";
		auto scopedRequests = getScopedRequests();

		injectionWarningDialog.setTitle(dialogTitle);
		injectionWarningDialog.setText(dialogText);

		injectionWarningDialog.beginDialog();
		while (!GlobalKill::isKillSet() && injectionWarningDialog.isDialogOpen()) { Sleep(10); }
		PLOG_DEBUG << "injectionWarningDialog returning " << injectionWarningDialog.currentReturnValue;
		return injectionWarningDialog.currentReturnValue;
	}

	void showFailedOptionalCheatServices(std::shared_ptr<GUIServiceInfo> guiFailures)
	{
		PLOG_DEBUG << "beginning showFailedOptionalCheatServices";
		auto scopedRequests = getScopedRequests();

		failedOptionalCheatServicesDialog.setGUIFailures(guiFailures);
		failedOptionalCheatServicesDialog.beginDialog();
		while (!GlobalKill::isKillSet() && failedOptionalCheatServicesDialog.isDialogOpen()) { Sleep(10); }
		PLOG_DEBUG << "showFailedOptionalCheatServices returning";
		return;
	}

	ModalDialogRendererImpl(std::shared_ptr<RenderEvent> pRenderEvent, std::shared_ptr<ControlServiceContainer> controlServiceContainer)
		: mImGuiRenderCallbackHandle(pRenderEvent, [this](Vec2 screenSize) {onImGuiRenderEvent(screenSize); }),
		mFreeMCCCursorService(controlServiceContainer->freeMCCSCursorService),
		mBlockGameInputService(controlServiceContainer->blockGameInputService),
		mPauseGameService(controlServiceContainer->pauseGameService)
	{
	}
};


ModalDialogRenderer::ModalDialogRenderer(std::shared_ptr<RenderEvent> pRenderEvent, std::shared_ptr<ControlServiceContainer> controlServiceContainer, std::shared_ptr<ActionEvent> showFailsEvent, std::shared_ptr<GUIServiceInfo> guiFailures)
	: pimpl(std::make_unique<ModalDialogRendererImpl>(pRenderEvent, controlServiceContainer)),
	mGUIFailures(guiFailures),
	mShowGUIFailuresEventCallback(showFailsEvent, [this]() {showFailedOptionalCheatServices(); })
	{}

ModalDialogRenderer::~ModalDialogRenderer() = default;



std::tuple<bool, std::string> ModalDialogRenderer::showSaveDumpNameDialog(std::string dialogTitle, std::string defaultValue) { return pimpl->showSaveDumpNameDialog(dialogTitle, defaultValue); }
bool ModalDialogRenderer::showInjectionWarningDialog(std::string dialogTitle, std::string dialogText) { return pimpl->showInjectionWarningDialog(dialogTitle, dialogText); }
void ModalDialogRenderer::showFailedOptionalCheatServices() { return pimpl->showFailedOptionalCheatServices(mGUIFailures); }