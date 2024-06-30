#pragma once
#include "pch.h"
#include "imgui.h"

// smart raii wrapper for changing font scale then resetting it back to previous on destruction
class ScopedImFontScaler {
private: 
	float previousFontScale;
	ImFont* const pFont;

public:
	ScopedImFontScaler(float newFontScale);

	~ScopedImFontScaler();
};