#pragma once
#include "pch.h"
#include "HaloEnums.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_stdlib.h"

class GUIElementBase
{
protected:
	float currentHeight = 0;
	std::set<GameState> supportedGames;
public:
	float getCurrentHeight() { return this->currentHeight; }
	virtual std::set<GameState>& getSupportedGames() = 0;
	virtual void render() = 0;

};

