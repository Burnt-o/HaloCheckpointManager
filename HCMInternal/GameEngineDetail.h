#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"



// Based on WinterSquires work here https://github.com/WinterSquire/AlphaRing/blob/1ac8f1156c95db9ce70d2f9dd3ffab89827c5b19/src/mcc/CGameEngine.h#L38

// But with my dynamic pointer data shennanigans
// and room for some game templating shennanigans later if I have time to work on it
// Also potentially could add event-hooks for all the stuff (and so on further down the chain)

class ICommonGameEngineHandle
{
public:
	enum CommonEventType {
		EventPause = 0,
		EventResume = 1,
		EventExit = 2,
		EventLoadCheckpoint = 3,
		EventRestart = 4,
		EventLoadSetting = 5,
		EventNewRound = 12,
		EventTeamChange = 18,
		EventReloadSetting = 23,
	};

	struct Item {
		struct Data { __int64 data[2]; };
		struct _SLIST_ENTRY* Next;
		CommonEventType eventType;
		int one;
		Data data;
	};



	virtual ~ICommonGameEngineHandle() = default;
	virtual std::expected<PSLIST_ENTRY, std::string> __fastcall set_event(int event_type, Item::Data* data) = 0;
	virtual std::expected<PSLIST_ENTRY, std::string> __fastcall execute_command(const char* command) = 0;
};

/* if I reverse engineer any functionality specific to a game engine then I'll probably put these back in.

class IH1GameEngineHandle : public ICommonGameEngineHandle
{
public:
	virtual ~IH1GameEngineHandle() = default;
};

class IH2GameEngineHandle : public ICommonGameEngineHandle
{
public:
	virtual ~IH2GameEngineHandle() = default;
};

class IH3GameEngineHandle : public ICommonGameEngineHandle
{
public:
	virtual ~IH3GameEngineHandle() = default;
};

class IODGameEngineHandle : public ICommonGameEngineHandle
{
public:
	virtual ~IODGameEngineHandle() = default;
};

class IHRGameEngineHandle : public ICommonGameEngineHandle
{
public:
	virtual ~IHRGameEngineHandle() = default;
};

class IH4GameEngineHandle : public ICommonGameEngineHandle
{
public:
	virtual ~IH4GameEngineHandle() = default;
};


template <GameState::Value> struct GameStateToHandle;
template <> struct GameStateToHandle<GameState::Value::Halo1> { using type = IH1GameEngineHandle; };
template <> struct GameStateToHandle<GameState::Value::Halo2> { using type = IH2GameEngineHandle; };
template <> struct GameStateToHandle<GameState::Value::Halo3> { using type = IH3GameEngineHandle; };
template <> struct GameStateToHandle<GameState::Value::Halo3ODST> { using type = IODGameEngineHandle; };
template <> struct GameStateToHandle<GameState::Value::HaloReach> { using type = IHRGameEngineHandle; };
template <> struct GameStateToHandle<GameState::Value::Halo4> { using type = IH4GameEngineHandle; };
*/

class GameEngineDetail : public IOptionalCheat
{
	class GameEngineDetailImpl;
	std::unique_ptr<GameEngineDetailImpl> pimpl;
public:

	GameEngineDetail(GameState gameImpl, IDIContainer& dicon);
	~GameEngineDetail();

	std::string_view getName() override { return nameof(GameEngineDetail); }

	std::unique_ptr<ICommonGameEngineHandle> getCommonHandle();

	/*
	template <GameState::Value gameT>
	std::unique_ptr<typename GameStateToHandle<gameT>::type> getSpecificHandle();
	*/


};