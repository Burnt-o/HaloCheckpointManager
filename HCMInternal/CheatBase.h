#pragma once
#include "GameState.h"
#include "MessagesGUI.h"
#include "FailedServiceInfo.h"

class CheatBase
{
private:


protected:
	GameState mGame;
	virtual void initialize() = 0;

public:
	CheatBase(GameState gameImpl) : mGame(gameImpl) {}
	virtual ~CheatBase() = default;

	void initializeBase()
	{
		attemptedInitialization = true;
		try
		{
			PLOG_VERBOSE << "initializing cheat service: " << mGame.toString() << "::" << getName();
			initialize();
			successfullyInitialized = true;
		}
		catch (HCMInitException ex)
		{
			
			FailedServiceInfo::addServiceFailure(std::string{mGame.toString() + "::" + std::string{getName()}}, ex);
			PLOG_ERROR << "detailed service failure info: " << std::endl
				<< "ex.what(): " << ex.what() << std::endl
				<< "ex.source(): " << ex.source() << std::endl
				<< "ex.trace():" << ex.trace();
		}
		
	}


	virtual std::string_view getName() = 0;
	bool successfullyInitialized = false;
	bool attemptedInitialization = false;

};

template <typename T>
concept CheatBaseTemplate = std::is_base_of<CheatBase, T>::value;


