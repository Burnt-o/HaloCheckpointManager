#include "pch.h"
#include "GetTriggerData.h"
#include "GetTriggerData.h"
#include "Render3DEventProvider.h"

template<GameState::Value mGame>
class GetTriggerDataImpl : public GetTriggerDataUntemplated
{
private:

	std::map<TriggerPointer, TriggerData> triggerData;
	bool triggerDataCached = false;

	void updateTriggerData()
	{
		triggerData.clear();
		triggerData.emplace(0, TriggerData("box", TriggerModel({14.671300, -95.203300, -73.22693634 }, {0.2, 0.2, 0.2}, SimpleMath::Vector3::Forward, SimpleMath::Vector3::Up))); // test trigger at keyes start
	
		// TODO: get the real data
	}

public:
	GetTriggerDataImpl(GameState gameImpl, IDIContainer& dicon)
	{

	}

	virtual std::map<TriggerPointer, TriggerData>& getTriggerData() override
	{
		if (triggerDataCached == false)
			updateTriggerData();

		return triggerData;
	}
};


std::map<TriggerPointer, TriggerData>& GetTriggerData::getTriggerData()
{
	return pimpl->getTriggerData();
}

GetTriggerData::GetTriggerData(GameState gameImpl, IDIContainer& dicon)
{
	switch (gameImpl)
	{
	case GameState::Value::Halo1:
		pimpl = std::make_unique<GetTriggerDataImpl<GameState::Value::Halo1>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo2:
		pimpl = std::make_unique<GetTriggerDataImpl<GameState::Value::Halo2>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo3:
		pimpl = std::make_unique<GetTriggerDataImpl<GameState::Value::Halo3>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo3ODST:
		pimpl = std::make_unique<GetTriggerDataImpl<GameState::Value::Halo3ODST>>(gameImpl, dicon);
		break;

	case GameState::Value::HaloReach:
		pimpl = std::make_unique<GetTriggerDataImpl<GameState::Value::HaloReach>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo4:
		pimpl = std::make_unique<GetTriggerDataImpl<GameState::Value::Halo4>>(gameImpl, dicon);
		break;
	default:
		throw HCMInitException("not impl yet");
	}
}

GetTriggerData::~GetTriggerData() = default;