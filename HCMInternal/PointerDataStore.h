#pragma once
#include "pch.h"
#include "GameState.h"


class PointerDataStore
{
private:
	typedef std::tuple<std::string, std::optional<GameState>> DataKey;
	std::map<DataKey, std::any> mDataStore;
	friend class PointerDataParser; // this friend will fill up our data store
public:

	template <typename T>
	T getData(std::string dataName, std::optional<GameState> game = std::nullopt);

	template <typename T>
	std::shared_ptr<std::vector<T>> getVectorData(std::string dataName, std::optional<GameState> game = std::nullopt);
};

