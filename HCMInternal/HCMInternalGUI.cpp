#include "pch.h"
#include "HCMInternalGUI.h"
#include "GlobalKill.h"
#include "GUISimpleButton.h"
#include "MCCStateHook.h"
#include "imgui.h"
#define addTooltip(x) if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) ImGui::SetTooltip(x)



ImVec2 minimumWindowSize{ 500, 500 };




void HCMInternalGUI::initializeHCMInternalGUI()
{
	ImGui::SetNextWindowCollapsed(false);
	windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

	PLOG_VERBOSE << "actual screen size: " << mFullScreenSize.x << ", " << mFullScreenSize.y;
	if (mFullScreenSize.x > minimumWindowSize.x && mFullScreenSize.y > minimumWindowSize.y) // check for really small screens or getScreenSize returning a junk value, then we just use default 500, 500
	{
		// adjust vertical window height to be 2/3rds of screen
		minimumWindowSize.y = mFullScreenSize.y / 3 * 2;
	}

	//std::string currentHaloLevel = MCCStateHook::getCurrentHaloLevel().data();
	//onGameStateChange(MCCStateHook::getCurrentGameState(), currentHaloLevel);

}



void HCMInternalGUI::onImGuiRenderEvent(Vec2 screenSize)
{
	std::unique_lock<std::mutex> lock(mDestructionGuard);

	if (mFullScreenSize != screenSize) // recalculate our gui canvas size
	{
		mFullScreenSize = screenSize;
		if (screenSize.x > minimumWindowSize.x && screenSize.y > minimumWindowSize.y) // check for really small screens or getScreenSize returning a junk value, then we just use default 500, 500
		{
			// adjust vertical window height to be 2/3rds of screen
			minimumWindowSize.y = screenSize.y / 3 * 2;
		}
	}


	if (!m_HCMInternalGUIinitialized)
	{
		onGameStateChange(mccStateHook->getCurrentMCCState());

		PLOG_INFO << "Initializing HCMInternalGUI";
		try
		{
			initializeHCMInternalGUI();
			m_HCMInternalGUIinitialized = true;
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


	renderErrorDialog();
	primaryRender();

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
	ImVec2 pos = ImVec2(mFullScreenSize / 2.f) - (size / 2.f);

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

//https://www.modernescpp.com/index.php/dealing-with-mutation-locking/


void HCMInternalGUI::onGameStateChange(const MCCState& newState)
{

	LOG_ONCE_CAPTURE(PLOG_VERBOSE << "onGameStateChange locking currentGameGUIElementsMutex @ 0x" << std::hex << pMutex, pMutex = &currentGameGUIElementsMutex);
	std::unique_lock<std::mutex> lock(currentGameGUIElementsMutex);

	p_currentGameGUIElements = &mGUIStore->getTopLevelGUIElements(newState.currentGameState);
	LOG_ONCE(PLOG_VERBOSE << "unlocking currentGameGUIElementsMutex");
}



std::unique_ptr<ScopedServiceRequest> freeCursorRequest;
void HCMInternalGUI::primaryRender()
{
	LOG_ONCE_CAPTURE(PLOG_VERBOSE << "onGameStateChange locking currentGameGUIElementsMutex @ 0x" << std::hex << pMutex, pMutex = &currentGameGUIElementsMutex);
	std::unique_lock<std::mutex> lock(currentGameGUIElementsMutex);
	// Calculate height of everything
	int totalContentHeight = 0;
	for (auto& element : *p_currentGameGUIElements)
	{
		totalContentHeight += element->getCurrentHeight();
	}

	totalContentHeight += 40; // some padding for debug
	totalContentHeight += 20; // for header

	mWindowSize.y = (minimumWindowSize.y > totalContentHeight ? totalContentHeight : minimumWindowSize.y);



	ImGui::SetNextWindowSize(mWindowSize);
	ImGui::SetNextWindowPos(mWindowPos);

	if (m_WindowOpen)
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.00f, 0.80f, 0.40f, 1.00f));
	else
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.00f, 0.60f, 0.25f, 1.00f));

	bool windowOpenLastFrame = m_WindowOpen;
	m_WindowOpen = ImGui::Begin(m_WindowOpen ? "Halo Checkpoint Manager###HCM" : "HCM###HCM", nullptr, windowFlags); // Create window

	ImGui::PopStyleColor();

	if (m_WindowOpen)  //only bother rendering children if it's not collapsed
	{
		mGUIMCCState.render(); // render header

		for (auto& element : *p_currentGameGUIElements)
		{
			element->render(*mHotkeyRenderer.get());
		}
	}

	ImGui::End(); // end main window


	// do stuff if window just opened
	if (m_WindowOpen && !windowOpenLastFrame)
	{
		if (mSettings->GUIShowingFreesCursor->GetValue() && mControlServices->freeMCCSCursorService.has_value())
			freeCursorRequest = mControlServices->freeMCCSCursorService.value()->scopedRequest(nameof(HCMInternalGUI));
	}
	// do stuff if window just closed
	else if (!m_WindowOpen && windowOpenLastFrame)
	{
		if (freeCursorRequest)
			freeCursorRequest.reset();
	}

	LOG_ONCE(PLOG_VERBOSE << "unlocking currentGameGUIElementsMutex");

}

void HCMInternalGUI::onGUIShowingFreesMCCCursorChanged(bool& newval)
{
	if (newval && m_WindowOpen && mControlServices->freeMCCSCursorService.has_value() && !freeCursorRequest)
	{
		freeCursorRequest = mControlServices->freeMCCSCursorService.value()->scopedRequest(nameof(HCMInternalGUI));
	}
	else if (!newval && m_WindowOpen && freeCursorRequest)
	{
		freeCursorRequest.reset();
	}
}