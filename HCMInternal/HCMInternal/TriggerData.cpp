
#include "TriggerData.h"
//#include "hookd3d.h"
#include <iostream>
#include<iterator>
#include "includes.h"
#include "detours.h"


#pragma comment(lib, "Detours.lib") 
using namespace DirectX;


TriggerDataInfo g_TriggerDataInfo;
std::mutex g_TriggerDataMutex;

UINT32 g_TriggerTestValue = 42069;
int g_TriggerTestValueOffset = 0;



std::vector<Trigger> g_AllTriggers;
std::vector<Trigger> g_IncludedTriggers; // g_AllTriggers after being filtered by triggername

typedef  BOOL(* TriggerHitTest ) (int16_t triggerIndex, int32_t entityDatum);
BOOL  hkTriggerHitTest(int16_t triggerIndex, int32_t entityDatum);
TriggerHitTest g_HitTriggerTestOriginal;



extern "C" __declspec(dllexport) BOOL IsTriggerDataLoaded();
extern "C" __declspec(dllexport) BOOL LoadTriggerData(TriggerDataInfo tdi);
BOOL IsValidTrigger(uintptr_t pTrigger);


float adjustRotationx = 0.0f;
float adjustRotationy = 0.0f;
float adjustRotationz = 0.0f;


std::ostream& operator << (std::ostream& os, const XMVECTOR& s) {
	XMFLOAT4 sf;
	XMStoreFloat4(&sf, s);
	return (os << sf.x << ", " << sf.y << ", " << sf.z << ", " << sf.w);
}

std::ostream& operator << (std::ostream& os, const XMMATRIX& s) {
;
	XMVECTOR sf1, sf2, sf3, sf4;
	sf1 = s.r[0];
	sf2 = s.r[1];
	sf3 = s.r[2];
	sf4 = s.r[3];
	return (os << sf1 << std::endl << sf2 << std::endl << sf3 << std::endl << sf4 << std::endl);
}

void TriggerDataOnFrameDebug()
{
	if (GetKeyState('Z') & 0x8000)
	{
		LoadTriggerData(g_TriggerDataInfo);
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(200ms);
	}

	if (GetKeyState('M') & 0x8000)
	{
		if (GetKeyState('F') & 0x8000)
		adjustRotationx += 0.01;

		if (GetKeyState('G') & 0x8000)
			adjustRotationy += 0.01;

		if (GetKeyState('H') & 0x8000)
			adjustRotationz += 0.01;
	}

	if (GetKeyState('N') & 0x8000)
	{
		if (GetKeyState('F') & 0x8000)
			adjustRotationx -= 0.01;

		if (GetKeyState('G') & 0x8000)
			adjustRotationy -= 0.01;

		if (GetKeyState('H') & 0x8000)
			adjustRotationz -= 0.01;
	}

	if (GetKeyState('B') & 0x8000)
	{
		std::cout << adjustRotationx << std::endl;
		std::cout << adjustRotationy << std::endl;
		std::cout << adjustRotationz << std::endl << std::endl;
	}

}


BOOL IsTriggerDataLoaded()
{

	void* pFirstTrigger = g_TriggerDataInfo.pFirstTrigger;
	return IsValidTrigger((uintptr_t)pFirstTrigger);
}

BOOL IsValidTrigger(uintptr_t pTrigger)
{

	
	if (pTrigger == (uintptr_t)NULL || IsBadReadPtr((void*)pTrigger, 4))
	{
		return FALSE;
	}

	UINT32* pTriggerTestValue = (UINT32*)(pTrigger + g_TriggerTestValueOffset);
	if (IsBadReadPtr(pTriggerTestValue, 4))
	{
		return FALSE;
	}
	UINT32 TestValueObserved = *pTriggerTestValue;
	UINT32 TestValueExpected = g_TriggerTestValue;

	if (TestValueObserved == TestValueExpected)
	{
		return true;
	}
	else
	{
		std::cout << "IsValidTrigger failed, TestValueObserved: " << TestValueObserved << ", pointer: " << pTrigger << std::endl;
		return false;
	}

}


