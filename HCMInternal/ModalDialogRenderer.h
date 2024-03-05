#pragma once
#include "ControlServiceContainer.h"
#include "GUIServiceInfo.h"

#include "ScopedServiceRequest.h"
#include "IModalDialog.h"

class ModalDialogRenderer
{
private:
	class ModalDialogRendererImpl;
	std::unique_ptr<ModalDialogRendererImpl> pimpl;

public:
	ModalDialogRenderer(std::shared_ptr<RenderEvent> pRenderEvent, std::shared_ptr<ControlServiceContainer> controlServiceContainer, std::shared_ptr<GenericScopedServiceProvider> hotkeyDisabler);
	~ModalDialogRenderer();

	template <typename ret>
	ret showReturningDialog(std::shared_ptr<IModalDialogReturner<ret>>dialogToShow);

	void showVoidDialog(std::shared_ptr<IModalDialogVoid> dialogToShow);

};

