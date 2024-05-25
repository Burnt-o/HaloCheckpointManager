#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "PointerDataStore.h"
#include "MultilevelPointer.h"

class HideWatermarkCheck : public IOptionalCheat
{
private:



public:
	HideWatermarkCheck(GameState gameImpl, IDIContainer& dicon)
	{
		auto ptr = dicon.Resolve<PointerDataStore>().lock();
		auto dxgiInternalPresentFunction = ptr->getData < std::shared_ptr<MultilevelPointer>>(nameof(dxgiInternalPresentFunction));
		assert(dxgiInternalPresentFunction);
	}


	std::string_view getName() override { return nameof(HideWatermarkCheck); }

};