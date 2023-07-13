#pragma once
#include "HaloEnums.h"



class CheatBase
{
private:
	bool isUsable = false;

protected:
	GameState mGameImpl;
	static std::string_view getClassNameMain1() { return "CheatBase"; }

	void setUsability(bool newVal) { isUsable = newVal; }
public:
	CheatBase(GameState gameImpl) : mGameImpl(gameImpl) {}
	virtual ~CheatBase() = default;

	virtual void initialize() = 0;
	virtual std::string_view getName() = 0;
	virtual std::set<GameState> getSupportedGames() = 0;
	bool getUsability() { return isUsable; }

};

template <typename T>
concept CheatBaseTemplate = std::is_base_of<CheatBase, T>::value;


