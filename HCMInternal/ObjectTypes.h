#pragma once

enum class CommonObjectType : byte
{
	Biped,
	Vehicle,
	Weapon,
	Equipment,
	GarbageOrTerminal,
	Projectile,
	Scenery,
	Machine,
	Control,
	FailedTypeConversion = 0xFF
};

enum class Halo1ObjectType : byte
{
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
	FailedTypeConversion = 0xFF
};

enum class Halo2ObjectType : byte
{
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
	Creature,
	FailedTypeConversion = 0xFF
};

enum class Halo3ObjectType : byte
{
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
	EffectScenery,
	FailedTypeConversion = 0xFF
};


enum class Halo3ODSTObjectType : byte
{
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
	EffectScenery,
	FailedTypeConversion = 0xFF
};

enum class HaloReachObjectType : byte
{
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
	EffectScenery,
	FailedTypeConversion = 0xFF
};

enum class Halo4ObjectType : byte
{
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
	Spawner,
	FailedTypeConversion = 0xFF
};