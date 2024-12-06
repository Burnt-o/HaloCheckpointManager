#include "pch.h"
#include "GameEngineFunctions.h"
#include "GameEngineDetail.h"
#include "IMakeOrGetCheat.h"
#include "RuntimeExceptionHandler.h"


class GameEngineFunctions::GameEngineFunctionsImpl 
{
    std::shared_ptr< GameEngineDetail> gameEngineDetail;
    std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;

    void handle_error(std::string error) {

        HCMRuntimeException ex(error);
        runtimeExceptions->handleMessage(ex);
    }

public:
    GameEngineFunctionsImpl(GameState game, IDIContainer& dicon)
    {
        gameEngineDetail = resolveDependentCheat(GameEngineDetail);
    }

    void SetPause(bool isOn)
    {
        auto result = gameEngineDetail->getCommonHandle()->set_event(isOn ? ICommonGameEngineHandle::EventPause : ICommonGameEngineHandle::EventResume, nullptr);
        if (!result) handle_error(std::format("Failed to set pause: {}", result.error()));
    }
    void RestartCheckpoint()
    {
        auto result = gameEngineDetail->getCommonHandle()->set_event(ICommonGameEngineHandle::EventLoadCheckpoint, nullptr);
        if (!result) handle_error(std::format("Failed to revert: {}", result.error()));
    }
    void RestartLevel()
    {
        PLOG_VERBOSE << "GameEngineFunctionsImpl::RestartLevel()";
        auto result = gameEngineDetail->getCommonHandle()->set_event(ICommonGameEngineHandle::EventRestart, nullptr);
        if (!result) handle_error(std::format("Failed to restart level: {}", result.error()));
    }
    void LoadMainMenu()
    {
        auto result = gameEngineDetail->getCommonHandle()->set_event(ICommonGameEngineHandle::EventExit, nullptr);
        if (!result) handle_error(std::format("Failed to exit to main menu: {}", result.error()));
    }

    void SendCommand(const char* command)
    {
        std::string HScommand = "HS: " + std::string(command);
        auto result = gameEngineDetail->getCommonHandle()->execute_command(HScommand.c_str());
        if (!result) handle_error(std::format("Failed to send command: {}", result.error()));
    }
};


GameEngineFunctions::GameEngineFunctions(GameState gameImpl, IDIContainer& dicon)
    : pimpl(std::make_unique<GameEngineFunctionsImpl>(gameImpl, dicon))
{
}

GameEngineFunctions::~GameEngineFunctions() = default;

void GameEngineFunctions::SetPause(bool isOn) { return pimpl->SetPause(isOn); }
void GameEngineFunctions::RestartCheckpoint() { return pimpl->RestartCheckpoint(); }
void GameEngineFunctions::RestartLevel() { return pimpl->RestartLevel(); }
void GameEngineFunctions::LoadMainMenu() { return pimpl->LoadMainMenu(); }
void GameEngineFunctions::SendCommand(const char* command) { return pimpl->SendCommand(command); }