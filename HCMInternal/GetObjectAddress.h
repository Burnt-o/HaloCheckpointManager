#pragma once
#include "IOptionalCheat.h"
#include "DIContainer.h"
#include "GameState.h"
#include "Datum.h"

enum class CommonObjectType
{
	Null = -1,
	Biped,
	Vehicle,
	Weapon,
	Equipment,
	GarbageOrTerminal,
	Projectile,
	Scenery,
	Machine,
	Control,
};

enum class Halo1ObjectType
{
	Null = -1,
	Biped,
	Vehicle,
	Weapon,
	Equipment,
	Garbage,
	Projectile,
	Scenery,
	Machine,
	Control,
	LightFixture,
	Placeholder,
	SoundScenery
};

enum class Halo2ObjectType
{
	Null = -1,
	Biped,
	Vehicle,
	Weapon,
	Equipment,
	Garbage,
	Projectile,
	Scenery,
	Machine,
	Control,
	LightFixture,
	Placeholder,
	SoundScenery,
	Crate,
	Creature
};

enum class Halo3ObjectType
{
	Null = -1,
	Biped,
	Vehicle,
	Weapon,
	Equipment,
	Garbage,
	Projectile,
	Scenery,
	Machine,
	Control,
	SoundScenery,
	Crate,
	Creature,
	Giant,
	EffectScenery
};


enum class Halo3ODSTObjectType
{
	Null = -1,
	Biped,
	Vehicle,
	Weapon,
	Equipment,
	Garbage,
	Projectile,
	Scenery,
	Machine,
	Control,
	SoundScenery,
	Crate,
	Creature,
	Giant,
	EffectScenery
};

enum class HaloReachObjectType
{
	Null = -1,
	Biped,
	Vehicle,
	Weapon,
	Equipment,
	Terminal,
	Projectile,
	Scenery,
	Machine,
	Control,
	SoundScenery,
	Crate,
	Creature,
	Giant,
	EffectScenery
};

enum class Halo4ObjectType
{
	Null = -1,
	Biped,
	Vehicle,
	Weapon,
	Equipment,
	Terminal,
	Projectile,
	Scenery,
	Machine,
	Control,
	Dispenser,
	SoundScenery,
	Crate,
	Creature,
	Giant,
	EffectScenery,
	Spawner
};


class GetObjectAddress : public IOptionalCheat
{
	public:
		class GetObjectAddressImpl;
	private:
		std::unique_ptr<GetObjectAddressImpl> pimpl;
	
	public:
		GetObjectAddress(GameState game, IDIContainer& dicon);
		~GetObjectAddress();
	
		uintptr_t getObjectAddress(Datum entityDatum, std::set < CommonObjectType> expectedObjectTypes); 
		template <typename TemplatedObjectType>
		uintptr_t getObjectAddress(Datum entityDatum, std::set < TemplatedObjectType > expectedObjectTypes); 

		uintptr_t getObjectAddress(Datum entityDatum, CommonObjectType expectedObjectType) {
			return getObjectAddress(entityDatum, std::set<CommonObjectType>{expectedObjectType});
		}
		template <typename TemplatedObjectType>
		uintptr_t getObjectAddress(Datum entityDatum, TemplatedObjectType expectedObjectType)
		{
			return getObjectAddress(entityDatum, std::set<TemplatedObjectType>{expectedObjectType});
		}

		uintptr_t getObjectAddress(Datum entityDatum)
		{
			return getObjectAddress(entityDatum, std::set<CommonObjectType>{});
		}
	
		virtual std::string_view getName() override { return nameof(GetObjectAddress); }
};
