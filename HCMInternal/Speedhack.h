#pragma once
#include "pch.h"
#include "GameState.h"
#include "IOptionalCheat.h"
#include "DIContainer.h"

class ISpeedhackImpl {};

class Speedhack : public IOptionalCheat
{
private:
	//impl. static, shared between all games
	static inline std::unique_ptr<ISpeedhackImpl> impl;

public:
	// is this the best way to do this.. why can't we send speedhack only the dependencies it needs?
	// also should cheats be templated instead of taking a game parameter?
	Speedhack(GameState game, IDIContainer& dicon);
	~Speedhack()
	{
		PLOG_DEBUG << "~Speedhack() killing impl";
		// kill impl
		impl.reset();
	}

	virtual std::string_view getName() override { return nameof(Speedhack); }
};