#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "ObjectPoolContainer.h"


class GameObjectPool : public IOptionalCheat, public IProvideObjectPoolContainer
{
private:

	std::unique_ptr<IProvideObjectPoolContainer> pimpl;

public:
	GameObjectPool(GameState gameImpl, IDIContainer& dicon);

    virtual ObjectPoolContainer::iterator begin() override { return pimpl->begin();  }
    virtual ObjectPoolContainer::iterator end() override { return pimpl->end(); }

    virtual bool empty() override { return pimpl->empty(); }

    virtual uint16_t size() override { return pimpl->size(); }


	std::string_view getName() override { return nameof(GameObjectPool); }

};

