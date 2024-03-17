#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "Datum.h"


class MeasurePlayerDistanceToObject : public IOptionalCheat
{
private:
	class MeasurePlayerDistanceToObjectImpl;
	std::unique_ptr<MeasurePlayerDistanceToObjectImpl> pimpl;

public:
	MeasurePlayerDistanceToObject(GameState gameImpl, IDIContainer& dicon);
	~MeasurePlayerDistanceToObject();

	// returns std::nullopt if player or object datum is invalid
	std::optional<float> measure(SimpleMath::Vector3 worldPos, bool measureHorizontalOnly = false);
	std::optional<float> measure(Datum objectDatum, bool measureHorizontalOnly = false);

	std::string_view getName() override { return nameof(MeasurePlayerDistanceToObject); }

};