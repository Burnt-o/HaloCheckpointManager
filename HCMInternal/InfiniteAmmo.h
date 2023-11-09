#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"



class IInfiniteAmmoImpl { public: virtual ~IInfiniteAmmoImpl() = default; };
								  class InfiniteAmmo : public IOptionalCheat
								  {
								  private:

									  std::unique_ptr<IInfiniteAmmoImpl> pimpl;

								  public:
									  InfiniteAmmo(GameState gameImpl, IDIContainer& dicon);
									  ~InfiniteAmmo();

									  std::string_view getName() override { return nameof(InfiniteAmmo); }

								  };