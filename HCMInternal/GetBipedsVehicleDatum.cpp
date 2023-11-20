#include "pch.h"
#include "GetBipedsVehicleDatum.h"
#include "GetObjectAddress.h"
#include "IMakeOrGetCheat.h"
#include "DynamicStructFactory.h"
#include "ObjectTypes.h"

class GetBipedsVehicleDatum::GetBipedsVehicleDatumImpl 
{
private:

	// injected services
	std::weak_ptr< GetObjectAddress> getObjectAddressWeak;

	enum class bipedDataFields { currentVehicleDatum };
	std::shared_ptr<DynamicStruct<bipedDataFields>> bipedDataStruct;

public:
	GetBipedsVehicleDatumImpl(GameState game, IDIContainer& dicon)
	{
		getObjectAddressWeak = resolveDependentCheat(GetObjectAddress);

		auto ptr = dicon.Resolve<PointerManager>().lock();
		bipedDataStruct = DynamicStructFactory::make<bipedDataFields>(ptr, game);
	}

	Datum getBipedsVehicleDatum(Datum bipedDatum)
	{
		lockOrThrow(getObjectAddressWeak, getObjectAddress);

		CommonObjectType thisObjectTypeEnum;
		auto bipedObjectAddress = getObjectAddress->getObjectAddress(bipedDatum, &thisObjectTypeEnum);

		if (thisObjectTypeEnum != CommonObjectType::Biped) throw HCMRuntimeException(std::format("Object was not biped type! was {}", magic_enum::enum_name<CommonObjectType>(thisObjectTypeEnum)));
	
		return getBipedsVehicleDatum(bipedObjectAddress);
	}
	Datum getBipedsVehicleDatum(uintptr_t bipedObjectAddress)
	{
		bipedDataStruct->currentBaseAddress = bipedObjectAddress;

//#ifdef HCM_DEBUG
//		PLOG_VERBOSE << "bipedObjectAddress: " << std::hex << bipedObjectAddress;
//		auto* pBipedsVehicleDatumtest = bipedDataStruct->field<Datum>(bipedDataFields::currentVehicleDatum);
//		PLOG_VERBOSE << "pBipedsVehicleDatum: " << std::hex << (uintptr_t)pBipedsVehicleDatumtest;
//		Datum test = *pBipedsVehicleDatumtest;
//		PLOG_VERBOSE << "BipedsVehicleDatum: " << test;
//#endif

		auto* pBipedsVehicleDatum = bipedDataStruct->field<Datum>(bipedDataFields::currentVehicleDatum);
		Datum bipedsVehicleDatum = *pBipedsVehicleDatum;

//#ifdef HCM_DEBUG
//		PLOG_VERBOSE << "returning: " << bipedsVehicleDatum;
//#endif

		return bipedsVehicleDatum;
	}
};






GetBipedsVehicleDatum::GetBipedsVehicleDatum(GameState gameImpl, IDIContainer& dicon)
{
	pimpl = std::make_unique<GetBipedsVehicleDatumImpl>(gameImpl, dicon);
}
GetBipedsVehicleDatum::~GetBipedsVehicleDatum()
{
	PLOG_DEBUG << "~" << getName();
}

Datum GetBipedsVehicleDatum::getBipedsVehicleDatum(Datum bipedDatum) { return pimpl->getBipedsVehicleDatum(bipedDatum); }
Datum GetBipedsVehicleDatum::getBipedsVehicleDatum(uintptr_t bipedAddress) { return pimpl->getBipedsVehicleDatum(bipedAddress); }