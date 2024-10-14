#pragma once
#include "pch.h"
#include "VersionInfo.h"
#include "GameState.h"
#include "PointerDataTypes.h"

class PointerDataStore
{
private:
	typedef std::tuple<std::string, std::optional<GameState>> DataKey;
	std::map<DataKey, std::any> mDataStore;

public:
	explicit PointerDataStore(std::map<DataKey, std::any> dataStore) : mDataStore(std::move(dataStore)) {}

	template <typename T>
	T getData(std::string dataName, std::optional<GameState> game = std::nullopt);

	template <typename T>
	std::shared_ptr<std::vector<T>> getVectorData(std::string dataName, std::optional<GameState> game = std::nullopt);

};

