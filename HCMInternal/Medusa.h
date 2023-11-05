#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"


class MedusaImplUntemplated { public: virtual ~MedusaImplUntemplated() = default; };

class Medusa : public IOptionalCheat
{
private:
	std::unique_ptr<MedusaImplUntemplated> pimpl;

public:
	Medusa(GameState gameImpl, IDIContainer& dicon);
	~Medusa();

	std::string_view getName() override { return nameof(Medusa); }

};