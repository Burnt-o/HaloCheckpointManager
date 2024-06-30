#include "pch.h"
#include "ScopedImFontScaler.h"
#include "ImGuiManager.h"
ScopedImFontScaler::ScopedImFontScaler(float newFontScale)
// tries to grab the rescalable font from ImGuiManager (if it's not nullptr), otherwise whatever the current font is
	: pFont(ImGuiManager::getRescalableMonospacedFont() ? ImGuiManager::getRescalableMonospacedFont() : ImGui::GetFont())
{
	this->previousFontScale = this->pFont->Scale;
	this->pFont->Scale = newFontScale / 16.f;
	ImGui::PushFont(this->pFont);
}

ScopedImFontScaler::~ScopedImFontScaler()
{
	this->pFont->Scale = previousFontScale;
	ImGui::PopFont();
}