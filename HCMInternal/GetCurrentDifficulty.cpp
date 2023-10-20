#include "pch.h"
#include "GetCurrentDifficulty.h"
#include "MultilevelPointer.h"
#include "PointerManager.h"
class GetCurrentDifficulty::GetCurrentDifficultyImpl
{
private:
	//data
	std::shared_ptr<MultilevelPointer> currentDifficulty;

public:
	GetCurrentDifficultyImpl(GameState game, IDIContainer& dicon)
	{
		currentDifficulty = dicon.Resolve<PointerManager>()->getData<std::shared_ptr<MultilevelPointer>>(nameof(currentDifficulty), game);
	}

	DifficultyEnum getCurrentDifficulty()
	{
		DifficultyEnum readDifficulty;
		if (!currentDifficulty->readData(&readDifficulty)) throw HCMRuntimeException("Could not read readDifficulty");
		return std::move(readDifficulty);
	}

	~GetCurrentDifficultyImpl()
	{
		PLOG_DEBUG << "~GetCurrentDifficultyImpl";
	}
};

GetCurrentDifficulty::GetCurrentDifficulty(GameState game, IDIContainer& dicon) : pimpl(std::make_unique<GetCurrentDifficulty::GetCurrentDifficultyImpl>(game, dicon)) {}
GetCurrentDifficulty::~GetCurrentDifficulty() { PLOG_DEBUG << "~" << getName(); }
DifficultyEnum GetCurrentDifficulty::getCurrentDifficulty() { return pimpl->getCurrentDifficulty(); }