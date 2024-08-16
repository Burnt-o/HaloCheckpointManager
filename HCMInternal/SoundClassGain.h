#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"

enum class CommonSoundType
{
	dialog,
	ambience,
	weapons,
	vehicles,
	music,
	other
};


class SoundClassGain : public IOptionalCheat
{

public:
	SoundClassGain(GameState game, IDIContainer& dicon);
	void soundTypeSetGain(CommonSoundType soundType, float value);
	const float soundTypeGetGain(CommonSoundType soundType) const;

	virtual std::string_view getName() override {
		return nameof(SoundClassGain);
	}


	class IImpl
	{
	public:
		virtual void soundTypeSetGain(CommonSoundType soundType, float value) = 0;
		virtual const float soundTypeGetGain(CommonSoundType soundType) const = 0;
		virtual ~IImpl() = default;
	};

private:
	std::unique_ptr<IImpl> pimpl;
};