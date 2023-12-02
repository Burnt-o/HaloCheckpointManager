#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"

// enables 3rd person rendering of the players model, and hides game UI

class ThirdPersonRenderingImplUntemplated 
{ 
public: 
	virtual ~ThirdPersonRenderingImplUntemplated() = default; 
	virtual void toggleThirdPersonRendering(bool enabled) = 0;
};
class ThirdPersonRendering : public IOptionalCheat
{
private:
	std::unique_ptr<ThirdPersonRenderingImplUntemplated> pimpl;


public:
	ThirdPersonRendering(GameState gameImpl, IDIContainer& dicon);

	~ThirdPersonRendering();

	std::string_view getName() override { return nameof(ThirdPersonRendering); }

	void toggleThirdPersonRendering(bool enabled);

};