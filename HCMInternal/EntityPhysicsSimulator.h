#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "ScopedServiceRequest.h"
#include "Datum.h"



class EntityPhysicsSimulator : public IOptionalCheat
{
private:
	class EntityPhysicsSimulatorImpl;
	std::unique_ptr<EntityPhysicsSimulatorImpl> pimpl;

public:
	EntityPhysicsSimulator(GameState gameImpl, IDIContainer& dicon);

	~EntityPhysicsSimulator();
	std::string_view getName() override { return nameof(EntityPhysicsSimulator); }


	typedef void(AdvancePhysicsTicks)(Datum entityDatum, int ticksToAdvance);
	typedef void(SimulationThread)(AdvancePhysicsTicks*);
	void beginSimulation(SimulationThread* beginThread);

	static void setShieldDatum(Datum shieldDatum);

};