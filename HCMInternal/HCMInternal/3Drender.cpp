#include "3Drender.h"

#include "includes.h"
#include "TriggerOverlay.h"
//#include <mutex>
#include "TriggerData.h"
#include <iostream>
#include <sstream>
#include <array>
#include <DirectXCollision.h>

using namespace DirectX;



std::ostream& operator << (std::ostream& os, const XMFLOAT3& s) {
	return (os << s.x << ", " << s.y << ", " << s.z);
}

std::ostream& operator << (std::ostream& os, const XMFLOAT4& s) {
	return (os << s.x << ", " << s.y << ", " << s.z << ", " << s.w);
}



//XMFLOAT4 WorldToScreen(const XMFLOAT3& worldPos);

bool g_debug = false;

void render3D()
{

	g_debug = GetKeyState('I') & 0x8000;

    if (!(g_triggerOverlayEnabled)) return;

    try
    {
        RenderTriggerOverlay();
    }
    catch (...)
    {
        std::cout << "Exception in RenderTriggerOverlay! Ex: " << what() << std::endl;
    }
}

// Returns true and outs the screen coordinate of the worldPos, unless the object is BEHIND the camera, in which case returns false.
XMFLOAT4 WorldToScreen(const XMFLOAT3& worldPos)
{
	//XMFLOAT4 worldPosF4;
	

    bool debugWorldToScreen = GetKeyState('L') & 0x8000;

    XMVECTOR screenVect = XMVector4Transform(XMVECTOR{ worldPos.x, worldPos.y, worldPos.z, 1.f }, XMLoadFloat4x4(&g_CameraData.viewProjectionMatrix));
	//XMVECTOR screenVect = DirectX::XMVector3Project(XMVECTOR{worldPos.x, worldPos.y, worldPos.z, 1.f }, 0, 0, g_screenWidth, g_screenHeight, 0.1f, 1.0f, XMLoadFloat4x4(&g_CameraData.projectionMatrix), XMLoadFloat4x4(&g_CameraData.viewMatrix), DirectX::XMMatrixIdentity());
	XMFLOAT4 screenOut;
	XMStoreFloat4(&screenOut, screenVect);

	float w = XMVectorGetW(screenVect);
	
	//if (w < 0.f) return screenOut; // Can't compute valid screenPos if w negative (vertex is behind camera)


		if (debugWorldToScreen) std::cout << "screenOut before invw: " << screenOut << std::endl;
		float invw = 1.f / w;
		screenOut.x *= invw;
		screenOut.y *= invw;



		if (debugWorldToScreen) std::cout << "screenOut before normalizing: " << screenOut << std::endl;

		// Normalise so the value is 0 thru 1, instead of -1 thru 1.
		screenOut.x = (screenOut.x + 1) / 2;
		screenOut.y = ((screenOut.y * -1.0f) + 1) / 2; // Y is the wrong way around so flip the sign

	




		if (debugWorldToScreen) std::cout << "screenOut before screen dimensions scaling: " << screenOut << std::endl;
		// Scale to screen dimensions
		screenOut.x *= g_screenWidth;
		screenOut.y *= g_screenHeight;



	if (debugWorldToScreen) std::cout << "final screenout: : " << screenOut << std::endl << std::endl;
    return screenOut;
}








struct Face
{
	// Constructor
	Face(XMFLOAT4 i1, XMFLOAT4 i2, XMFLOAT4 i3, XMFLOAT4 i4) : v1(i1), v2(i2), v3(i3), v4(i4) {}
	XMFLOAT4 v1, v2, v3, v4;
};



float ensurePositive = 0.99f;
// Uses difference in w values to calculate a vertex position that is guarenteed to be on screen
XMFLOAT3 getOnscreenVertexBetween(XMFLOAT3 onscreenWorldPos, XMFLOAT4 onscreenScreenPos, XMFLOAT3 offscreenWorldPos, XMFLOAT4 offscreenScreenpos)
{
	bool debug = (GetKeyState('P') & 0x8000);


	if (GetKeyState('1') & 0x8000)
	{
		ensurePositive += 0.001;
	}
	if (GetKeyState('2') & 0x8000)
	{
		ensurePositive -= 0.001;
	}
	if (GetKeyState('3') & 0x8000)
	{
		std::cout << "ensurePositive: " << ensurePositive << std::endl;
	}


	float ratioOfNewVert = abs(offscreenScreenpos.w) / ((onscreenScreenPos.w - offscreenScreenpos.w) * ensurePositive);
	if (debug) std::cout << "ratioOfNewVert: " << ratioOfNewVert << std::endl << std::endl;

	// argh FIX ME 
	XMVECTOR newVertex = XMVectorAdd(XMVectorScale(XMLoadFloat3(&onscreenWorldPos), ratioOfNewVert), XMVectorScale(XMLoadFloat3(&offscreenWorldPos), 1.f - ratioOfNewVert));
	XMFLOAT3 out;
	XMStoreFloat3(&out, newVertex);


	if (debug)
	{
		XMFLOAT4 newVertexScreenPos = WorldToScreen(out);

		std::cout << "offscreenWorldPos: " << offscreenWorldPos << std::endl;
		std::cout << "onscreenWorldPos: " << onscreenWorldPos << std::endl;
		std::cout << "newVertWorld: " << out << std::endl;
		std::cout << "offscreenScreenpos: " << offscreenScreenpos << std::endl;
		std::cout << "onscreenScreenPos: " << onscreenScreenPos << std::endl;
		std::cout << "newVertScreen: " << newVertexScreenPos << std::endl; 

	}




	return out;
}

