#include "pch.h"
#include "HCMInternalGUI.h"
#include "GlobalKill.h"
#include "GUISimpleButton.h"
#include "GameStateHook.h"
#define addTooltip(x) if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) ImGui::SetTooltip(x)

bool HCMInternalGUI::m_WindowOpen = true;
HCMInternalGUI* HCMInternalGUI::instance = nullptr;


ImVec2 minimumWindowSize{ 500, 500 };

HCMInternalGUI::~HCMInternalGUI()
{
	std::scoped_lock<std::mutex> lock(mDestructionGuard); // onPresentHookCallback also locks this
	if (mImGuiRenderCallbackHandle && pImGuiRenderEvent)
	{
		pImGuiRenderEvent.remove(mImGuiRenderCallbackHandle);
		mImGuiRenderCallbackHandle = {};
	}

	if (mWindowResizeCallbackHandle && pWindowResizeEvent)
	{
		pWindowResizeEvent.remove(mWindowResizeCallbackHandle);
		mWindowResizeCallbackHandle = {};
	}

	if (mGameStateChangeCallbackHandle && pGameStateChangeEvent)
	{
		pGameStateChangeEvent.remove(mGameStateChangeCallbackHandle);
		mGameStateChangeCallbackHandle = {};
	}

	instance = nullptr;
}


void HCMInternalGUI::initializeHCMInternalGUI()
{
	ImGui::SetNextWindowCollapsed(false);
	windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

	auto fullScreenSize = D3D11Hook::getScreenSize();
	PLOG_VERBOSE << "actual screen size: " << fullScreenSize.x << ", " << fullScreenSize.y;
	if (fullScreenSize.x > minimumWindowSize.x && fullScreenSize.y > minimumWindowSize.y) // check for really small screens or getScreenSize returning a junk value, then we just use default 500, 500
	{
		// adjust vertical window height to be 2/3rds of screen
		minimumWindowSize.y = fullScreenSize.y / 3 * 2;
	}

	std::string currentHaloLevel = GameStateHook::getCurrentHaloLevel().data();
	onGameStateChange(GameStateHook::getCurrentGameState(), currentHaloLevel);

}


void HCMInternalGUI::onWindowResizeEvent(ImVec2 newScreenSize)
{

	PLOG_VERBOSE << "new actual screen size: " << newScreenSize.x << ", " << newScreenSize.y;
	if (newScreenSize.x > minimumWindowSize.x && newScreenSize.y > minimumWindowSize.y) // check for really small screens or getScreenSize returning a junk value, then we just use default 500, 500
	{
		// adjust vertical window height to be 2/3rds of screen
		minimumWindowSize.y = newScreenSize.y / 3 * 2;
	}
}

void HCMInternalGUI::onImGuiRenderEvent()
{
	std::scoped_lock<std::mutex> lock(instance->mDestructionGuard);

	//PLOG_VERBOSE << "CEERGUI::onImGuiRenderCallback()";
	if (!instance->m_HCMInternalGUIinitialized)
	{
		PLOG_INFO << "Initializing HCMInternalGUI";
		try
		{
			instance->initializeHCMInternalGUI();
			instance->m_HCMInternalGUIinitialized = true;
		}
		catch (HCMInitException& ex)
		{
			PLOG_FATAL << "Failed to initialize HCMInternalGUI, info: " << std::endl
				<< ex.what() << std::endl
				<< "HCM will now automatically close down";
			GlobalKill::killMe();
			return;
		}
	}


	instance->renderErrorDialog();
	instance->primaryRender();

}


