#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "DIContainer.h"
#include "GameState.h"
#include "ObservedEvent.h"

using GameTickEvent = eventpp::CallbackList<void(uint32_t)>;

class GameTickEventHook : public IOptionalCheat
{
public:
	class GameTickEventHookImpl
	{
	public:
		virtual ~GameTickEventHookImpl() = default;
		virtual std::shared_ptr<ObservedEvent<GameTickEvent>> getGameTickEvent() = 0;
		virtual uint32_t getCurrentGameTick() = 0;
	};
private:
	std::unique_ptr<GameTickEventHookImpl> pimpl;
	static inline std::mutex constructionMutex;

public:
	GameTickEventHook(GameState game, IDIContainer& dicon);
	~GameTickEventHook();
	std::shared_ptr<ObservedEvent<GameTickEvent>> getGameTickEvent();
	uint32_t getCurrentGameTick();

	std::string_view getName() override { return nameof(GameTickEventHook); }
};