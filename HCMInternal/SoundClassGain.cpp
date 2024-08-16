#include "pch.h"
#include "SoundClassGain.h"
#include "ModuleHook.h"
#include "MultilevelPointer.h"
#include "SettingsStateAndEvents.h"
#include "RuntimeExceptionHandler.h"
#include "MidhookContextInterpreter.h"
#include "IMessagesGUI.h"
#include "IMCCStateHook.h"
#include "PointerDataStore.h"

class H1SoundClassGainValue final
{
	// same layout as in-game memory
	float targetValue = 1.f; // the value it's aiming to get to
	float currentValue = 1.f; // the value it's currently at
	float interpolationSecondsRemaining = 0.f; // how many seconds left until currentValue becomes targetValue

public:
	// we ignore the interpolation stuff
	void set(float v) { currentValue = v; targetValue = v; }
	const float get() const { return currentValue; }
};

static_assert(sizeof(H1SoundClassGainValue) == 0xC);

class H1SoundClassGainArray final
{
	// mData has same layout as in-game memory
	std::array< H1SoundClassGainValue, 0x33> mData{};
public:
	enum class ClassName
	{
		projectile_impact = 0x0,
		projectile_detonation = 0x1,
		//unused = 0x2,
		//unused = 0x3,
		weapon_fire = 0x4,
		weapon_ready = 0x5,
		weapon_reload = 0x6,
		weapon_empty = 0x7,
		weapon_charge = 0x8,
		weapon_overheat = 0x9,
		weapon_idle = 0xA,
		//unused = 0xB,
		//unused = 0xC,
		object_impacts = 0xD,
		particle_impacts = 0xE,
		slow_particle_impacts = 0xF,
		//unused = 0x10,
		//unused = 0x11,
		unit_footsteps = 0x12,
		unit_dialog = 0x13,
		//unused = 0x14,
		//unused = 0x15,
		vehicle_collision = 0x16,
		vehicle_engine = 0x17,
		//unused = 0x18,
		//unused = 0x19,
		device_door = 0x1A,
		device_force_field = 0x1B,
		device_machinery = 0x1C,
		device_nature = 0x1D,
		device_computers = 0x1E,
		//unused = 0x1F,
		music = 0x20,
		ambient_nature = 0x21,
		ambient_machinery = 0x22,
		ambient_computers = 0x23,
		//unused = 0x24,
		//unused = 0x25,
		//unused = 0x26,
		first_person_damage = 0x27,
		//unused = 0x28,
		//unused = 0x29,
		//unused = 0x2A,
		//unused = 0x2B,
		scripted_dialog_player = 0x2C,
		scripted_effect = 0x2D,
		scripted_dialog_other = 0x2E,
		scripted_dialog_force_unspatialized = 0x2F,
		//unused = 0x30,
		//unused = 0x31,
		game_event = 0x32,
	};

	H1SoundClassGainValue* data() { return mData.data(); }
	H1SoundClassGainValue* getSoundClassByName(ClassName name) {
		static_assert(magic_enum::enum_integer(ClassName::scripted_dialog_player) == 0x2C);
		return &mData.at(magic_enum::enum_integer(name)); }

	std::set<ClassName> getClassesByType(CommonSoundType soundType)
	{
		switch (soundType)
		{
		case CommonSoundType::dialog:
			return { ClassName::scripted_dialog_player, ClassName::scripted_dialog_other, ClassName::scripted_dialog_force_unspatialized, 
				ClassName::unit_dialog };

		case CommonSoundType::ambience:
			return { ClassName::ambient_nature, ClassName::ambient_computers, ClassName::ambient_machinery};

		case CommonSoundType::weapons:
			return { ClassName::projectile_impact, ClassName::projectile_detonation, ClassName::weapon_fire,
			ClassName::weapon_ready, ClassName::weapon_reload, ClassName::weapon_empty,
				ClassName::weapon_charge, ClassName::weapon_overheat, ClassName::weapon_idle,
			};

		case CommonSoundType::vehicles:
			return { ClassName::vehicle_collision, ClassName::vehicle_engine };

		case CommonSoundType::music:
			return { ClassName::music, };

		case CommonSoundType::other:
			return { ClassName::object_impacts, ClassName::particle_impacts, ClassName::slow_particle_impacts,
			ClassName::unit_footsteps, ClassName::device_door, ClassName::device_force_field,
			ClassName::device_machinery, ClassName::device_nature, ClassName::device_computers,
			ClassName::first_person_damage, ClassName::scripted_effect, ClassName::game_event,
			};


		default:
			PLOG_ERROR << "MISSING CASE! " << magic_enum::enum_name< CommonSoundType>(soundType);
			return {}; // empty set
		}
	}
};





