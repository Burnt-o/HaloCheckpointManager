#pragma once
#include "RebindableHotkey.h"
#include "SharedRequestProvider.h"
class HotkeyRebindDialog
{
	std::optional<std::vector<BindingCombo>::iterator> currentSelectedBindingForEditing = std::nullopt; // nullopt means none selected
	std::vector<BindingCombo> workingBindingSet; // binding set that will be set if user clicks save. starts off as original value.

	std::shared_ptr<RebindableHotkey> mHotkeyToChange;
	std::shared_ptr<SharedRequestToken> mHotkeyDisableRequest;

	bool mIsFinished = false;
public:
	HotkeyRebindDialog(std::shared_ptr<RebindableHotkey> hotkeyToChange, std::shared_ptr<SharedRequestToken> hotkeyDisableRequest);

	void render(SimpleMath::Vector2 screenSize);
	bool isFinished() const { return mIsFinished; }
};