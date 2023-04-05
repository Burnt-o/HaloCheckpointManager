#include "includes.h"
//#include <Windows.h>
//#include "imgui/imgui.h"
//#include "imgui/imgui_impl_win32.h"
//#include "imgui/imgui_impl_dx11.h"
#include <vector>
#include <chrono>
#include <iostream>
#include "draw-text.h"
#include "messages.h"
#include <atlconv.h>
#include <locale>
#include <codecvt>
#include <string>

std::wstring g_HCMActiveCheatString = L"HCM persistent message string - initializing...";

// Forward declaration
extern "C" __declspec(dllexport) BOOL AddTemporaryMessage(const TCHAR * pChars);
extern "C" __declspec(dllexport) BOOL ChangeActiveMessage(const TCHAR * pChars);
void PrintTemporaryMessages();
void PrintActiveMessages();


const static float messageFontSize = 16;

std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

class TemporaryMessage
{
public:
	std::wstring messageText;
	std::chrono::steady_clock::time_point messageTimestamp;
};

std::mutex g_temporaryMessagesMutex;
std::vector<TemporaryMessage> g_temporaryMessages; // List of all current temporary messages
double const MESSAGE_EXPIRY_TIME = 3000; // Messages last on screen for 3 seconds
double const MESSAGE_VERTICAL_SEPERATION = 20; // 20 pixels between each message



extern "C" __declspec(dllexport) BOOL ChangeActiveMessage(const TCHAR * pChars)
{

	std::wstring inputText = converter.from_bytes(std::string(pChars));
	g_HCMActiveCheatString = inputText;
	return TRUE;
}




extern "C" __declspec(dllexport) BOOL AddTemporaryMessage(const TCHAR * pChars)
{
	std::wstring inputText = converter.from_bytes(std::string(pChars));
	auto curr_time = std::chrono::high_resolution_clock::now();

	TemporaryMessage tmessage;
	tmessage.messageText = inputText;
	tmessage.messageTimestamp = curr_time;

	g_temporaryMessagesMutex.lock();
	g_temporaryMessages.insert(g_temporaryMessages.begin(), tmessage); // Messages go to the FRONT of the g_temporaryMessages vector
	g_temporaryMessagesMutex.unlock();

	//std::cout << "Added to tempMessages map, count: " << g_temporaryMessages.size() << std::endl;

	return TRUE;

}







// Called every frame by hkPresent
void PrintTemporaryMessages()
{

	float x = 10;
	float y = 30;


	auto curr_time = std::chrono::high_resolution_clock::now(); // Get current time so we know if a message is old or not
	int numberOfValidMessages = 0;

	// Iterate over temporaryMessages, printing each one
	g_temporaryMessagesMutex.lock();
	for (std::vector<TemporaryMessage>::iterator it = g_temporaryMessages.begin(); it != g_temporaryMessages.end();)
	{
		std::wstring currentMessageText = it->messageText;
		std::chrono::steady_clock::time_point currentMessageTime = it->messageTimestamp;

		//calculate how long it's been (in milliseconds)
		double messageAge = std::chrono::duration<double, std::milli>(curr_time - currentMessageTime).count();

		// If message age larger than MESSAGE_EXPIRY_TIME, delete it from temporaryMessages & continue loop
		if (messageAge > MESSAGE_EXPIRY_TIME)
		{
			//std::cout << "Erasing old message." << std::endl;
			it = g_temporaryMessages.erase(it);
			continue;
		}

		// Calculate what the opacity of the message should be - messages that are nearly expired should progressively become transparent
		float opacityValue;
		double half_messageExpiryTime_ms = (MESSAGE_EXPIRY_TIME / 2); // Messages start to fade when they're halfway to expiring
		if (messageAge < half_messageExpiryTime_ms)
		{
			opacityValue = 1;
		}
		else
		{
			opacityValue = 1 - ((messageAge - half_messageExpiryTime_ms) / half_messageExpiryTime_ms); // Linear interpolation
		}

		// Calculate the vertical pixel position of the message based on how many have been printed (each one should be below the last)
		int placeY = y + (numberOfValidMessages * MESSAGE_VERTICAL_SEPERATION);

		// Send the message off to be drawn
		//renderer->drawOutlinedText(Vec2(x, y), currentMessageText, mainTextColor, messageFontSize);


		numberOfValidMessages++;
		it++;
	}
	g_temporaryMessagesMutex.unlock();
}

void PrintActiveMessages()
{

	float x = 10;
	float y = 10;
	//renderer->drawOutlinedText(Vec2(x, y), g_HCMActiveCheatString, mainTextColor, messageFontSize);


}


