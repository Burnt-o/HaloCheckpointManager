#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"

// Wrapper for GameEngineDetail
// and based on some debug string found in mcc
class GameEngineFunctions : public IOptionalCheat
{
private:
	class GameEngineFunctionsImpl;
	std::unique_ptr< GameEngineFunctionsImpl> pimpl;

public:
	virtual std::string_view getName() override {
		return nameof(GameEngineFunctions);
	}
	GameEngineFunctions(GameState gameImpl, IDIContainer& dicon);
	~GameEngineFunctions();

	void SetPause(bool isOn);
    void RestartCheckpoint();
    void RestartLevel();
    void LoadMainMenu();
    void SendCommand(const char* command);
};