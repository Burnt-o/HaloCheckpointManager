#include "TriggerOverlay.h"

#include <mutex>


#include "3Drender.h"
#include "TriggerData.h"
#include "PlayerData.h"
#include "CameraData.h"
#include <iostream>
#include <array>

bool g_triggerOverlayEnabled = false; // Control flag
bool useDebugCameraData2 = true;



//void DrawBox(const XMVECTOR(&vertexes)[8], const CameraData& cameraData, ImDrawList*& drawList, const ImU32& color);

extern "C" __declspec(dllexport) BOOL EnableTriggerOverlay()
{
	std::cout << "EnableTriggerOverlay beginning";
	if (!g_hcmInitialised || !g_hookEnabled)
	{
		std::cout << "Bailing on EnableTriggerOverlay";
		return FALSE;
	}

	g_triggerOverlayEnabled = true;
	return TRUE;

}

// HCMExternal calls this to turn off TriggerOverlay
extern "C" __declspec(dllexport) BOOL DisableTriggerOverlay()
{
	g_triggerOverlayEnabled = false;
	return TRUE;
}

// HCMExternal calls this to check if it's on or off
extern "C" __declspec(dllexport) BOOL IsTriggerOverlayEnabled()
{
	return g_triggerOverlayEnabled;
}


inline bool PointInsideTrigger(const XMVECTOR& entityPosition, const Trigger& trigger)
{
	if (!trigger.boundingBox.Contains(entityPosition)) return false; // Do axis aligned test first, it's much faster.
	return trigger.boundingOrientedBox.Contains(entityPosition);
}


UINT64 lastSeenTickcount = 0;
bool firstRenderTriggerOverlay = true;

