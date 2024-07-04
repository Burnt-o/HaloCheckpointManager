#include "pch.h"
#include "BSPChangeHookEvent.h"


class BSPChangeHookEvent::BSPChangeHookEventImpl
{

};

BSPChangeHookEvent::BSPChangeHookEvent(GameState game, IDIContainer& dicon)
{
	throw HCMInitException(std::format("{} not impl yet", nameof(BSPChangeHookEvent)));
}
BSPChangeHookEvent::~BSPChangeHookEvent() = default;

std::shared_ptr<eventpp::CallbackList<void(uint32_t)>> BSPChangeHookEvent::getBSPChangeEvent() { throw; }