XMVECTOR makeQuaternionFromDirection(const XMVECTOR& direction, const XMVECTOR& up)
{
	XMFLOAT3 up3;
	XMStoreFloat3(&up3, up);
	XMVECTOR upFixed = XMVectorSet(up3.x, up3.y, up3.z, 0.f);

	XMFLOAT3 dir3;
	XMStoreFloat3(&dir3, direction);
	XMVECTOR dirFixed = XMVectorSet(dir3.x, dir3.z, dir3.y, 0.f);

	XMVECTOR quarternion = XMVector3Cross(upFixed, dirFixed); // order matters here, cross product is not cumulative

	float upLengthSq = XMVectorGetX(XMVector3LengthSq(upFixed));
	float directionLengthSq = XMVectorGetX(XMVector3LengthSq(dirFixed));

	float w = sqrt(upLengthSq * directionLengthSq) + XMVectorGetX(XMVector3Dot(dirFixed, upFixed));
	XMVectorSetW(quarternion, w);

	quarternion = XMQuaternionNormalize(quarternion);
	return quarternion;
}


	//XMMATRIX makeRotationDir(const XMVECTOR& direction, const XMVECTOR& up)
	//{
	//	XMVECTOR xaxis = XMVector3Cross(up, direction); // Vec3::Cross(up, direction);
	//	xaxis = XMVector3Normalize(xaxis);

	//	XMVECTOR yaxis = XMVector3Cross(direction, xaxis); // Vec3::Cross(up, direction);
	//	yaxis = XMVector3Normalize(yaxis);

	//	XMMATRIX matrix (xaxis, yaxis, direction, g_XMIdentityR3);
	//	matrix = XMMatrixTranspose(matrix);

	//	/*column1.x = xaxis.x;
	//	column1.y = yaxis.x;
	//	column1.z = direction.x;

	//	column2.x = xaxis.y;
	//	column2.y = yaxis.y;
	//	column2.z = direction.y;

	//	column3.x = xaxis.z;
	//	column3.y = yaxis.z;
	//	column3.z = direction.z;*/
	//}



