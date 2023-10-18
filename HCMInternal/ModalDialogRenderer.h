#pragma once
#include "Vec2.h"
#include "ControlServiceContainer.h"
#include "GUIServiceInfo.h"

class IModalDialogRenderer
{
public:
	virtual ~IModalDialogRenderer() = default;
	virtual std::tuple<bool, std::string> showCheckpointDumpNameDialog(std::string defaultName) = 0; // This call blocks
	virtual bool showCheckpointInjectWrongLevelWarningDialog(std::string expectedLevel, std::string observedLevel) = 0; // This call blocks
	virtual void showFailedOptionalCheatServices() = 0; // This call blocks
};


class ModalDialogRenderer : public IModalDialogRenderer
{
private:
	class ModalDialogRendererImpl;
	std::unique_ptr<ModalDialogRendererImpl> pimpl;

	ScopedCallback<ActionEvent> mShowGUIFailuresEventCallback;
	std::shared_ptr<GUIServiceInfo> mGUIFailures;

public:
	ModalDialogRenderer(std::shared_ptr<RenderEvent> pRenderEvent, std::shared_ptr<ControlServiceContainer> controlServiceContainer, std::shared_ptr<ActionEvent> showFailsEvent, std::shared_ptr<GUIServiceInfo> guiFailures);
	~ModalDialogRenderer();
	virtual std::tuple<bool, std::string> showCheckpointDumpNameDialog(std::string defaultName) override; // This call blocks
	virtual bool showCheckpointInjectWrongLevelWarningDialog(std::string expectedLevel, std::string observedLevel) override; // This call blocks
	virtual void showFailedOptionalCheatServices() override; // This call blocks

};

