#pragma once


template <typename T>
class StaticHookWrapper_MidHook {
protected:
    virtual void mFunc(SafetyHookContext ctx) = 0;
public:
	static StaticHookWrapper_MidHook<T>* mInstance;

	static void staticFunc(SafetyHookContext ctx);
};



template <typename T, typename Params, typename ...Args>
class StaticHookWrapper_InlineHook {
protected:
	virtual Params mFunc(Args...) = 0;
public:
	static StaticHookWrapper_InlineHook<T, Params, Args...>* mInstance;

	static Params staticFunc(Args...);
};


// examples
/*
class DerivedA : public StaticHookWrapper_MidHook<DerivedA> {
	virtual void mFunc(SafetyHookContext ctx) override
	{
		// whatever
	};
};

class DerivedB : public StaticHookWrapper_InlineHook<DerivedB, void, int, int> {
	virtual void mFunc(int param1, int param2) override
	{
		// whatever
	};
};

*/