BOOL LoadTriggerData(TriggerDataInfo tdi)
{

	g_TriggerDataInfo = tdi;
	g_HitTriggerTestOriginal = (TriggerHitTest)tdi.p_oTriggerHitTest;

	if (!IsTriggerDataLoaded)
	{
		std::cout << "IsTriggerDataLoaded failed, pFirstTrigger: " << tdi.pFirstTrigger << std::endl;
		return FALSE;
	} 

	if (tdi.TriggerCount < 1 )
	{
		std::cout << "TriggerCount was less than 1! " << tdi.TriggerCount << std::endl;
		return false;
	}

	if (tdi.TriggerStride < 1)
	{
		std::cout << "TriggerStride is less than 1!" << tdi.TriggerStride << std::endl;
		return false;
	}


	g_TriggerTestValue = tdi.TriggerTestValue;
	g_TriggerTestValueOffset = tdi.OffsetTriggerTestValue;

	g_TriggerDataMutex.lock();
	g_AllTriggers.clear();
	g_IncludedTriggers.clear();
	g_AllTriggers.reserve(tdi.TriggerCount);
	g_IncludedTriggers.reserve(tdi.TriggerCount);


	bool tryRotation = true;

	std::cout << "Initializing trigger data, count: " + g_TriggerDataInfo.TriggerCount << std::endl;
	bool firstTriggerEval = true;
	for (int i = 0; i < g_TriggerDataInfo.TriggerCount; i++)
	{
		// for debugging rotation, we're just going to work with lz_ledge and lz_bridge on Halo start (index 43 and 8 respectively) 
		if (i != 43 && i != 8) continue;

		Trigger newTrigger;
		uintptr_t pNewTrigger = (uintptr_t)tdi.pFirstTrigger + (i * tdi.TriggerStride);
		if (!IsValidTrigger(pNewTrigger))
		{
			
			continue;
		}

		try
		{
			newTrigger.triggerIndex = i;
			newTrigger.pGameTriggerData = pNewTrigger;
			newTrigger.Position = *(XMFLOAT3*)(pNewTrigger + tdi.OffsetTriggerPosition);
			newTrigger.Extents = *(XMFLOAT3*)(pNewTrigger + tdi.OffsetTriggerExtents);
			newTrigger.RotForward = *(XMFLOAT3*)(pNewTrigger + tdi.OffsetTriggerRotForward);
			newTrigger.RotUp = *(XMFLOAT3*)(pNewTrigger + tdi.OffsetTriggerRotUp);

			if (firstTriggerEval)
			{
				std::cout << "address of first trigger center position: " << (pNewTrigger + tdi.OffsetTriggerExtents) << std::endl;
				std::cout << "address of first trigger extents: " << (pNewTrigger + tdi.OffsetTriggerExtents) << std::endl;
				std::cout << "xpos of first trigger position" << newTrigger.Position.x << std::endl;
				std::cout << "xpos of first trigger extents" << newTrigger.Extents.x << std::endl;
			}


			
			BoundingBox trigBoxUnrotated;
			// Operating in the model-space, for now, we'll translate to the correct position after we do the rotation
			BoundingBox::CreateFromPoints(trigBoxUnrotated, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMLoadFloat3(&newTrigger.Extents));

			BoundingOrientedBox trigBox;
			BoundingOrientedBox::CreateFromBoundingBox(trigBox, trigBoxUnrotated);

			// Construct the rotation transform matrix from right, forward, up, pos
			XMVECTOR right; // Have to derive from forward and up
			XMVECTOR forward = XMVectorSet(newTrigger.RotForward.x, newTrigger.RotForward.y, newTrigger.RotForward.z, 0.f);
			XMVECTOR up = XMVectorSet(newTrigger.RotUp.x, newTrigger.RotUp.y, newTrigger.RotUp.z, 0.f);
			XMVECTOR pos = XMVectorSet(0.f, 0.f, 0.f, 1.f);

			right = XMVector3Cross(up, forward);
			right = XMVector3Normalize(right);




			// Apply rotation
			XMMATRIX transformMatrix = XMMATRIX(forward, right, up, pos);
			trigBox.Transform(trigBox, transformMatrix);

			// Translate to the correct position
			XMMATRIX translationMatrix = XMMatrixTranslationFromVector(XMLoadFloat3(&newTrigger.Position));
			trigBox.Transform(trigBox, translationMatrix);

			// Store the BoundingOrientedBox for hittesting later
			newTrigger.boundingOrientedBox = trigBox;

			// Store the vertices for rendering later
			trigBox.GetCorners(newTrigger.vertexes);

			// Calculate axis-aligned bounding box for fast hit testing later
			BoundingBox::CreateFromPoints(newTrigger.boundingBox, 8, newTrigger.vertexes, sizeof(XMFLOAT3));

			
			// Get triggerName
			char* name = (char*)(pNewTrigger + tdi.OffsetTriggerName);
			if (name[0] != '\0')
			{
				newTrigger.triggerName = (name);
			}
			else
			{
				newTrigger.triggerName = "Error, missing trigger name";
;			}


			if (firstTriggerEval)
			{
				std::cout << "rotation transformation matrix: " << std::endl << transformMatrix;
				std::cout << "translation transformation matrix: " << std::endl << translationMatrix;
			}


		}
		catch (...)
		{
			std::cout << "Exception thrown loading triggers, " + i << ", pointer: " << pNewTrigger << std::endl;
			std::cout << what() << std::endl;

			continue;
		}
		
		g_AllTriggers.push_back(newTrigger);
		firstTriggerEval = false;

	}


	std::copy(g_AllTriggers.begin(), g_AllTriggers.end(), back_inserter(g_IncludedTriggers));

		std::cout << "Finished setting up triggers! Now hooking TriggerHitTest.." << std::endl;
		std::cout << "p_oTriggerHitTest: " << tdi.p_oTriggerHitTest << std::endl << "hkTriggerHitTest: " << hkTriggerHitTest << std::endl;
		// hook TriggerHitTest
		LONG error = NO_ERROR;
		error = DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		std::cout << "hooking TriggerHitTest 1.." << std::endl;
		if (error != NO_ERROR)  std::cout << "ERROR with DetourTransactionBegin" << std::endl;
		error = DetourAttach(&(PVOID&)g_HitTriggerTestOriginal, hkTriggerHitTest);
		std::cout << "hooking TriggerHitTest 2.." << std::endl;
		if (error != NO_ERROR)  std::cout << "ERROR hooking TriggerHitTest" << std::endl << "p_oTriggerHitTest: " << tdi.p_oTriggerHitTest << std::endl << "hkTriggerHitTest: " << hkTriggerHitTest << std::endl;
		error = DetourTransactionCommit();
		std::cout << "hooking TriggerHitTest 3.." << std::endl;
		if (error != NO_ERROR)  std::cout << "ERROR with DetourTransactionCommit" << std::endl;


	g_TriggerDataMutex.unlock();


	return TRUE;
}


bool firsthkTriggerHitTest = true;
// ah. the hkTriggerTest thing is a nested function. uh oh. Could we use inline assembly to fix this?
// could probably just do like push r10, r11, then pop them right before return
// well we might have to do something similiar to this https://github.com/adamhlt/Easy-Mid-Hook/blob/f8ef71d47807cc6828132e449238d73b122a618e/README.md
// where are asm is, yeah push and pop
BOOL hkTriggerHitTest(int16_t triggerIndex, int32_t entityDatum)
{
	__asm(R"(
push %rax\n\t
push %rdx\n\t
push %r10\n\t
push %r11\n\t
)");


	if (firsthkTriggerHitTest)
	{
		std::cout << "hkTriggerHitTest called!" << std::endl << "triggerIndex: " << triggerIndex << ", " << "entityDatum: " << entityDatum << std::endl;
	}
	if (triggerIndex < g_AllTriggers.size())
	{
		UINT32 currentTickcount = *g_TriggerDataInfo.Tickcounter;
		g_AllTriggers[triggerIndex].tickLastSearched = currentTickcount;
	}

	__asm(R"(
pop %rax\n\t
pop %rdx\n\t
pop %r10\n\t
pop %r11\n\t
)");


	return g_HitTriggerTestOriginal(triggerIndex, entityDatum);
}