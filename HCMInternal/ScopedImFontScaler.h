#pragma once
#include "pch.h"
#include "imgui.h"

// smart raii wrapper for changing font scale then resetting it back to previous on destruction
class ScopedImFontScaler {
private: 
	float previousFontScale;
	ImFont* pFont;

public:
	ScopedImFontScaler(float newFontScale)
	{
		this->pFont = ImGui::GetFont();
		this->previousFontScale = this->pFont->Scale;
		this->pFont->Scale = newFontScale;
		ImGui::PushFont(this->pFont);
	}

	~ScopedImFontScaler()
	{
		this->pFont->Scale = previousFontScale;
		ImGui::PopFont();
	}
};