#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "BSPSet.h"


// Third gen only. Allows you to manipulate which BSPs are loaded (by index or set of indexes)
class SwitchBSPSet : public IOptionalCheat
{
private:
	class SwitchBSPSetImpl;
	std::unique_ptr<SwitchBSPSetImpl> pimpl;

public:
	SwitchBSPSet(GameState gameImpl, IDIContainer& dicon);
	~SwitchBSPSet();

	void LoadBSPSet(BSPSet bspSet);
	void UnloadBSPByIndex(int BSPindex);
	void LoadBSPByIndex(int BSPindex);

	std::string_view getName() override { return nameof(SwitchBSP); }

};