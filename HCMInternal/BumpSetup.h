#pragma once
#include "pch.h"
#include "GameInputEnum.h"
#include "BumpPlayerStruct.h"
#include "BumpControlDefs.h"

typedef std::pair<InpMask, int> InputAndLength;
typedef std::vector<std::pair<InputAndLength, bool>> SetupInputVector;
typedef std::vector<InputAndLength> GoInputVectorCollapsed;
typedef std::vector<InpMask> GoInputVector;




struct BumpSetup
{
	std::vector<std::vector < std::pair<float, float>>> leftRightAngleRanges;

#ifdef SHIELD_BUMPS
	int shieldOnTickLastResult = -1000;
#endif


	SetupInputVector setupInputs = {};
	GoInputVector goInputs;


	InpMask goIdleInput = InpMask(Input::None); // either none or crouch.

	int complexityLevel() const { return setupInputs.size() + goInputs.size(); }
	GoInputVectorCollapsed GetCollapsedGoInputVector() const
	{
		GoInputVectorCollapsed out;
		InpMask previousInput(Input::None);
		for (int i = 0; i < goInputs.size(); i++)
		{
			if (goInputs.at(i) != previousInput)
			{
				out.emplace_back(InputAndLength(goInputs.at(i), 1));
			}
			else
			{
				out.back().second++;
			}

			previousInput = goInputs.at(i);
		}
		return out;
	}



	friend std::ostream& operator<<(std::ostream& out, const BumpSetup& setup);

	bool operator==(const BumpSetup& other) const
	{
		return (goIdleInput == other.goIdleInput
			&& goInputs == other.goInputs
			&& setupInputs == other.setupInputs
#ifdef SHIELD_BUMPS
			&& shieldOnTickLastResult == other.shieldOnTickLastResult
#endif
			);
	}
};


struct BumpSetupHasher
{
	std::size_t operator()(const BumpSetup& k) const;
};


