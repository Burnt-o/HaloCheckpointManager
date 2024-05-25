#include "pch.h"
#include "PlayerPositionToClipboard.h"
#include "Datum.h"
#include "ScopedCallback.h"
#include "SettingsStateAndEvents.h"

#include "RuntimeExceptionHandler.h"
#include "IMakeOrGetCheat.h"
#include "GetPlayerDatum.h"
#include "GetObjectPhysics.h"
#include "IMessagesGUI.h"
#include "IMCCStateHook.h"
#include "imgui.h"
class PlayerPositionToClipboard::PlayerPositionToClipboardImpl
{
private:
	GameState mGame;

	ScopedCallback<ActionEvent> playerPositionToClipboardEventCallback;


	// injected services
	std::weak_ptr< GetPlayerDatum> getPlayerDatumWeak;
	std::weak_ptr< GetObjectPhysics> getObjectPhysicsWeak;
	std::weak_ptr<IMessagesGUI> messagesGUIWeak;
	std::weak_ptr< IMCCStateHook> mccStateHookWeak;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;


	void onPlayerPositionToClipboardEventCallback()
	{
		try
		{
			lockOrThrow(mccStateHookWeak, mccStateHook);
			if (mccStateHook->isGameCurrentlyPlaying(mGame) == false) return;

			lockOrThrow(getPlayerDatumWeak, getPlayerDatum);
			lockOrThrow(getObjectPhysicsWeak, getObjectPhysics);
			lockOrThrow(messagesGUIWeak, messagesGUI);


			auto playerDatum = getPlayerDatum->getPlayerDatum();
			if (playerDatum.isNull()) throw HCMRuntimeException("Null player datum!");

			auto playerPosition = getObjectPhysics->getObjectPosition(playerDatum);

			if (IsBadReadPtr(playerPosition, sizeof(SimpleMath::Vector3))) throw HCMRuntimeException(std::format("Bad player pos read at {:x}", (uintptr_t)playerPosition));

			messagesGUI->addMessage(std::format("Copying player position to clipboard:\n{}", vec3ToStringSetPrecision(*playerPosition)));

			auto exactString = vec3ToString(*playerPosition);

			pugi::xml_document doc;
			pugi::xml_node node = doc.append_child("");
			auto child = node.append_child("position");

			child.text().set(exactString.c_str());

			std::ostringstream ss;
			node.first_child().print(ss);
			ImGui::SetClipboardText(ss.str().c_str());
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}
	}
public:

	PlayerPositionToClipboardImpl(GameState game, IDIContainer& dicon)
		: 
		mGame(game),
		mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>().lock()),
		getPlayerDatumWeak(resolveDependentCheat(GetPlayerDatum)),
		getObjectPhysicsWeak(resolveDependentCheat(GetObjectPhysics)),
		messagesGUIWeak(dicon.Resolve<IMessagesGUI>().lock()),
		playerPositionToClipboardEventCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->playerPositionToClipboardEvent, [this]() {onPlayerPositionToClipboardEventCallback(); })
	{}

};







PlayerPositionToClipboard::PlayerPositionToClipboard(GameState gameImpl, IDIContainer& dicon)
{
	pimpl = std::make_unique< PlayerPositionToClipboardImpl>(gameImpl, dicon);
}

PlayerPositionToClipboard::~PlayerPositionToClipboard() = default;