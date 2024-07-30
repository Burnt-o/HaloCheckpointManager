#pragma once
#include "PointerDataStore.h"
#include "FreeMCCCursor.h"
#include "BlockGameInput.h"
#include "PauseGame.h"


class ControlServiceContainer
{
public:
	std::optional<std::shared_ptr<FreeMCCCursor>> freeMCCSCursorService = std::nullopt;
	std::optional<HCMInitException> freeMCCSCursorServiceFailure = std::nullopt;

	std::optional<std::shared_ptr<BlockGameInput>> blockGameInputService = std::nullopt;
	std::optional<HCMInitException> blockGameInputServiceFailure = std::nullopt;

	std::optional<std::shared_ptr<PauseGame>> pauseGameService = std::nullopt;
	std::optional<HCMInitException> pauseGameServiceFailure = std::nullopt;

	std::shared_ptr< TokenScopedServiceProvider> hotkeyDisabler;

	ControlServiceContainer(std::shared_ptr<PointerDataStore> ptr, std::shared_ptr< TokenScopedServiceProvider> hotkeyDisabler)
		: hotkeyDisabler(hotkeyDisabler)
	{
		try
		{
			freeMCCSCursorService = std::make_optional<std::shared_ptr<FreeMCCCursor>>(std::make_shared<FreeMCCCursor>(ptr));
		}
		catch (HCMInitException ex)
		{
			PLOG_ERROR << "Failed to create freeMCCCursorService, " << ex.what();
			freeMCCSCursorServiceFailure = ex;
		}

		try
		{
			blockGameInputService = std::make_optional<std::shared_ptr<BlockGameInput>>(std::make_shared<BlockGameInput>(ptr));
		}
		catch (HCMInitException ex)
		{
			PLOG_ERROR << "Failed to create blockGameInputService, " << ex.what();
			blockGameInputServiceFailure = ex;
		}

		try
		{
			pauseGameService = std::make_optional<std::shared_ptr<PauseGame>>(std::make_shared<PauseGame>(ptr));
		}
		catch (HCMInitException ex)
		{
			PLOG_ERROR << "Failed to create pauseGameService, " << ex.what();
			pauseGameServiceFailure = ex;
		}
	}



};