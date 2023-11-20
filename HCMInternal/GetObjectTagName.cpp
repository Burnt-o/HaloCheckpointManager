#include "pch.h"
#include "GetObjectTagName.h"
#include "GetTagName.h"
#include "GetObjectAddress.h"
#include "IMakeOrGetCheat.h"
#include "DynamicStructFactory.h"

class GetObjectTagName::GetObjectTagNameImpl
{
private:
	// injected services
	std::weak_ptr< GetObjectAddress> getObjectAddressWeak;
	std::weak_ptr< GetTagName> getTagNameWeak;

	//data
	enum class commonObjectDataFields { tagDatum };
	std::shared_ptr<DynamicStruct<commonObjectDataFields>> commonObjectDataStruct;

public:
	GetObjectTagNameImpl(GameState game, IDIContainer& dicon)
		: 
		getObjectAddressWeak(resolveDependentCheat(GetObjectAddress)),
		getTagNameWeak(resolveDependentCheat(GetTagName))
	{
		auto ptr = dicon.Resolve<PointerManager>().lock();
		commonObjectDataStruct = DynamicStructFactory::make<commonObjectDataFields>(ptr, game);
	}

	const char* getObjectTagName(Datum objectDatum)
	{  
		LOG_ONCE(PLOG_DEBUG << "getObjectTagName");
		lockOrThrow(getObjectAddressWeak, getObjectAddress);
		commonObjectDataStruct->currentBaseAddress = getObjectAddress->getObjectAddress(objectDatum);
		
		lockOrThrow(getTagNameWeak, getTagName);
		Datum* tagDatum = commonObjectDataStruct->field<Datum>(commonObjectDataFields::tagDatum);
		LOG_ONCE_CAPTURE(PLOG_DEBUG << "getting tag name for tag datum at address: " << ptd, ptd = (uintptr_t)tagDatum);
		LOG_ONCE_CAPTURE(PLOG_DEBUG << "getting tag name for tag datum: " << td, td = *tagDatum);
		return getTagName->getTagName(*tagDatum);
	}

};




GetObjectTagName::GetObjectTagName(GameState gameImpl, IDIContainer& dicon)
{
	pimpl = std::make_unique< GetObjectTagNameImpl>(gameImpl, dicon);
}

GetObjectTagName::~GetObjectTagName()
{
	PLOG_DEBUG << "~" << getName();
}


const char* GetObjectTagName::getObjectTagName(Datum objectDatum) { return pimpl->getObjectTagName(objectDatum); }