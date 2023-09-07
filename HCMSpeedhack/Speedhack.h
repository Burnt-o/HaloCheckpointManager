#pragma once

class SpeedhackImplBase {
public:
	virtual void setSpeed(double in) = 0;
};



class Speedhack
{
private:
	static inline Speedhack* instance;
	std::unique_ptr<SpeedhackImplBase> impl;
public: 

	Speedhack();
	~Speedhack();
	static void setSpeed(double in) { 
		if (!instance)
		{
			instance = new Speedhack();
		}
		return instance->impl.get()->setSpeed(in); }
};


extern "C"
{
	__declspec(dllexport) extern void setSpeed(double in) { 
		return Speedhack::setSpeed(in); }
}
