#include "pch.h"
#include "PointInsideTrigger.h"
#include "MultilevelPointer.h"
#include "PointerDataStore.h"
/*
template<GameState::Value gameT>
class PointInsideTriggerImpl : public IPointInsideTrigger
{
private:
	std::shared_ptr<MultilevelPointer> pointInsideTriggerFunction;

public:

	PointInsideTriggerImpl(GameState gameImpl, IDIContainer& dicon)
	{
		auto ptr = dicon.Resolve<PointerDataStore>().lock();
		auto pointInsideTriggerFunction = ptr->getData < std::shared_ptr<MultilevelPointer>>(nameof(pointInsideTriggerFunction), game);

	}

	virtual bool isPointInsideTrigger(const SimpleMath::Vector3& worldPoint, const uintptr_t triggerData, const uint32_t triggerIndex) override
	{
		uintptr_t pointInsideTriggerFunctionResolved;
		if (!pointInsideTriggerFunction->resolve(&pointInsideTriggerFunctionResolved))
			throw HCMRuntimeException(std::format("Failed to resolve pointInsideTriggerFunction, error: {}", MultilevelPointer::GetLastError()));

		if constexpr (mGame == GameState::Value::Halo1)
		{
			typedef int64_t(*PointInsideTrigger_t)(uint16_t tIndex, SimpleMath::Vector3* wPos);
			PointInsideTrigger_t pointInsideTrigger_vptr;
			pointInsideTrigger_vptr = static_cast<PointInsideTrigger_t>((void*)pointInsideTriggerFunctionResolved);
			return pointInsideTrigger_vptr(triggerIndex, &worldPoint);
		}
		else if constexpr (mGame == GameState::Value::Halo2)
		{
			// should double check if the game is making a copy of the trigger data and whether we need to as well
			typedef int64_t(*PointInsideTrigger_t)(uintptr_t triggerDataPlus10, SimpleMath::Vector3* wPos, uintptr_t triggerData);
			PointInsideTrigger_t pointInsideTrigger_vptr;
			pointInsideTrigger_vptr = static_cast<PointInsideTrigger_t>((void*)pointInsideTriggerFunctionResolved);
			return pointInsideTrigger_vptr((triggerData + 0x10), &worldPoint, triggerData);
		}
		else if constexpr (mGame == GameState::Value::Halo3)
		{
			// same as h2 but trigger data is a copy? shouoould be fine
			typedef int64_t(*PointInsideTrigger_t)(uintptr_t triggerDataPlus10, SimpleMath::Vector3* wPos, uintptr_t triggerData);
			PointInsideTrigger_t pointInsideTrigger_vptr;
			pointInsideTrigger_vptr = static_cast<PointInsideTrigger_t>((void*)pointInsideTriggerFunctionResolved);
			return pointInsideTrigger_vptr((triggerData + 0x10), &worldPoint, triggerData);
		}
		else if constexpr (mGame == GameState::Value::Halo3)
		{
			// trigger data + 0x5C
			typedef int64_t(*PointInsideTrigger_t)(SimpleMath::Vector3* wPos, uintptr_t triggerDataPlus5C);
			PointInsideTrigger_t pointInsideTrigger_vptr;
			pointInsideTrigger_vptr = static_cast<PointInsideTrigger_t>((void*)pointInsideTriggerFunctionResolved);
			return pointInsideTrigger_vptr((triggerData + 0x10), &worldPoint, triggerData);
		}

	}

};



PointInsideTrigger::PointInsideTrigger(GameState gameImpl, IDIContainer& dicon)
{
	switch (gameImpl)
	{
	case GameState::Value::Halo1:
		pimpl = std::make_unique<PointInsideTriggerImpl<GameState::Value::Halo1>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo2:
		pimpl = std::make_unique<PointInsideTriggerImpl<GameState::Value::Halo2>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo3:
		pimpl = std::make_unique<PointInsideTriggerImpl<GameState::Value::Halo3>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo3ODST:
		pimpl = std::make_unique<PointInsideTriggerImpl<GameState::Value::Halo3ODST>>(gameImpl, dicon);
		break;

	case GameState::Value::HaloReach:
		pimpl = std::make_unique<PointInsideTriggerImpl<GameState::Value::HaloReach>>(gameImpl, dicon);
		break;

	case GameState::Value::Halo4:
		pimpl = std::make_unique<PointInsideTriggerImpl<GameState::Value::Halo4>>(gameImpl, dicon);
		break;
	default:
		throw HCMInitException("not impl yet");
	}
}

PointInsideTrigger::~PointInsideTrigger() = default;



*/