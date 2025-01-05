#include "pch.h"
#include "ShieldInputPrinter.h"
#include "DynamicStructFactory.h"
#include "PointerDataStore.h"
#include "IMakeOrGetCheat.h"
#include "GetPlayerDatum.h"
#include "GetObjectAddress.h"
#include "EnumClass.h"
#include "GameTickEventHook.h"
#include "IMCCStateHook.h"
#include "ObjectTableRange.h"
#include "IMessagesGUI.h"
#include "RuntimeExceptionHandler.h"
#include "SettingsStateAndEvents.h"
#include "GetObjectTagName.h"
#include "GetObjectPhysics.h"

enum class Input
{
	None = 0,
	Forward = 0x1 << 0,
	Backward = 0x1 << 1,
	Left = 0x1 << 2,
	Right = 0x1 << 3,
	Crouch = 0x1 << 4,
	Jump = 0x1 << 5,

};
enableEnumClassBitmask(Input); // Activate bitmask operators
typedef bitmask<Input> InpMask;

bool contains(const InpMask& lhs, Input rhs)
{
	return (lhs & InpMask(rhs)).operator bool();
}

class ShieldInputPrinter::ShieldInputPrinterImpl
{
private:

	// injected services
	std::weak_ptr< IMCCStateHook> mccStateHookWeak;
	std::weak_ptr< GetPlayerDatum> getPlayerDatumWeak;
	std::weak_ptr< GetObjectAddress> getObjectAddressWeak;
	std::weak_ptr< GameTickEventHook> gameTickEventHookWeak;
	std::weak_ptr< GetObjectTagName> getObjectTagNameWeak;
	std::weak_ptr< ObjectTableRange> objectTableRangeWeak;
	std::weak_ptr< SettingsStateAndEvents> settingsWeak;
	std::weak_ptr< GetObjectPhysics> getObjectPhysicsWeak;
	std::weak_ptr< IMessagesGUI> messagesWeak;
	std::shared_ptr< RuntimeExceptionHandler> runtimeExceptions;

	// callbacks
	std::unique_ptr<ScopedCallback<eventpp::CallbackList<void(uint32_t)>>> mGameTickEventCallback;
	ScopedCallback<ToggleEvent> shieldInputPrinterToggleCallback;
	ScopedCallback< eventpp::CallbackList<void(const MCCState&)>> MCCStateChangedCallback;

	// data
	GameState mGame;
	std::optional<Datum> lockedShield;
	InpMask lastInputs = Input::None;

	enum class bipedDataFields { crouchJumpInput, movementInput };
	std::shared_ptr<DynamicStruct<bipedDataFields>> playerDataStruct;

	enum class deployableShieldDataFields { shieldCooldown };
	std::shared_ptr<DynamicStruct<deployableShieldDataFields>> shieldDataStruct;


	const InpMask getPlayerInputState(std::shared_ptr<DynamicStruct<bipedDataFields>>& player)
	{
		InpMask out = Input::None;
		auto movementInput = *player->field<SimpleMath::Vector2>(bipedDataFields::movementInput);

		if (movementInput.x == 1.f)
			out |= Input::Forward; 
		else if (movementInput.x == -1.f)
			out |= Input::Backward; 

		if (movementInput.y == 1.f)
			out |= Input::Left; 
		else if (movementInput.y == -1.f)
			out |= Input::Right; 

		auto crouchJumpInput = *player->field<byte>(bipedDataFields::crouchJumpInput);

		// test crouch (bit 0)
		if (crouchJumpInput & (1 << 0))
			out |= Input::Crouch;

		// test jump (bit 1)
		if (crouchJumpInput & (1 << 1))
			out |= Input::Jump;

		return out;
	}

