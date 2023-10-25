#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "DIContainer.h"
#include "GameState.h"
class GameTickEventHook : public IOptionalCheat
{
public:
	class GameTickEventHookImpl
	{
	public:
		virtual ~GameTickEventHookImpl() = default;
		virtual std::shared_ptr<eventpp::CallbackList<void(int)>> getGameTickEvent() = 0;
	};
private:
	std::unique_ptr<GameTickEventHookImpl> pimpl;

public:
	GameTickEventHook(GameState game, IDIContainer& dicon);
	~GameTickEventHook();
	std::shared_ptr<eventpp::CallbackList<void(int)>> getGameTickEvent();

	std::string_view getName() override { return nameof(GameTickEventHook); }
};