int vertexToDebug = 0;
int delayCount = 0;
extern void RenderTriggerOverlay()
{
	TriggerDataOnFrameDebug();
	



	if (GetKeyState('Y') & 0x8000)
	{
		if (delayCount > 100)
		{
			vertexToDebug = (vertexToDebug + 1) % 8;
			std::cout << "debugging vertex: " << vertexToDebug << std::endl;
			delayCount = 0;
		}
		else
		{
			delayCount++;
		}
	}

	bool debug = firstRenderTriggerOverlay || GetKeyState('U') & 0x8000;
	if (debug) std::cout << "RenderTriggerOverlay beginning" << std::endl;
	if (g_triggerOverlayEnabled == false) return;

	// Check if camera data is good to read.
	if (!IsCameraDataLoaded())
	{
		g_triggerOverlayEnabled = false;
		return;
	}
	g_CameraDataMutex.lock();

	if (debug) std::cout << "RenderTriggerOverlay getting current tickcount" << std::endl;
	UINT64 currentTickcount = *(g_CameraData.CDI.pTickCounter);
	bool newTick = currentTickcount != lastSeenTickcount;
	lastSeenTickcount = currentTickcount;
	if (debug) std::cout << "RenderTriggerOverlay got current tickcount" << std::endl;

	if (useDebugCameraData2)
	{
		g_CameraData.UpdateCameraData();
	}

	bool testNewTick = (GetKeyState('U') & 0x8000);

	// Do new tick stuff, if it's a new tick
	if (newTick && testNewTick)
	{
		if (debug) std::cout << "RenderTriggerOverlay processing new tick" << std::endl;
		// Get player trigger-hit position
		g_PlayerData_mutex.lock();
		void* pPlayerEntity = g_PlayerData->pPlayerEntity;
		if (g_PlayerData->IsEntityDataValid(g_PlayerData->pPlayerEntity))
		{
			if (debug) std::cout << "RenderTriggerOverlay loading trigposition" << std::endl;
			XMVECTOR PlayerTrigPosition = XMLoadFloat3(g_PlayerData->pEntityTrigPosition(&pPlayerEntity));
			g_PlayerData_mutex.unlock();
			// Loop through EVERY trigger (even filtered ones), checking if the player is inside it, if so set the flag saying such.
			for (Trigger trig : g_AllTriggers)
			{
				trig.playerIsInsideMe = PointInsideTrigger(PlayerTrigPosition, trig);
			}
		}
		else // if we couldn't read the player data then we can't process the new tick.
		{
			g_PlayerData_mutex.unlock();
			newTick = false;
		}
	}


	std::vector<XMFLOAT3> loadedVertexes;
	loadedVertexes.resize((size_t)8);

	ImDrawList* drawList = ImGui::GetOverlayDrawList();

	// Now loop through non-filtered triggers, and render them to the screen.
	if (debug) std::cout << "Looping through all triggers to get vertexes, count: " << g_IncludedTriggers.size() << std::endl;
	if (g_IncludedTriggers.size() > 0)
	{
		for (Trigger trig : g_IncludedTriggers)
		{
			// Determine the colour the trigger should be. 
			// For now, we'll just make it yellow if player inside, red otherwise.

			if (debug) std::cout << "Loading color of trigger" << trig.triggerName << std::endl;
			bool test = trig.playerIsInsideMe;
			if (debug) std::cout << "aaaaaaaa" << std::endl;

			ImU32 triggerColor = test ? ImGui::ColorConvertFloat4ToU32(ImVec4(0.f, 1.f, 0.f, 0.05f)) : ImGui::ColorConvertFloat4ToU32(ImVec4(1.f, 0.f, 0.f, 0.05f));;

			// Get the vertexes
			if (debug) std::cout << "Loading trigger vertexes of trigger " << trig.triggerName << std::endl;
			for (int i = 0; i < 8; i++)
			{
				if (debug) std::cout << "Loading vertex, trig: " << trig.triggerName << ", vert: " << i << ": " << trig.vertexes[i].x << ", " << trig.vertexes[i].y << ", " << trig.vertexes[i].z << std::endl;
				loadedVertexes[i] = trig.vertexes[i];
				
			}


			if (GetKeyState('Q') & 0x8000)
			{

				// put circle on the pivot point of the trigger to help debug rotation
				XMFLOAT4 ah = WorldToScreen(loadedVertexes[4]);

				ImVec2 screenCoordsIm(ah.x, ah.y);
				drawList->AddCircleFilled(screenCoordsIm, 30.f, triggerColor); // for test


			}

			// Draw one face at a time
			std::vector<std::vector<XMFLOAT3>>faceVertexCollection
			{ 
				{loadedVertexes[0], loadedVertexes[1], loadedVertexes[2], loadedVertexes[3]},
				{loadedVertexes[0], loadedVertexes[1], loadedVertexes[5], loadedVertexes[4]},
				{loadedVertexes[1], loadedVertexes[2], loadedVertexes[6], loadedVertexes[5]},
				{loadedVertexes[2], loadedVertexes[3], loadedVertexes[7], loadedVertexes[6]},
				{loadedVertexes[3], loadedVertexes[0], loadedVertexes[4], loadedVertexes[7]},
				{loadedVertexes[4], loadedVertexes[5], loadedVertexes[6], loadedVertexes[7]},
			};


			for (int i = 0; i < 6; i++)
			{
				// Get the screen coordinates
				std::vector<XMFLOAT4> screenCoords = GetNGonScreenPositions(faceVertexCollection[i]);
				//std::vector<XMFLOAT4> screenCoords = { XMFLOAT4(0,0,0,0) };
				// If empty, skip drawing
				if (screenCoords.empty()) continue;

				// Convert to ImVec2 vector


				std::vector<ImVec2> screenCoordsIm;
				screenCoordsIm.resize(screenCoords.size());
				for (int i = 0; i < screenCoordsIm.size(); i++)
				{
					screenCoordsIm[i] = ImVec2(screenCoords[i].x, screenCoords[i].y);
				}

				// Draw it!
				if (debug) std::cout << "Drawing trigger " << trig.triggerName << std::endl
					<< "screenCoordsIm.size(): " << screenCoordsIm.size() << std::endl
					<< "first screenPos: " << screenCoords[0].x << ", " << screenCoords[0].y << ", W: " << screenCoords[0].w << std::endl;




				if (debug)
				{
						drawList->AddPolyline(screenCoordsIm.data(), (int)screenCoordsIm.size(), triggerColor, true, 5.0f);
					if (debug && vertexToDebug < screenCoordsIm.size()) drawList->AddCircleFilled(screenCoordsIm[vertexToDebug], 30.f, triggerColor); // for test
				}
				else
				{
					drawList->AddPolyline(screenCoordsIm.data(), (int)screenCoordsIm.size(), triggerColor, true, 3.0f);
					drawList->AddConvexPolyFilled(&screenCoordsIm[0], screenCoordsIm.size(), triggerColor);
				}
			}


			


			


		}
	}
	else
	{
		if (firstRenderTriggerOverlay) std::cout << "Error, g_IncludedTriggers was empty" << std::endl;
	}

	g_CameraDataMutex.unlock();
	if (firstRenderTriggerOverlay) std::cout << "Done rendering!";
	if (firstRenderTriggerOverlay) firstRenderTriggerOverlay = false;
}




