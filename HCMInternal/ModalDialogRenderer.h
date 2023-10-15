#pragma once
#include "Vec2.h"
#include "FreeMCCCursor.h"

class IModalDialogRenderer
{
public:
	virtual ~IModalDialogRenderer() = default;
	virtual std::tuple<bool, std::string> showCheckpointDumpNameDialog(std::string defaultName) = 0;
	virtual bool showCheckpointInjectWrongLevelWarningDialog(std::string expectedLevel, std::string observedLevel) = 0;
};


class ModalDialogRenderer : public IModalDialogRenderer
{
private:
	class ModalDialogRendererImpl;
	std::unique_ptr<ModalDialogRendererImpl> pimpl;


public:
	ModalDialogRenderer(std::shared_ptr<RenderEvent> pRenderEvent, std::shared_ptr< FreeMCCCursor> freeCursor);
	~ModalDialogRenderer();
	virtual std::tuple<bool, std::string> showCheckpointDumpNameDialog(std::string defaultName) override;
	virtual bool showCheckpointInjectWrongLevelWarningDialog(std::string expectedLevel, std::string observedLevel) override;
};

