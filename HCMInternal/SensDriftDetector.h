#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "DIContainer.h"
#include "GameState.h"

class SensDriftDetectorImplUntemplated
{
public: virtual ~SensDriftDetectorImplUntemplated() = default;
};

/* on each new frame, checks if the players view angle has increased or decreased by more than one discrete mouse-movement-input, printing a message if so.*/
class SensDriftDetector : public IOptionalCheat
{
private:

	std::unique_ptr<SensDriftDetectorImplUntemplated> pimpl;

public:
	SensDriftDetector(GameState game, IDIContainer& dicon);
	~SensDriftDetector();

	virtual std::string_view getName() override { return nameof(SensDriftDetector); }
};