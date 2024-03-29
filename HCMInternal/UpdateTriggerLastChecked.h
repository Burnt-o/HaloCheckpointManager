#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"



class UpdateTriggerLastCheckedUntemplated { public: virtual ~UpdateTriggerLastCheckedUntemplated() = default; };
class UpdateTriggerLastChecked : public IOptionalCheat
{
private:

	std::unique_ptr<UpdateTriggerLastCheckedUntemplated> pimpl;

public:
	UpdateTriggerLastChecked(GameState gameImpl, IDIContainer& dicon);
	~UpdateTriggerLastChecked();

	std::string_view getName() override { return nameof(UpdateTriggerLastChecked); }

};