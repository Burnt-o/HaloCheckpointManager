#pragma once
#include "IOptionalCheat.h"
#include "DIContainer.h"
#include "GameState.h"
#include "Datum.h"
#include "ObjectTypes.h"



class IGetObjectAddressImpl
{
public:
	virtual ~IGetObjectAddressImpl() = default;
	uintptr_t getObjectAddressNoObj(Datum datum) { return getObjectAddressCommon(datum, (CommonObjectType*)nullptr); }
	virtual uintptr_t getObjectAddressCommon(Datum datum, CommonObjectType* commonObjectType ) = 0;

	virtual uintptr_t getObjectAddressTemplated(Datum datum, Halo1ObjectType* commonObjectType) { throw HCMRuntimeException("game mismatch!"); }
	virtual uintptr_t getObjectAddressTemplated(Datum datum, Halo2ObjectType* commonObjectType) { throw HCMRuntimeException("game mismatch!"); }
	virtual uintptr_t getObjectAddressTemplated(Datum datum, Halo3ObjectType* commonObjectType) { throw HCMRuntimeException("game mismatch!"); }
	virtual uintptr_t getObjectAddressTemplated(Datum datum, Halo3ODSTObjectType* commonObjectType) { throw HCMRuntimeException("game mismatch!"); }
	virtual uintptr_t getObjectAddressTemplated(Datum datum, HaloReachObjectType* commonObjectType) { throw HCMRuntimeException("game mismatch!"); }
	virtual uintptr_t getObjectAddressTemplated(Datum datum, Halo4ObjectType* commonObjectType) { throw HCMRuntimeException("game mismatch!"); }
};


class GetObjectAddress : public IOptionalCheat
{
	private:
		std::unique_ptr<IGetObjectAddressImpl> pimpl;
	
	public:
		GetObjectAddress(GameState game, IDIContainer& dicon);
		~GetObjectAddress();
	

		uintptr_t getObjectAddress(Datum entityDatum);

		template <typename TemplatedObjectType>
		uintptr_t getObjectAddress(Datum entityDatum, TemplatedObjectType* outObjectType);

		uintptr_t getObjectAddress(Datum entityDatum, CommonObjectType* outObjectType);
	
		virtual std::string_view getName() override { return nameof(GetObjectAddress); }
};
