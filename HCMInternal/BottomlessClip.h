#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"



class IBottomlessClipImpl { public: virtual ~IBottomlessClipImpl() = default; };
class BottomlessClip : public IOptionalCheat
{
private:

std::unique_ptr<IBottomlessClipImpl> pimpl;

public:
	BottomlessClip(GameState gameImpl, IDIContainer& dicon);
~BottomlessClip();

std::string_view getName() override { return nameof(BottomlessClip); }

};