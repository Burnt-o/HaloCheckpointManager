#include "pch.h"
#include "UpdateTriggerLastChecked.h"
#include "GetTriggerData.h"
#include "Render3DEventProvider.h"

template<GameState::Value mGame>
class UpdateTriggerLastCheckedImpl : public UpdateTriggerLastCheckedUntemplated
{
private:

public:
	UpdateTriggerLastCheckedImpl(GameState gameImpl, IDIContainer& dicon)
	{
		// TODO
		throw HCMInitException("Not implemented yet");
	}


};



UpdateTriggerLastChecked::UpdateTriggerLastChecked(GameState gameImpl, IDIContainer& dicon)
{
	switch (gameImpl)
	{
	case GameState::Value::Halo1:
		pimpl = std::make_unique<UpdateTriggerLastCheckedImpl<GameState::Value::Halo1>>(gameImpl, dicon);
		break;

	//case GameState::Value::Halo2:
	//	pimpl = std::make_unique<UpdateTriggerLastCheckedImpl<GameState::Value::Halo2>>(gameImpl, dicon);
	//	break;

	//case GameState::Value::Halo3:
	//	pimpl = std::make_unique<UpdateTriggerLastCheckedImpl<GameState::Value::Halo3>>(gameImpl, dicon);
	//	break;

	//case GameState::Value::Halo3ODST:
	//	pimpl = std::make_unique<UpdateTriggerLastCheckedImpl<GameState::Value::Halo3ODST>>(gameImpl, dicon);
	//	break;

	//case GameState::Value::HaloReach:
	//	pimpl = std::make_unique<UpdateTriggerLastCheckedImpl<GameState::Value::HaloReach>>(gameImpl, dicon);
	//	break;

	//case GameState::Value::Halo4:
	//	pimpl = std::make_unique<UpdateTriggerLastCheckedImpl<GameState::Value::Halo4>>(gameImpl, dicon);
	//	break;
	default:
		throw HCMInitException("not impl yet");
	}
}

UpdateTriggerLastChecked::~UpdateTriggerLastChecked() = default;