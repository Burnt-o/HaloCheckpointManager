#pragma once
#ifndef HCM_INCULDES_INCLUDED
#define HCM_INCULDES_INCLUDED
#include <Windows.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include <string>
#include <mutex>


//std::string what(const std::exception_ptr& eptr = std::current_exception());


extern ID3D11DeviceContext* g_pContext;




extern std::string what(const std::exception_ptr& eptr = std::current_exception());



extern bool g_hcmInitialised;
extern ImFont* g_defaultFont;


void DrawDisplayInfo();
LONG InitSpeedhack();

extern bool g_hookEnabled;

extern std::string g_HCMExternalPath;



extern float g_screenWidth;
extern float g_screenWidthHalved;
extern float g_screenHeight;
extern float g_screenHeightHalved;
extern float g_aspectRatio;

#endif

