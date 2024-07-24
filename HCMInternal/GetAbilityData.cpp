#include "pch.h"
#include "GetAbilityData.h"
#include "PointerDataStore.h"
#include "DynamicStructFactory.h"
#include "MultilevelPointer.h"
#include "IMakeOrGetCheat.h"

#include "GetPlayerDatum.h"
#include "GetObjectAddress.h"
#include "TagTableRange.h"
/*
For Reach:
	Get player datum
	get player object from datum
	lookup currently equiped eqip object (+0x10 into player)
	get eqip object from eqip datum
	ability % is at eqip obj + 0x1EC
	cooldown (in ticks) is at eqip obj + 0x1DC (do we convert to percent out of 239?)

	(what if eqip isn't ability? what if player has no ability equipped?)

For Halo 4:
	Very similiar to reach, except the player has a permanent eqip slot called
	"hero_assist".
	datum to this eqip obj is at player + 0x20.
	ability % is at eqip obj + 0x544
	cooldown in ticks at +0x520. unlike reach where it's zero while recharging, it is 0xFFFFFFFE in h4.
*/



template <class bipedDataFields, bipedDataFields equipmentDatumField, bipedDataFields heroDatumField>
class GetAbilityDataImpl : public IGetAbilityData
{
private:
	// injected services
	std::weak_ptr< GetPlayerDatum> getPlayerDatumWeak;
	std::weak_ptr< GetObjectAddress> getObjectAddressWeak;
	std::weak_ptr< TagTableRange> tagTableRangeWeak;

	std::shared_ptr<DynamicStruct<bipedDataFields>> playerDataStruct;

	enum class equipmentDataFields { abilityPercent, cooldownTicks, tagDatum };
	std::shared_ptr<DynamicStruct<equipmentDataFields>> equipmentDataStruct;

	enum class equipmentTagDataFields { abilityMaxCooldownF };
	std::shared_ptr<DynamicStruct<equipmentTagDataFields>> equipmentTagDataStruct;

public:
	GetAbilityDataImpl(GameState game, IDIContainer& dicon)
		: getPlayerDatumWeak(resolveDependentCheat(GetPlayerDatum)),
		getObjectAddressWeak(resolveDependentCheat(GetObjectAddress)),
		tagTableRangeWeak(resolveDependentCheat(TagTableRange))
	{
		auto ptr = dicon.Resolve<PointerDataStore>().lock();
		playerDataStruct = DynamicStructFactory::make<bipedDataFields>(ptr, game);
		equipmentDataStruct = DynamicStructFactory::make<equipmentDataFields>(ptr, game);
		equipmentTagDataStruct = DynamicStructFactory::make<equipmentTagDataFields>(ptr, game);
	}

