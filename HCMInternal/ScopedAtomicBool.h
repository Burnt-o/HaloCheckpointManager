#pragma once

// there's probably some std thing that does what I want here but can't figure out the words to google
class ScopedAtomicBool
{
	std::atomic_bool& atom;
public:
	ScopedAtomicBool(std::atomic_bool& atm) : atom(atm)
	{
		atm.wait(true);
		atom = true;
	}
	~ScopedAtomicBool() { atom = false; atom.notify_all(); }
};

class ScopedAtomicBoolShared
{
	std::shared_ptr<std::atomic_bool> atom;
public:
	ScopedAtomicBoolShared(std::shared_ptr<std::atomic_bool> atm) : atom(atm)
	{
		atom->wait(true);
		atom->operator=(true);
	}
	~ScopedAtomicBoolShared() { atom->operator=(false); atom->notify_all(); }
};