static bool errorDialogShowing = false;
void HCMInternalGUI::renderErrorDialog()
{
	if (errorsToDisplay.empty())
	{
		return;
	}

	if (!errorDialogShowing)
	{
		ImGui::OpenPopup("Error!");
		errorDialogShowing = true;
	}

	ImVec2 size = ImVec2(300, 0);
	ImVec2 pos = ImVec2(ImGuiManager::getScreenSize() / 2.f) - (size / 2.f);

	ImGui::SetNextWindowSize(size);
	ImGui::SetNextWindowPos(pos);

	if (ImGui::BeginPopupModal("Error!", NULL, ImGuiWindowFlags_NoResize))
	{

		ImGui::TextWrapped("HCM encountered an error!");
		ImGui::TextWrapped(errorsToDisplay.front().what().c_str());
		ImGui::TextWrapped("");
		ImGui::TextWrapped("Nerdy info for Burnt: ");

		ImGui::BeginChild("trace", ImVec2(ImGui::GetWindowSize().x - 15, 150), NULL);
		ImGui::TextWrapped("The error occured here:");
		ImGui::TextWrapped(errorsToDisplay.front().source().c_str());
		ImGui::TextWrapped("");
		ImGui::TextWrapped("Full stack trace:");
		ImGui::TextWrapped(errorsToDisplay.front().trace().c_str());
		ImGui::EndChild();

		if (ImGui::Button("OK"))
		{
			errorDialogShowing = false;
			errorsToDisplay.erase(errorsToDisplay.begin());
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		if (ImGui::Button("Copy to clipboard"))
		{
			std::string copyText = errorsToDisplay.front().what() + "\n\n" + errorsToDisplay.front().source() + "\n\n" + errorsToDisplay.front().trace();
			ImGui::SetClipboardText(copyText.c_str());
		}
		ImGui::EndPopup();
	}

}

const std::set<GameState> AllGames{GameState::Halo1, GameState::Halo2, GameState::Halo3, GameState::Halo3ODST, GameState::HaloReach, GameState::Halo4};

std::vector<std::shared_ptr<GUIElementBase>> HCMInternalGUI::allGUIElements
{
	std::make_shared<GUISimpleButton>(AllGames, "Force Checkpoint", OptionsState::forceCheckpointEvent),
		std::make_shared<GUISimpleButton>(AllGames, "Force Revert", OptionsState::forceRevertEvent),
		std::make_shared<GUISimpleButton>(std::set<GameState>{GameState::Halo2, GameState::Halo3, GameState::Halo3ODST, GameState::HaloReach, GameState::Halo4}, "Force Double Revert", OptionsState::forceDoubleRevertEvent),
		std::make_shared<GUISimpleButton>(AllGames, "Inject Checkpoint", OptionsState::injectCheckpointEvent), // won't be simple buttons later
		std::make_shared<GUISimpleButton>(AllGames, "Dump Checkpoint", OptionsState::dumpCheckpointEvent), // won't be simple buttons later

};


void HCMInternalGUI::onGameStateChange(GameState newGameState, std::string newLevel)
{
	instance->currentlyRenderingGUIElements.clear();
	for (auto element : allGUIElements)
	{
		if (element.get()->getSupportedGames().contains(newGameState))
		{
			instance->currentlyRenderingGUIElements.emplace_back(element);
		}
	}
}

void HCMInternalGUI::primaryRender()
{
	// Calculate height of everything
	int totalContentHeight = 0;
	for (auto& element : currentlyRenderingGUIElements)
	{
		totalContentHeight += element->getCurrentHeight();
	}

	totalContentHeight += 40; // some padding for debug

	mWindowSize.y = (minimumWindowSize.y > totalContentHeight ? totalContentHeight : minimumWindowSize.y);



	ImGui::SetNextWindowSize(mWindowSize);
	ImGui::SetNextWindowPos(mWindowPos);

	if (m_WindowOpen)
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.00f, 0.80f, 0.40f, 1.00f));
	else
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.00f, 0.60f, 0.25f, 1.00f));


	m_WindowOpen = ImGui::Begin(m_WindowOpen ? "Halo Checkpoint Manager###HCM" : "HCM###HCM", nullptr, windowFlags); // Create window

	ImGui::PopStyleColor();

	if (m_WindowOpen)  //only bother rendering children if it's not collapsed
	{
		for (auto& element : currentlyRenderingGUIElements)
		{
			element.get()->render();
		}
	}

	ImGui::End(); // end main window

}