bool verticeIsForward(XMFLOAT4 vert) { return vert.w > 0.f; }

// Supports input vertices of length 1 and 2 as well (ie point and line)
// Input vertices are assumed to be in the order they will be connected, from each element to the next (and from the last to the first)

bool firstGetNGonScreenPositions = true;
std::vector<XMFLOAT4> GetNGonScreenPositions(std::vector<XMFLOAT3> vertices)
{

	bool debug = firstGetNGonScreenPositions || GetKeyState('J') & 0x8000;
	if (firstGetNGonScreenPositions) firstGetNGonScreenPositions = false;

	std::vector<XMFLOAT4> firstPass;
	// Handle empty input, so empty output
	if (vertices.empty()) return firstPass;
	if (debug)	// Handle 1 vertex input (point)
	if (vertices.size() == 1)
	{
		firstPass.resize((size_t)1);
		firstPass.push_back(WorldToScreen(vertices.front()));
		if (firstPass.front().w < 0) 
		{ 
			firstPass.clear(); 
		}
		return firstPass;
	}

	// Rest of the functions assumes two or more vertices


	firstPass.resize(vertices.size()); // To start us off for this first loop
	if (debug) std::cout << "Getting WorldToScreen for vertices" << std::endl;
	bool AllVerticesForward = true;
	bool NoVerticesForward = true;
	for (int i = 0; i < vertices.size(); i++)
	{
		firstPass[i] = WorldToScreen(vertices[i]);
		if (debug) std::cout << "firstpass evaluating WorldToScreen for vert[" << i << "]: " << std::endl << vertices[i] << std::endl << firstPass[i] << std::endl;

		if (verticeIsForward(firstPass[i]))
		{
			NoVerticesForward = false;
		}
		else
		{
			AllVerticesForward = false;
		}
	}

	if (NoVerticesForward)
	{
		firstPass.clear();
		return firstPass;
	}
	if (AllVerticesForward)
	{
		return firstPass;
	}

	std::vector<XMFLOAT4> secondPass;
	secondPass.reserve(((size_t)vertices.size() * 2) - 1); 	 //maximum amount we'll need
	// Now loop through and check if any vertex has negative w (behind camera)
	// If so we need to construct adjacent replacement vertices, since worldToScreen can't output valid coords for negative w verts
	// Specifically we test if W negative, if so then we try to generate a new vertex between that one and it's two neighbours (but only if the neighbour is W positive)
	for (int i = 0; i < firstPass.size(); i++)
	{
		if (debug) std::cout << "secondpass evaluating WorldToScreen for vert[" << i << "]: " << std::endl << vertices[i] << std::endl;
		
		if (verticeIsForward(firstPass[i]))
		{
			secondPass.push_back(firstPass[i]);
		}
		else
		{
			int indexOfPrev = (i - 1) % firstPass.size();
			int indexOfNext = (i + 1) % firstPass.size();

			if (verticeIsForward(firstPass[indexOfPrev]))
			{
				XMFLOAT3 newVertex = getOnscreenVertexBetween(vertices[indexOfPrev], firstPass[indexOfPrev], vertices[i], firstPass[i]);
				XMFLOAT4 newVertexScreenPos = WorldToScreen(newVertex);
				if (!verticeIsForward(newVertexScreenPos))
				{
					if (GetKeyState('T') & 0x8000)	std::cout << "getOnscreenVertexBetween prev failed" << std::endl;


					firstPass.clear();
					return firstPass;
				}
				//assert(verticeIsForward(newVertexScreenPos));
				secondPass.push_back(newVertexScreenPos);
			}

			if (verticeIsForward(firstPass[indexOfNext]))
			{
				XMFLOAT3 newVertex = getOnscreenVertexBetween(vertices[indexOfNext], firstPass[indexOfNext], vertices[i], firstPass[i]);
				XMFLOAT4 newVertexScreenPos = WorldToScreen(newVertex);
				if (!verticeIsForward(newVertexScreenPos))
				{
					if (GetKeyState('T') & 0x8000)	std::cout << "getOnscreenVertexBetween next failed" << std::endl;
					firstPass.clear();
					return firstPass;
				}
				//assert(verticeIsForward(newVertexScreenPos));
				secondPass.push_back(newVertexScreenPos);
			}

		}

	}

	return secondPass;

	


}
