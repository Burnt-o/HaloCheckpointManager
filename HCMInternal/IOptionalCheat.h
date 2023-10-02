#pragma once

class IOptionalCheat
{
public:
	virtual std::string_view getName() = 0;
};


//#define MAKE_CHEAT_PIMPL_HEADER(cheatName) \
//class cheatName : public IOptionalCheat\
//{\
//private:\
//	class cheatName##Impl;\
//	std::unique_ptr<cheatName##Impl> impl;\
//	\
//public:\
//	cheatName(GameState game, IDIContainer dicon);\
//	~cheatName();\
//	\
//	std::string_view getName() override { return nameof(cheatName); }\
//	\
//}