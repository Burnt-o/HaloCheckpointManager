#include "pch.h"
#include "MessagesGUI.h"
#include "HCMInternalGUI.h"

MessagesGUI* MessagesGUI::instance = nullptr;

MessagesGUI::~MessagesGUI()
{
	std::scoped_lock<std::mutex> lock(mDestructionGuard); 
	if (mCallbackHandle && pImGuiRenderEvent)
	{
		pImGuiRenderEvent.remove(mCallbackHandle);
		mCallbackHandle = {};
	}

	instance = nullptr;
}

void MessagesGUI::onImGuiRenderEvent()
{
	std::scoped_lock<std::mutex> lock(instance->mDestructionGuard);
	instance->iterateMessages();
}



// in milliseconds
constexpr float messageExpiryTimeout = 12000.f;
constexpr float messageFullOpacityTime = 8000.f;
constexpr float messageFadeTime = messageExpiryTimeout - messageFullOpacityTime;
void MessagesGUI::iterateMessages()
{
	if (messages.empty()) return;

	// Calculate age of each message
	auto currentTime = std::chrono::high_resolution_clock::now();
	for (auto& message : messages)
	{
		message.messageAge = std::chrono::duration<float, std::milli>(currentTime - message.timeStamp).count();
	}

	// remove old messages from the list

	std::erase_if(messages, [&currentTime](const auto& message)
		{
			bool erase = message.messageAge > messageExpiryTimeout;
			if (erase) PLOG_VERBOSE << "erasing expired message";
			return erase;
		});

	// draw remaining messages
	ImVec2 mesPosWhenOptionsOpen(10, HCMInternalGUI::getHCMInternalWindowHeight() + 15.f + 15.f);
	constexpr ImVec2 mesPosWhenOptionsClosed(10.f, 35.f + 15.f);

	ImVec2 messagePosition = HCMInternalGUI::isWindowOpen() ? mesPosWhenOptionsOpen : mesPosWhenOptionsClosed;
	for (auto& message : messages)
	{
		drawMessage(message, messagePosition);
		// update messagePosition based on previous messages linecount
		messagePosition = ImVec2(messagePosition.x, messagePosition.y + (message.lineCount * 17.f) + 5.f);
	}
}

// returns vertical pixel height of message
void MessagesGUI::drawMessage(const temporaryMessage& message, const ImVec2& position)
{
	// Calculate opacity using message age
	float opacity = 1.f; 
	float delta = (message.messageAge - messageFullOpacityTime);
	if (delta > 0)
	{
		opacity = (messageFadeTime - delta) / messageFadeTime;
	}

	constexpr int outlineWidth = 1;
	ImColor textColor(1.f, 0.2f, 0.0f, opacity); // todo: mess with colours. maybe increase font size?
	ImColor outlineColor(0.3f, 0.f, 0.3f, opacity);

	// outline
	ImGui::GetForegroundDrawList()->AddText(ImVec2(position.x, position.y - outlineWidth), outlineColor, message.message.c_str());
	ImGui::GetForegroundDrawList()->AddText(ImVec2(position.x, position.y + outlineWidth), outlineColor, message.message.c_str());
	ImGui::GetForegroundDrawList()->AddText(ImVec2(position.x - outlineWidth, position.y), outlineColor, message.message.c_str());
	ImGui::GetForegroundDrawList()->AddText(ImVec2(position.x + outlineWidth, position.y), outlineColor, message.message.c_str());

	// main
	ImGui::GetForegroundDrawList()->AddText(ImVec2(position.x, position.y), textColor, message.message.c_str());

}

//https://stackoverflow.com/a/27757111
// Prefers inserting new lines at spaces instead of splitting words
std::string insertNewLines(const std::string& in, const size_t every_n, int& outLineCount)
{
	PLOG_VERBOSE << "insertNewLines";

	if (in.size() < every_n)
	{
		outLineCount = std::count(in.begin(), in.end(), '\n');
		outLineCount++;
		return in;
	}

	std::string temp;
	std::vector<std::string> lines;
	for (std::string::size_type i = 0; i < in.size(); i++)
	{
		//PLOG_VERBOSE << (int)in[i];
		if (in[i] == '\n')
		{
			//PLOG_DEBUG << "Found preexisting endline";
			lines.push_back(temp);
			temp.clear();
		}
		else
		{
			if (temp.size() == every_n)
			{
				auto lastSpace = temp.find_last_of(' ');
				if (lastSpace != std::string::npos && temp.length() - lastSpace < 20) // check the last space isn't too far back
				{
					lines.push_back(temp.substr(0, lastSpace));
					temp.erase(0, lastSpace);
				}
				else // space was too far back
				{
					lines.push_back(temp);
					temp.clear();
				}
			}
			temp.push_back(in[i]);
		}

	}

	if (temp.size() > 0) lines.push_back(temp); // put the last line in

	temp.clear();
	for (auto& line : lines)
	{
		temp.append(line);
		temp.push_back('\n');
	}

	// get rid of extra last newLineChar at the end
	temp.pop_back();
	outLineCount = lines.size();
	PLOG_VERBOSE << "lineCount: " << outLineCount;
	return temp;


}

std::mutex addMessageMutex;
void MessagesGUI::addMessage(std::string message)
{
	std::scoped_lock<std::mutex> lock(addMessageMutex);
	// split message to multiple lines if necessary
	int lineCount;
	message = insertNewLines(message, 150, lineCount);
	PLOG_DEBUG << "Message added with linecount: " << lineCount;
	instance->messages.emplace_back(temporaryMessage{message, std::chrono::high_resolution_clock::now(), lineCount});

}

