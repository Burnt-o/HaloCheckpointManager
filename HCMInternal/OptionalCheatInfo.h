#pragma once
#include "GameState.h"
#include "OptionalCheatEnum.h"
struct OptionalCheatInfoData
{

private:
	friend class OptionalCheatInfo;
	OptionalCheatInfoData(bool sigDifferentiator) : init(false), initAttempted(false), ex("initialization not attempted") {}; // bool input ignored, just exists to differentiate the function signature
	static OptionalCheatInfoData emptyCheat() { return OptionalCheatInfoData(true); }
public:
	const bool init;
	const bool initAttempted;
	const std::optional<HCMInitException> ex;

	OptionalCheatInfoData() : init(true), initAttempted(true), ex(std::nullopt) {}
	OptionalCheatInfoData(HCMInitException e) : init(false), initAttempted(true), ex(e) {}
};



class OptionalCheatInfo
{
private:
	OptionalCheatInfoData emptyInfo = OptionalCheatInfoData::emptyCheat();
	std::map<std::pair<GameState, OptionalCheatEnum>, OptionalCheatInfoData> infoMap;

	friend class OptionalCheatConstructor;
	void setInfo(std::pair<GameState, OptionalCheatEnum> gameCheatPair, OptionalCheatInfoData info)
	{
		infoMap.emplace(gameCheatPair, info);
	}

public:
	const OptionalCheatInfoData& getOptionalCheatInfo(std::pair<GameState, OptionalCheatEnum> gameCheatPair)
	{
		if (!infoMap.contains(gameCheatPair)) return emptyInfo;
		return infoMap.at(gameCheatPair);
	}

};