template <GameState::Value gameT>
class SoundClassGainImplT : public SoundClassGain::IImpl
{
private:
	// static instance for hook
	static inline SoundClassGainImplT<gameT>* instance = nullptr;
	static inline std::mutex mDestructionGuard{};
	// the real data we pass to the games sound functions
	H1SoundClassGainArray soundClassGainArray{};

	// just some meta data for the get function, not really used.
	std::array<float, magic_enum::enum_count<CommonSoundType>()> soundTypeGains =
	{
		1, 1, 1, 1, 1, 1
	};



	std::shared_ptr<ModuleMidHook> loadSoundClassGainArrayHook;
	std::shared_ptr<MidhookContextInterpreter> loadSoundClassGainArrayFunctionContext;


	// injected services
	std::weak_ptr<IMessagesGUI> messagesWeak;
	std::weak_ptr<IMCCStateHook> mccStateHookWeak;
	std::weak_ptr<SettingsStateAndEvents> settingsWeak;
	std::shared_ptr<RuntimeExceptionHandler> runtimeExceptions;


	// callbacks
	ScopedCallback<eventpp::CallbackList<void(float&)>> dialogGainValueChangedCallback;
	ScopedCallback<eventpp::CallbackList<void(float&)>> ambienceGainValueChangedCallback;
	ScopedCallback<eventpp::CallbackList<void(float&)>> weaponsGainValueChangedCallback;
	ScopedCallback<eventpp::CallbackList<void(float&)>> vehiclesGainValueChangedCallback;
	ScopedCallback<eventpp::CallbackList<void(float&)>> musicGainValueChangedCallback;
	ScopedCallback<eventpp::CallbackList<void(float&)>> otherGainValueChangedCallback;

	ScopedCallback<ToggleEvent> soundGainAdjusterToggleCallback;
	void onSoundClassGainAdjusterToggle(bool& n)
	{
		try
		{
			lockOrThrow(mccStateHookWeak, mccStateHook);

			if (mccStateHook->isGameCurrentlyPlaying(gameT))
			{
				lockOrThrow(messagesWeak, messages);
				messages->addMessage(std::format("Sound class gain adjuster {}.", (n ? "enabled" : "disabled")));
			}



			lockOrThrow(settingsWeak, settings);
			soundTypeSetGain(CommonSoundType::dialog, settings->soundClassGainDialog->GetValue());
			soundTypeSetGain(CommonSoundType::ambience, settings->soundClassGainAmbience->GetValue());
			soundTypeSetGain(CommonSoundType::weapons, settings->soundClassGainWeapons->GetValue());
			soundTypeSetGain(CommonSoundType::vehicles, settings->soundClassGainVehicles->GetValue());
			soundTypeSetGain(CommonSoundType::music, settings->soundClassGainMusic->GetValue());
			soundTypeSetGain(CommonSoundType::other, settings->soundClassGainOther->GetValue());

			loadSoundClassGainArrayHook->setWantsToBeAttached(n);

			
		}
		catch (HCMRuntimeException ex)
		{
			runtimeExceptions->handleMessage(ex);
		}


		
	}