	std::optional<Datum> findNearestShield()
	{
		lockOrThrow(objectTableRangeWeak, objectTableRange);
		auto objectRange = objectTableRange->getObjectTableRange();

		lockOrThrow(getObjectTagNameWeak, getObjectTagName);


		struct ShieldInfo
		{
			ObjectInfo objectInfo;
			SimpleMath::Vector3 shieldPosition;
			float distanceToPlayer;
		};
		std::vector<ShieldInfo> allShieldObjects;

		lockOrThrow(getObjectPhysicsWeak, getObjectPhysics);
		lockOrThrow(getPlayerDatumWeak, getPlayerDatum);
		auto playerDatum = getPlayerDatum->getPlayerDatum();

		if (playerDatum.isNull())
			return std::nullopt;

		auto playerPosition = *getObjectPhysics->getObjectPosition(playerDatum);

		// find all shield objects
		for (auto& object : objectRange)
		{
			// shields are scenery objects
			if (object.objectType != CommonObjectType::Scenery) 
				continue;

			// check tag name matches
			std::string tagName = getObjectTagName->getObjectTagName(object.objectDatum);
			if (tagName.ends_with("c_field_generator") == false) 
				continue;

			auto shieldPosition = *getObjectPhysics->getObjectPosition(object.objectDatum);
			float distanceToPlayer = SimpleMath::Vector3::Distance(playerPosition, shieldPosition);

			allShieldObjects.emplace_back(ShieldInfo(object, shieldPosition, distanceToPlayer));
		}

		if (allShieldObjects.empty()) return std::nullopt;

		// sort shield objects by distance to player
		std::ranges::sort(allShieldObjects, [](const auto& lhs, const auto& rhs) {

			return lhs.distanceToPlayer < rhs.distanceToPlayer;
			});
		

		// if closest shield over 1.5 units away, return null
		if (SimpleMath::Vector3::Distance(playerPosition, allShieldObjects.at(0).shieldPosition) > 1.5f)
		{
			return std::nullopt;
		}


		// return datum of closest shield object
		lockOrThrow(messagesWeak, messages);
		messages->addMessage(std::format("Input printing for Shield at position: {}", vec3ToStringSetPrecision(allShieldObjects.at(0).shieldPosition, 3)));

		return allShieldObjects.at(0).objectInfo.objectDatum;
	}

	void onGameTickEvent(uint32_t& currentTick)
	{
		/* Get shield object. calculate shield-on-tick.
Get player object. Get inputs and compare to last tick inputs.
Print message ("new input pressed at T-x ticks to shield-on-tick") as appopiate.
*/


		try
		{

			// check for reverts. if revert, refind shieldDatum.
			static uint32_t lastTick = 0;
			if (currentTick < lastTick || (lockedShield.has_value() == false && currentTick % 10 == 0))
			{
				lockedShield = findNearestShield();
			}
			lastTick = currentTick;

			if (lockedShield.has_value() == false) return;

			// check shield not too far away
			lockOrThrow(getPlayerDatumWeak, getPlayerDatum);
			lockOrThrow(getObjectPhysicsWeak, getObjectPhysics);

			auto playerDatum = getPlayerDatum->getPlayerDatum();
			if (playerDatum.isNull()) // commonly happens if the player dies
				return;

			
			auto playerPosition = *getObjectPhysics->getObjectPosition(playerDatum);
			auto shieldPosition = *getObjectPhysics->getObjectPosition(lockedShield.value());

			if (SimpleMath::Vector3::Distance(playerPosition, shieldPosition) > 1.5f)
			{
				lockedShield = std::nullopt;
				return;
			}



			// get shield object
			lockOrThrow(getObjectAddressWeak, getObjectAddress);
			shieldDataStruct->currentBaseAddress = getObjectAddress->getObjectAddress(lockedShield.value());

			auto shieldCooldown = *shieldDataStruct->field<uint16_t>(deployableShieldDataFields::shieldCooldown);

			LOG_ONCE_CAPTURE(PLOG_DEBUG << "shield object: " << p, p = shieldDataStruct->currentBaseAddress);
			LOG_ONCE_CAPTURE(PLOG_DEBUG << "shield cooldown: " << c, c = shieldCooldown);

			// no need to track inputs if shield still up
			if (shieldCooldown == 0 || shieldCooldown == 210)
				return;


			playerDataStruct->currentBaseAddress = getObjectAddress->getObjectAddress(getPlayerDatum->getPlayerDatum());

			InpMask currentInputs = getPlayerInputState(playerDataStruct);

			// if inputs haven't changed since last tick, no need to print anything, we're done.
			if (currentInputs == lastInputs)
				return;


			// ipnuts are different! We want to find what inputs are *newly* pressed this tick.
			// start by xoring lastInputs with currentInputs, then AND the result with currentInputs.
			InpMask newlyPressedInputs = (lastInputs ^ currentInputs) & currentInputs;

			// update lastInputs
			lastInputs = currentInputs;

			// player only released some inputs, we don't care
			if (newlyPressedInputs.value == 0)
				return;

			lockOrThrow(messagesWeak, messages);

			// print message corrosponding to each potential input
			if (contains(newlyPressedInputs, Input::Jump))
				messages->addMessage(std::format("Pressed Jump at T-{}", shieldCooldown));

			if (contains(newlyPressedInputs, Input::Crouch))
				messages->addMessage(std::format("Pressed Crouch at T-{}", shieldCooldown));

			if (contains(newlyPressedInputs, Input::Forward))
				messages->addMessage(std::format("Pressed Forward at T-{}", shieldCooldown));

			if (contains(newlyPressedInputs, Input::Left))
				messages->addMessage(std::format("Pressed Left at T-{}", shieldCooldown));

			if (contains(newlyPressedInputs, Input::Right))
				messages->addMessage(std::format("Pressed Right at T-{}", shieldCooldown));

			if (contains(newlyPressedInputs, Input::Backward))
				messages->addMessage(std::format("Pressed Backward at T-{}", shieldCooldown));

		}
		catch (HCMRuntimeException ex)
		{
			lockOrThrow(settingsWeak, settings);
			settings->shieldInputPrinterToggle->GetValueDisplay() = false;
			settings->shieldInputPrinterToggle->UpdateValueWithInput();
			runtimeExceptions->handleMessage(ex);
		}
	}

