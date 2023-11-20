#include "pch.h"
#include "GetNextObjectDatum.h"
#include "MultilevelPointer.h"
#include "PointerManager.h"

class GetNextObjectDatum::GetNextObjectDatumImpl
{
private:

	//data
	std::shared_ptr<MultilevelPointer> nextObjectSalt;
	std::shared_ptr<MultilevelPointer> nextObjectIndex;


public:
	GetNextObjectDatumImpl(GameState game, IDIContainer& dicon)
	{
		auto ptr = dicon.Resolve<PointerManager>().lock();
		nextObjectSalt = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(nextObjectSalt), game);
		nextObjectIndex = ptr->getData<std::shared_ptr<MultilevelPointer>>(nameof(nextObjectIndex), game);
	}

	Datum getNextObjectDatum()
	{
		Datum outDatum;
		if (!nextObjectSalt->readData(&outDatum.salt)) throw HCMRuntimeException("Could not resolve nextObjectSalt");
		if (!nextObjectIndex->readData(&outDatum.index)) throw HCMRuntimeException("Could not resolve nextObjectIndex");
		return outDatum;
	}

};




GetNextObjectDatum::GetNextObjectDatum(GameState gameImpl, IDIContainer& dicon)
{
	pimpl = std::make_unique< GetNextObjectDatumImpl>(gameImpl, dicon);
}

GetNextObjectDatum::~GetNextObjectDatum()
{
	PLOG_DEBUG << "~" << getName();
}


Datum GetNextObjectDatum::getNextObjectDatum() { return pimpl->getNextObjectDatum(); }