#pragma once
#include "IOptionalCheat.h"
#include "DIContainer.h"
#include "GameState.h"
#include "pch.h"

class TrackTriggerEnterExit : public IOptionalCheat
{
public:
	class ITrackTriggerEnterExit
	{
	public:
		virtual ~ITrackTriggerEnterExit() = default;
	};
private:
	std::unique_ptr<ITrackTriggerEnterExit> pimpl;

public:
	TrackTriggerEnterExit(GameState game, IDIContainer& dicon);


	virtual std::string_view getName() override { return nameof(TrackTriggerEnterExit); }
};

