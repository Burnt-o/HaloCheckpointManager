#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "ScopedServiceRequest.h"




class BumpTestingTool : public IOptionalCheat
{
private:
	class BumpTestingToolImpl;
	std::unique_ptr<BumpTestingToolImpl> pimpl;

public:
	BumpTestingTool(GameState gameImpl, IDIContainer& dicon);

	~BumpTestingTool();
	std::string_view getName() override { return nameof(BumpTestingTool); }



};