	// main hook
	static void loadSoundClassGainArrayHookFunction(SafetyHookContext& ctx)
	{
		std::unique_lock<std::mutex> lock(mDestructionGuard);
		if (!instance) return;

		LOG_ONCE(PLOG_DEBUG << "loadSoundClassGainArrayHookFunction running for the first time");

		enum class param
		{
			pSoundClassGainArray
		};

		auto* ctxInterpreter = instance->loadSoundClassGainArrayFunctionContext.get();

		LOG_ONCE(PLOG_DEBUG << "alright");
		// Replace games sound-class-gain-array with our own!
		auto pSoundClassGainArray = (H1SoundClassGainValue**)ctxInterpreter->getParameterRef(ctx, (int)param::pSoundClassGainArray);
		LOG_ONCE(PLOG_DEBUG << "yes");
		*pSoundClassGainArray = instance->soundClassGainArray.data();
		LOG_ONCE(PLOG_DEBUG << "okay");
	}


public:
	virtual void soundTypeSetGain(CommonSoundType soundType, float value) override
	{
		soundTypeGains.at(magic_enum::enum_integer(soundType)) = value;

		auto soundClassesForThatType = soundClassGainArray.getClassesByType(soundType);

#ifdef HCM_DEBUG
		PLOG_VERBOSE << "# of sound classes for type of " << magic_enum::enum_name(soundType) << ": " << soundClassesForThatType.size();
#endif

		for (auto soundClass : soundClassesForThatType)
		{
			soundClassGainArray.getSoundClassByName(soundClass)->set(value);
		}
	}

	virtual const float soundTypeGetGain(CommonSoundType soundType) const override
	{ 
		return soundTypeGains.at(magic_enum::enum_integer(soundType));
	}

	SoundClassGainImplT(GameState game, IDIContainer& dicon)
		: dialogGainValueChangedCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->soundClassGainDialog->valueChangedEvent, [this](float& n) { soundTypeSetGain(CommonSoundType::dialog, n); }),
		ambienceGainValueChangedCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->soundClassGainAmbience->valueChangedEvent, [this](float& n) { soundTypeSetGain(CommonSoundType::ambience, n); }),
		weaponsGainValueChangedCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->soundClassGainWeapons->valueChangedEvent, [this](float& n) { soundTypeSetGain(CommonSoundType::weapons, n); }),
		vehiclesGainValueChangedCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->soundClassGainVehicles->valueChangedEvent, [this](float& n) { soundTypeSetGain(CommonSoundType::vehicles, n); }),
		musicGainValueChangedCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->soundClassGainMusic->valueChangedEvent, [this](float& n) { soundTypeSetGain(CommonSoundType::music, n); }),
		otherGainValueChangedCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->soundClassGainOther->valueChangedEvent, [this](float& n) { soundTypeSetGain(CommonSoundType::other, n); }),
		soundGainAdjusterToggleCallback(dicon.Resolve<SettingsStateAndEvents>().lock()->soundClassGainAdjusterToggle->valueChangedEvent, [this](bool& n) { onSoundClassGainAdjusterToggle(n); }),
		messagesWeak(dicon.Resolve<IMessagesGUI>()),
		mccStateHookWeak(dicon.Resolve<IMCCStateHook>()),
		runtimeExceptions(dicon.Resolve<RuntimeExceptionHandler>()),
		settingsWeak(dicon.Resolve<SettingsStateAndEvents>())

	{
		instance = this;

		auto ptr = dicon.Resolve<PointerDataStore>().lock();
		auto loadSoundClassGainArrayFunction = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(loadSoundClassGainArrayFunction), game);
		loadSoundClassGainArrayFunctionContext = ptr->getData<std::shared_ptr<MidhookContextInterpreter>>(nameof(loadSoundClassGainArrayFunctionContext), game);
		loadSoundClassGainArrayHook = ModuleMidHook::make(game.toModuleName(), loadSoundClassGainArrayFunction, loadSoundClassGainArrayHookFunction);

	}

	~SoundClassGainImplT() 
	{ 
		instance = nullptr; 
		loadSoundClassGainArrayHook->setWantsToBeAttached(false);
		std::unique_lock<std::mutex> lock(mDestructionGuard);
	}
};






void SoundClassGain::soundTypeSetGain(CommonSoundType soundType, float value) { return pimpl->soundTypeSetGain(soundType, value); }
const float SoundClassGain::soundTypeGetGain(CommonSoundType soundType) const { return pimpl->soundTypeGetGain(soundType); }
SoundClassGain::SoundClassGain(GameState game, IDIContainer& dicon)
{
	switch (game)
	{
	case GameState::Value::Halo1:
		pimpl = std::make_unique<SoundClassGainImplT<GameState::Value::Halo1>>(game, dicon);
		break;


	default:
		throw HCMInitException("not impl yet");
	}
}