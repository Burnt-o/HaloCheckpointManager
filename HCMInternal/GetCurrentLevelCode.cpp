#include "pch.h"
#include "GetCurrentLevelCode.h"
#include "MultilevelPointer.h"
#include "PointerDataStore.h"
class GetCurrentLevelCode::GetCurrentLevelCodeImpl
{
private:
	//data
	std::shared_ptr<MultilevelPointer> currentLevelCode;

public:
	GetCurrentLevelCodeImpl(GameState game, IDIContainer& dicon)
	{
		currentLevelCode = dicon.Resolve<PointerDataStore>().lock()->getData<std::shared_ptr<MultilevelPointer>>(nameof(currentLevelCode), game);
	}

	std::string getCurrentLevelCode()
	{
		std::string readLevelCode;
		if (!currentLevelCode->readData(&readLevelCode)) throw HCMRuntimeException("Could not read currentLevelCode");
		return std::move(readLevelCode);
	}

	~GetCurrentLevelCodeImpl()
	{
		PLOG_DEBUG << "~GetCurrentLevelCodeImpl";
	}
};

GetCurrentLevelCode::GetCurrentLevelCode(GameState game, IDIContainer& dicon) : pimpl(std::make_unique<GetCurrentLevelCode::GetCurrentLevelCodeImpl>(game, dicon)) {}
GetCurrentLevelCode::~GetCurrentLevelCode() { PLOG_DEBUG << "~" << getName(); }
std::string GetCurrentLevelCode::getCurrentLevelCode() { return pimpl->getCurrentLevelCode(); }