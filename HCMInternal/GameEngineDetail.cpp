#include "pch.h"
#include "GameEngineDetail.h"
#include "MultilevelPointer.h"
#include "DynamicStructFactory.h"

enum class gameEngineCommonFunctionTableFields { set_event, execute_command };

class CommonEngineData
{
	std::shared_ptr<MultilevelPointer> gameEnginePointer;
	std::shared_ptr<MultilevelPointer> gameEngineFunctionTablePointer;
	std::shared_ptr<DynamicStruct<gameEngineCommonFunctionTableFields>> gameEngineCommonFunctionTableData;

	std::expected<uintptr_t, std::string> updatePointersAndGetGameEngineAddress()
	{
		{
			uintptr_t gameEngineFunctionTableAddress;
			if (!gameEngineFunctionTablePointer->resolve(&gameEngineFunctionTableAddress))
				return std::unexpected(std::format("Unable to resolve gameEngineFunctionTableAddress, error: {}", MultilevelPointer::GetLastError()));

			gameEngineCommonFunctionTableData->currentBaseAddress = gameEngineFunctionTableAddress;
		}

		{
			uintptr_t gameEngineAddress;
			if (!gameEnginePointer->resolve(&gameEngineAddress))
				return std::unexpected(std::format("Unable to resolve gameEngineAddress, error: {}", MultilevelPointer::GetLastError()));
			else
				return gameEngineAddress;
		}

	}
public:
	CommonEngineData(
		std::shared_ptr<MultilevelPointer> gameEnginePointer,
		std::shared_ptr<MultilevelPointer> gameEngineFunctionTablePointer,
		std::shared_ptr<DynamicStruct<gameEngineCommonFunctionTableFields>> gameEngineCommonFunctionTableData)
		:
		gameEnginePointer(gameEnginePointer),
		gameEngineFunctionTablePointer(gameEngineFunctionTablePointer),
		gameEngineCommonFunctionTableData(gameEngineCommonFunctionTableData)
	{
	}

	

	std::expected<PSLIST_ENTRY, std::string> __fastcall set_event(int event_type, ICommonGameEngineHandle::Item::Data* data)
	{
		auto eng = updatePointersAndGetGameEngineAddress();
		if (!eng)
			return std::unexpected(eng.error());

		PLOG_VERBOSE << "CommonGameEngineHandleImpl->set_event";

		typedef PSLIST_ENTRY(*engine_set_event)(uintptr_t pGameEngine, int event_type, ICommonGameEngineHandle::Item::Data* data);

		engine_set_event f_set_event = *gameEngineCommonFunctionTableData->field<engine_set_event>(gameEngineCommonFunctionTableFields::set_event);
		PLOG_VERBOSE << "f_set_event ptr: 0x" << std::hex << f_set_event;
		
		return f_set_event(eng.value(), event_type, data);
	}

	std::expected<PSLIST_ENTRY, std::string> __fastcall execute_command(const char* command)
	{
		auto eng = updatePointersAndGetGameEngineAddress();
		if (!eng)
			return std::unexpected(eng.error());

		typedef PSLIST_ENTRY(*engine_execute_command)(uintptr_t pGameEngine, const char* command);

		auto f_execute_command = *gameEngineCommonFunctionTableData->field<engine_execute_command>(gameEngineCommonFunctionTableFields::execute_command);
		return f_execute_command(eng.value(), command);
	}
};

class CommonGameEngineHandleImpl : public ICommonGameEngineHandle
{
	std::shared_ptr<CommonEngineData> commonEngineData;
public:
	CommonGameEngineHandleImpl(std::shared_ptr<CommonEngineData> commonEngineData) : commonEngineData(commonEngineData) { }
	virtual std::expected<PSLIST_ENTRY, std::string> set_event(int event_type, Item::Data* data) override
	{
		PLOG_VERBOSE << "CommonGameEngineHandleImpl->set_event";
		return commonEngineData->set_event(event_type, data);
	}
	virtual std::expected<PSLIST_ENTRY, std::string> __fastcall execute_command(const char* command) override
	{

		return commonEngineData->execute_command(command);
	}
};




