#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"



class CarrierBumpAnalyser : public IOptionalCheat
{
	private:
		class CarrierBumpAnalyserImpl;
		std::unique_ptr<CarrierBumpAnalyserImpl> pimpl;

	public:
		CarrierBumpAnalyser(GameState gameImpl, IDIContainer& dicon);
		~CarrierBumpAnalyser();

		std::string_view getName() override { return nameof(CarrierBumpAnalyser); }

};