	virtual std::expected <AbilityData, pHCMError> getAbilityData(bool forceHeroAssist) override
	{
		try
		{
		lockOrUnexpected(getPlayerDatumWeak, getPlayerDatum);
		auto playerDatum = getPlayerDatum->getPlayerDatum();

		if (playerDatum.isNull())
			return std::unexpected(std::make_shared< ErrorLiteralString>("Null Player Datum!"));

		lockOrUnexpected(getObjectAddressWeak, getObjectAddress);
		auto playerObjectAddress = getObjectAddress->getObjectAddress(playerDatum);

		playerDataStruct->currentBaseAddress = playerObjectAddress;
		Datum* equipmentDatum = forceHeroAssist ?  (playerDataStruct->field<Datum>(heroDatumField)) : (playerDataStruct->field<Datum>(equipmentDatumField));
		if (IsBadReadPtr(equipmentDatum, sizeof(Datum))) 
			return std::unexpected(std::make_shared<BadReadPtrError>(nameof(equipmentDatum), equipmentDatum));

		if (equipmentDatum->isNull())
		{
			return std::unexpected(std::make_shared<ErrorLiteralString>("No Ability Equipped!"));
		}

		auto equipmentObjectAddress = getObjectAddress->getObjectAddress(*equipmentDatum);
		equipmentDataStruct->currentBaseAddress = equipmentObjectAddress;

		LOG_ONCE_CAPTURE(PLOG_DEBUG << "equipmentObjectAddress: " << std::hex << p, p = equipmentObjectAddress);

		float* abilityPercent = equipmentDataStruct->field<float>(equipmentDataFields::abilityPercent);
		if (IsBadReadPtr(abilityPercent, sizeof(float)))
			return std::unexpected(std::make_shared<BadReadPtrError>(nameof(abilityPercent), abilityPercent));

		uint32_t* cooldownTicks = equipmentDataStruct->field<uint32_t>(equipmentDataFields::cooldownTicks);
		if (IsBadReadPtr(cooldownTicks, sizeof(uint32_t)))
			return std::unexpected(std::make_shared<BadReadPtrError>(nameof(cooldownTicks), cooldownTicks));




		Datum* abilityTagDatum = equipmentDataStruct->field<Datum>(equipmentDataFields::tagDatum);
		if (IsBadReadPtr(abilityTagDatum, sizeof(Datum)))
			return std::unexpected(std::make_shared<BadReadPtrError>(nameof(abilityTagDatum), abilityTagDatum));

		uint32_t thisAbilityMaxCooldown;
		static Datum lastAbilityTagDatum;
		static uint32_t lastAbilityMaxCooldown;
		if (*abilityTagDatum == lastAbilityTagDatum)
		{
			// basically we can just cache the max cooldown data if it's the same tag datum as last time - looking up objects in the tag table is kinda slow.
			thisAbilityMaxCooldown = lastAbilityMaxCooldown;
		}
		else
		{
			lockOrUnexpected(tagTableRangeWeak, tagTableRange);
			auto abilityTagAddress = tagTableRange->getTagByDatum(*abilityTagDatum);
			if (!abilityTagAddress)
				return std::unexpected(std::make_shared < RuntimeExceptionToError>(abilityTagAddress.error()));

			equipmentTagDataStruct->currentBaseAddress = abilityTagAddress.value().tagAddress;
			float* abilityMaxCooldownF = equipmentTagDataStruct->field<float>(equipmentTagDataFields::abilityMaxCooldownF);
			if (IsBadReadPtr(abilityMaxCooldownF, sizeof(float)))
				return std::unexpected(std::make_shared<BadReadPtrError>(nameof(abilityMaxCooldownF), abilityMaxCooldownF));

			// just need to convert float to ticks
			thisAbilityMaxCooldown = *abilityMaxCooldownF > 0.f ?( (*abilityMaxCooldownF * 60.f) - 1) : 0;
			

			// store for next time
			lastAbilityTagDatum = *abilityTagDatum;
			lastAbilityMaxCooldown = thisAbilityMaxCooldown;
		}



		// finally parse the abilityCooldown using help of max cooldown
		uint32_t parsedCooldownTicks = std::clamp(*cooldownTicks, (uint32_t)0, thisAbilityMaxCooldown);

		return AbilityData{ *abilityPercent, parsedCooldownTicks, thisAbilityMaxCooldown };

		}
		catch (HCMRuntimeException ex)
		{
			return std::unexpected(std::make_shared< RuntimeExceptionToError>(ex));
		}
	}
};






GetAbilityData::GetAbilityData(GameState game, IDIContainer& dicon)
{

	switch (game)
	{
	case GameState::Value::HaloReach:
	{
		enum class bipedDataFields { equipmentDatum };
		pimpl = std::make_unique<GetAbilityDataImpl<bipedDataFields, bipedDataFields::equipmentDatum, bipedDataFields::equipmentDatum>>(game, dicon);
	}
	break;


	case GameState::Value::Halo4:
	{
		enum class bipedDataFields { equipmentDatum, heroAssistDatum };
		pimpl = std::make_unique<GetAbilityDataImpl<bipedDataFields, bipedDataFields::equipmentDatum, bipedDataFields::heroAssistDatum>>(game, dicon);
	}
	break;


	default:
		throw HCMInitException("this game doesn't have ability, silly");
	}
}
GetAbilityData::~GetAbilityData() = default;