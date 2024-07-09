#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"

/*

class IPointInsideTrigger 
{ 
public: 
	virtual ~IPointInsideTrigger() = default; 
	virtual bool isPointInsideTrigger(const SimpleMath::Vector3& worldPoint, const uintptr_t triggerData, const uint32_t triggerIndex) = 0;
};
class PointInsideTrigger : public IOptionalCheat
{
private:

	std::unique_ptr<IPointInsideTrigger> pimpl;

public:
	PointInsideTrigger(GameState gameImpl, IDIContainer& dicon);
	~PointInsideTrigger();

	bool isPointInsideTrigger(const SimpleMath::Vector3& worldPoint, const uintptr_t triggerData, const uint32_t triggerIndex) {
		return pimpl->isPointInsideTrigger(worldPoint, triggerData, triggerIndex);
	}

	std::string_view getName() override { return nameof(PointInsideTrigger); }

};

*/