	void onMCCStateChanged()
	{
		try
		{
			lockOrThrow(settingsWeak, settings);
			lockOrThrow(mccStateHookWeak, mccStateHook);
			bool shouldDisable = settings->shieldInputPrinterToggle->GetValue() && !mccStateHook->isGameCurrentlyPlaying(mGame);
		
			if (shouldDisable)
			{
				settings->shieldInputPrinterToggle->GetValueDisplay() = false;
				settings->shieldInputPrinterToggle->UpdateValueWithInput();
			}

		}
		catch (HCMRuntimeException ex)
		{
			lockOrThrow(settingsWeak, settings);
			settings->shieldInputPrinterToggle->GetValueDisplay() = false;
			settings->shieldInputPrinterToggle->UpdateValueWithInput();
			runtimeExceptions->handleMessage(ex);
		}
	}

	void onSettingChanged()
	{
		try
		{
			lockOrThrow(settingsWeak, settings);
			lockOrThrow(mccStateHookWeak, mccStateHook);
			bool shouldEnable = settings->shieldInputPrinterToggle->GetValue() && mccStateHook->isGameCurrentlyPlaying(mGame);
			lockOrThrow(messagesWeak, messages);

			if (shouldEnable)
			{

				lockOrThrow(gameTickEventHookWeak, gameTickEventHook);
				mGameTickEventCallback = gameTickEventHook->getGameTickEvent()->subscribe([this](uint32_t i) {onGameTickEvent(i); });
				if (mccStateHook->isGameCurrentlyPlaying(mGame))
					messages->addMessage("Shield Input Printer enabled.");
			}
			else
			{
				mGameTickEventCallback.reset();
				if (mccStateHook->isGameCurrentlyPlaying(mGame))
					messages->addMessage("Shield Input Printer disabled.");
			}

		}
		catch (HCMRuntimeException ex)
		{
			lockOrThrow(settingsWeak, settings);
			settings->shieldInputPrinterToggle->GetValueDisplay() = false;
			settings->shieldInputPrinterToggle->UpdateValueWithInput();
			runtimeExceptions->handleMessage(ex);
		}
	}

public:
	ShieldInputPrinterImpl(GameState game, IDIContainer& dicon)
		:
		mGame(game),
		mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
		getPlayerDatumWeak(resolveDependentCheat(GetPlayerDatum)),
		getObjectAddressWeak(resolveDependentCheat(GetObjectAddress)),
		gameTickEventHookWeak(resolveDependentCheat(GameTickEventHook)),
		getObjectTagNameWeak(resolveDependentCheat(GetObjectTagName)),
		objectTableRangeWeak(resolveDependentCheat(ObjectTableRange)),
		getObjectPhysicsWeak(resolveDependentCheat(GetObjectPhysics)),
		messagesWeak(dicon.Resolve<IMessagesGUI>()),
		settingsWeak(dicon.Resolve<SettingsStateAndEvents>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
		shieldInputPrinterToggleCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->shieldInputPrinterToggle->valueChangedEvent, [this](bool& n) { onSettingChanged(); }),
		MCCStateChangedCallback(dicon.Resolve<IMCCStateHook>().lock()->getMCCStateChangedEvent(), [this](const MCCState& n) { onMCCStateChanged(); })
	{
		auto ptr = dicon.Resolve<PointerDataStore>().lock();
		playerDataStruct = DynamicStructFactory::make<bipedDataFields>(ptr, game);
		shieldDataStruct = DynamicStructFactory::make<deployableShieldDataFields>(ptr, game);
	}
};



ShieldInputPrinter::ShieldInputPrinter(GameState game, IDIContainer& dicon)
{
	pimpl = std::make_unique< ShieldInputPrinterImpl>(game, dicon);
}

ShieldInputPrinter::~ShieldInputPrinter() = default;