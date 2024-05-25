#pragma once
#include "pch.h"
#include "GameInputEnum.h"
#include "BumpPlayerStruct.h"
#include "BumpControlDefs.h"
#include "BumpSetup.h"


typedef std::tuple<BumpSetup, Nook, SimpleMath::Vector3, float, std::vector<DirectX::BoundingBox>> TestSetup;

class BumpSetupGenerator
{
private:
	class BumpSetupGeneratorImpl;
	std::unique_ptr< BumpSetupGeneratorImpl> pimpl;
public:
	BumpSetupGenerator(int startIndex = 0);
	~BumpSetupGenerator();


	std::vector<TestSetup> getTestCases();
	long long getSetupCount();
	BumpSetup getSetup();
};
