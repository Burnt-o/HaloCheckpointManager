#pragma once
#include "pch.h"
#include "GameState.h"
#include "IOptionalCheat.h"
#include "DIContainer.h"

class ISpeedhackImpl 
{ 
public:
	virtual ~ISpeedhackImpl() = default;
	virtual double getCurrentSpeedMultiplier() = 0;
};

class Speedhack : public IOptionalCheat
{
private:
	//impl. static, shared between all games
	static inline std::unique_ptr<ISpeedhackImpl> impl;
	static inline std::mutex implConstructionCheck;
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

	double getCurrentSpeedMultiplier();

	virtual std::string_view getName() override { return nameof(Speedhack); }
};