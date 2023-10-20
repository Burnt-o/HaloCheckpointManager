#pragma once
#include "GameState.h"
#include "GetMCCVersion.h"
// Uses libcurl to download PointerData.xml from the github page,
// instantiating all the MultilevelPointers (and other data) specific to the current MCC version we were injected into

typedef std::map<std::string, int> DynStructOffsetInfo;


class PointerManager
{

private:

	template <typename T>
	void getVectorImpl(std::string dataName, std::vector<T>& out);



	class PointerManagerImpl;
	std::unique_ptr<PointerManagerImpl> impl;
public:
	PointerManager(std::shared_ptr<IGetMCCVersion> ver, std::string dirPath);
	~PointerManager();

	template <typename T>
	T getData(std::string dataName, std::optional<GameState> game = std::nullopt);

	template <typename T>
	std::shared_ptr<std::vector<T>> getVectorData(std::string dataName, std::optional<GameState> game = std::nullopt);

};