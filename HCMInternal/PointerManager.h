#pragma once
#include "GameState.h"
#include "MultilevelPointer.h"
#include "MidhookContextInterpreter.h"

// Uses libcurl to download PointerData.xml from the github page,
// instantiating all the MultilevelPointers (and other data) specific to the current MCC version we were injected into



class PointerManager
{

private:
	// singleton
	//static PointerManager& get() {
	//	static PointerManager instance;
	//	return instance;
	//}
	static PointerManager* instance;


	template <typename T>
	static void getVectorImpl(std::string dataName, std::vector<T>& out);



	class PointerManagerImpl;
	std::unique_ptr<PointerManagerImpl> impl;
public:
	PointerManager();
	~PointerManager();

	template <typename T>
	static T getData(std::string dataName, std::optional<GameState> game = std::nullopt);

	static std::string getCurrentGameVersion();


	//template <typename T>
	//static void getVector(std::string dataName, std::vector<T>& out);
};