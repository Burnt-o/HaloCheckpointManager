#pragma once
#include "pch.h"
#include "GameState.h"
#include "OptionsState.h"
#include "MultilevelPointer.h"
#include "PointerManager.h"
#include "MessagesGUI.h"
#include "CheatBase.h"


class SpeedhackImplBase {
public:
	virtual void onToggle(bool& newValue) = 0;
	virtual void updateSetting(double& newValue) = 0;
};

class Speedhack : public CheatBase
{


private:



	//impl. static, shared between all games
	static inline std::unique_ptr<SpeedhackImplBase> impl{};



public:

	Speedhack(GameState game) : CheatBase(game) {}
	~Speedhack()
	{
		// kill impl
		impl.reset();
	}

	void initialize() override;



	std::string_view getName() override { return "Speedhack"; }


};