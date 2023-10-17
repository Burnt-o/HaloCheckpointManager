#pragma once
#include "PointerManager.h"
#include "FreeMCCCursor.h"



class ControlServiceContainer
{
public:
	std::optional<std::shared_ptr<FreeMCCCursor>> freeMCCSCursorService = std::nullopt;
	std::optional<HCMInitException> freeMCCSCursorServiceFailure = std::nullopt;

	ControlServiceContainer(std::shared_ptr<PointerManager> ptr)
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

	}



};