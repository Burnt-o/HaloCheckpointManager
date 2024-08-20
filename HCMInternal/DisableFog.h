#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"



class DisableFog : public IOptionalCheat
{

public:
	class DisableFogImpl;
	DisableFog(GameState gameImpl, IDIContainer& dicon);
	~DisableFog();

	virtual std::string_view getName() override {
		return nameof(DisableFog);
	}

private:
	std::unique_ptr<DisableFogImpl> pimpl;
};