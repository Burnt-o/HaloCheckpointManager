#pragma once
#include <memory>

class HookStateMachine
{
private:
	class HookStateMachineImpl;
	std::unique_ptr <HookStateMachineImpl> pimpl;

public:
	HookStateMachine();
	~HookStateMachine();
	void update();
	void reset();
};

