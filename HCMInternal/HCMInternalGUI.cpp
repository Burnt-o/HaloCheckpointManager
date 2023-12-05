#include "pch.h"
#include "HCMInternalGUI.h"
#include "GlobalKill.h"
#include "GUISimpleButton.h"
#include "MCCStateHook.h"
#include "imgui.h"
#define addTooltip(x) if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) ImGui::SetTooltip(x)



ImVec2 minimumWindowSize{ 500, 500 };

void HCMInternalGUI::adjustWindowPosition()
{
	// aspect ratio adjustment to main gui position so that it appears over the top of the games ammo counter ui
	constexpr float ratio16to9 = 16.f / 9.f;
	float currentRatio = mFullScreenSize.x / mFullScreenSize.y;

	if (currentRatio > ratio16to9) // need to move HCM window pos to the right
	{
		// if the aspect radio were 16:9, how wide would the screen be?
		float widthIf16to9 = mFullScreenSize.y * ratio16to9;
		float currentExtraWidth = mFullScreenSize.x - widthIf16to9;
		PLOG_DEBUG << "currentExtraWidth: " << currentExtraWidth;

		constexpr float magicAdjustmentFactor = 0.6;


		float delta = currentExtraWidth * 0.5;// halve it since each side gets half the extra width
		delta = delta * magicAdjustmentFactor; // idk

		mWindowPos = SimpleMath::Vector2(10 + delta, 25);
	}
	else
	{
		mWindowPos = SimpleMath::Vector2(10, 25);
	}
}


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

	adjustWindowPosition();

}



void HCMInternalGUI::onImGuiRenderEvent(SimpleMath::Vector2 screenSize)
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

		adjustWindowPosition();
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

	SimpleMath::Vector2 size = SimpleMath::Vector2(300, 0);
	SimpleMath::Vector2 pos = SimpleMath::Vector2(mFullScreenSize / 2.f) - (size / 2.f);

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

	totalContentHeight += 25; // some padding for.. actually I'm not sure why
	totalContentHeight += GUIFrameHeightWithSpacing; // for header


	mWindowSize.y = (minimumWindowSize.y > totalContentHeight ? totalContentHeight : minimumWindowSize.y);

	// check for free-camera-hide-watermark (only if window is collapsed)
	if (!m_WindowOpen && mSettings->freeCameraToggle->GetValue() && mSettings->freeCameraHideWatermark->GetValue())
	{
		// do nothing
		LOG_ONCE(PLOG_DEBUG << "skipping rendering collapsed window due to freeCameraHideWatermark setting");
	}
	else
	{
		// render
		ImGui::SetNextWindowSize(mWindowSize);
		ImGui::SetNextWindowPos(mWindowPos);

		if (m_WindowOpen)
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.00f, 0.80f, 0.40f, 1.00f));
		else
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.00f, 0.60f, 0.25f, 1.00f));



		ImGuiWindowFlags blockInputsIfNoCursor = *mIsCursorShowing == true ? ImGuiWindowFlags_None : ImGuiWindowFlags_NoInputs & ImGuiWindowFlags_NoMouseInputs;

		ImGui::SetNextWindowCollapsed(!m_WindowOpen);
		m_WindowOpen = ImGui::Begin(m_WindowOpen ? "Halo Checkpoint Manager###HCM" : "HCM###HCM", nullptr, windowFlags | blockInputsIfNoCursor); // Create window. Returns false when window collapsed

		ImGui::PopStyleColor();

		if (m_WindowOpen)  //only bother rendering children if it's not collapsed
		{	
			mGUIMCCState.render(); // render header

			// Tell all the current-games GUI elements to render themselves
			for (auto& element : *p_currentGameGUIElements)
			{
				element->render(*mHotkeyRenderer.get());
			}
		}

		ImGui::End(); // end main window

	}

	// do stuff if window just opened
	if (m_WindowOpen && !m_windowOpenLastFrame)
	{
		onWindowJustOpened();
	}
	// do stuff if window just closed
	else if (!m_WindowOpen && m_windowOpenLastFrame)
	{
		onWindowJustClosed();
	}

	m_windowOpenLastFrame = m_WindowOpen;

	LOG_ONCE(PLOG_VERBOSE << "unlocking currentGameGUIElementsMutex");

	//// little colour picker for gui stuff
	//ImGuiStyle* style = &ImGui::GetStyle();
	//ImVec4 newCol = style->Colors[ImGuiCol_ChildBg];
	//if (ImGui::ColorPicker4("col", &newCol.x))
	//{
	//	style->Colors[ImGuiCol_ChildBg] = newCol;
	//}

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


void HCMInternalGUI::onGUIShowingBlocksGameInputChanged(bool& newval)
{
	if (newval && m_WindowOpen && mControlServices->blockGameInputService.has_value() && !blockGameInputRequest)
	{
		blockGameInputRequest = mControlServices->blockGameInputService.value()->scopedRequest(nameof(HCMInternalGUI));
	}
	else if (!newval && m_WindowOpen && blockGameInputRequest)
	{
		blockGameInputRequest.reset();
	}
}

void HCMInternalGUI::onGUIShowingPausesGameChanged(bool& newval)
{
	if (newval && m_WindowOpen && mControlServices->pauseGameService.has_value() && !pauseGameRequest)
	{
		pauseGameRequest = mControlServices->pauseGameService.value()->scopedRequest(nameof(HCMInternalGUI));
	}
	else if (!newval && m_WindowOpen && pauseGameRequest)
	{
		pauseGameRequest.reset();
	}
}

void HCMInternalGUI::onWindowJustOpened()
{
	mSettings->GUIWindowOpen->GetValueDisplay() = m_WindowOpen;
	mSettings->GUIWindowOpen->UpdateValueWithInput();

	if (mSettings->GUIShowingFreesCursor->GetValue() && mControlServices->freeMCCSCursorService.has_value())
		freeCursorRequest = mControlServices->freeMCCSCursorService.value()->scopedRequest(nameof(HCMInternalGUI));

	if (mSettings->GUIShowingBlocksInput->GetValue() && mControlServices->blockGameInputService.has_value())
		blockGameInputRequest = mControlServices->blockGameInputService.value()->scopedRequest(nameof(HCMInternalGUI));

	if (mSettings->GUIShowingPausesGame->GetValue() && mControlServices->pauseGameService.has_value())
		pauseGameRequest = mControlServices->pauseGameService.value()->scopedRequest(nameof(HCMInternalGUI));
}

void HCMInternalGUI::onWindowJustClosed()
{
	mSettings->GUIWindowOpen->GetValueDisplay() = m_WindowOpen;
	mSettings->GUIWindowOpen->UpdateValueWithInput();

	if (freeCursorRequest)
		freeCursorRequest.reset();

	if (blockGameInputRequest)
		blockGameInputRequest.reset();

	if (pauseGameRequest)
		pauseGameRequest.reset();
}

