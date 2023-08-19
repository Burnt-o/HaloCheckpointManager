#include "pch.h"
#include "CheatManager.h"

std::map<GameState, std::shared_ptr<CheatBase>> CheatManager::forceCheckpointCollection{};
std::map<GameState, std::shared_ptr<CheatBase>> CheatManager::forceRevertCollection{};
std::map<GameState, std::shared_ptr<CheatBase>> CheatManager::forceDoubleRevertCollection{};
std::map<GameState, std::shared_ptr<CheatBase>> CheatManager::checkpointInjectDumpCollection{};