class GameEngineDetail::GameEngineDetailImpl
{
private:
	std::shared_ptr< CommonEngineData> commonEngineData;
public:
	GameEngineDetailImpl(GameState gameImpl, IDIContainer& dicon)
	{
		auto ptr = dicon.Resolve<PointerDataStore>().lock();
		auto gameEnginePointer = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(gameEnginePointer), gameImpl);
		auto gameEngineFunctionTablePointer = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(gameEngineFunctionTablePointer), gameImpl);
		auto gameEngineCommonFunctionTableData = DynamicStructFactory::make<gameEngineCommonFunctionTableFields>(ptr, gameImpl);
		commonEngineData = std::make_shared<CommonEngineData>(gameEnginePointer, gameEngineFunctionTablePointer, gameEngineCommonFunctionTableData);

	}
	std::unique_ptr<ICommonGameEngineHandle> getCommonHandle()
	{
		PLOG_VERBOSE << "constructing common handle";
		return std::make_unique<CommonGameEngineHandleImpl>(commonEngineData);
	}

	/*
	template <GameState::Value gameT>
	std::unique_ptr<typename GameStateToHandle<gameT>::type> getSpecificHandle();

	template<>
	std::unique_ptr<IH1GameEngineHandle> getSpecificHandle<GameState::Value::Halo1>()
	{
		return std::make_unique<H1GameEngineHandleImpl>("some h1 func", "some common func (but from h1)");
	}

	template<>
	std::unique_ptr<IH2GameEngineHandle> getSpecificHandle<GameState::Value::Halo2>()
	{
		return std::make_unique<H2GameEngineHandleImpl>("some h2 func", "some common func (but from h2)");
	}
	*/
};


GameEngineDetail::GameEngineDetail(GameState gameImpl, IDIContainer& dicon)
{ 
	pimpl = std::make_unique<GameEngineDetail::GameEngineDetailImpl>(gameImpl, dicon);

}

GameEngineDetail::~GameEngineDetail() = default;

std::unique_ptr<ICommonGameEngineHandle> GameEngineDetail::getCommonHandle() { return pimpl->getCommonHandle(); }

//template <> std::unique_ptr<typename GameStateToHandle<GameState::Value::Halo1>::type> GameEngineManager::getSpecificHandle<GameState::Value::Halo1>() { return pimpl->getSpecificHandle<GameState::Value::Halo1>(); }
//template <> std::unique_ptr<typename GameStateToHandle<GameState::Value::Halo2>::type> GameEngineManager::getSpecificHandle<GameState::Value::Halo2>() { return pimpl->getSpecificHandle<GameState::Value::Halo2>(); }
//template <> std::unique_ptr<typename GameStateToHandle<GameState::Value::Halo3>::type> GameEngineManager::getSpecificHandle<GameState::Value::Halo3>() { return pimpl->getSpecificHandle<GameState::Value::Halo3>(); }
//template <> std::unique_ptr<typename GameStateToHandle<GameState::Value::Halo3ODST>::type> GameEngineManager::getSpecificHandle<GameState::Value::Halo3ODST>() { return pimpl->getSpecificHandle<GameState::Value::Halo3ODST>(); }
//template <> std::unique_ptr<typename GameStateToHandle<GameState::Value::HaloReach>::type> GameEngineManager::getSpecificHandle<GameState::Value::HaloReach>() { return pimpl->getSpecificHandle<GameState::Value::HaloReach>(); }
//template <> std::unique_ptr<typename GameStateToHandle<GameState::Value::Halo4>::type> GameEngineManager::getSpecificHandle<GameState::Value::Halo4>() { return pimpl->getSpecificHandle<GameState::Value::Halo4>(); }
