#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "SoftCeilingData.h"
#include "CsLibGuarded\cs_plain_guarded.h"
#include "ScopedServiceRequest.h"

typedef std::vector<SoftCeilingData> SoftCeilingVector;
typedef libguarded::plain_guarded<SoftCeilingVector>::handle SoftCeilingVectorLock;


class IGetSoftCeilingDataImpl :public std::enable_shared_from_this<IGetSoftCeilingDataImpl>
{
public:
	virtual std::expected<SoftCeilingVectorLock, HCMRuntimeException> getSoftCeilings() = 0;
	virtual ~IGetSoftCeilingDataImpl() = default;
	virtual void updateRequestState(bool requested) = 0;
};

class SoftCeilingDataProvider : public ScopedServiceRequest
{
private:
	std::shared_ptr<IGetSoftCeilingDataImpl> dataPimpl;
public:
	SoftCeilingDataProvider(std::shared_ptr<IGetSoftCeilingDataImpl> dataProvider, std::shared_ptr<IProvideScopedRequests> requestManager, std::string callerID) : ScopedServiceRequest(requestManager, callerID), dataPimpl(dataProvider) {}
	std::expected<SoftCeilingVectorLock, HCMRuntimeException> getSoftCeilings() { return dataPimpl->getSoftCeilings(); }
};



class GetSoftCeilingData : public IOptionalCheat
{
private:
	// must be shared for request management
	std::shared_ptr<TemplatedScopedServiceProvider<SoftCeilingDataProvider>> pimpl;
public:

	GetSoftCeilingData(GameState gameImpl, IDIContainer& dicon);
	~GetSoftCeilingData();

	std::unique_ptr<SoftCeilingDataProvider> getSoftCeilingDataProvider(std::string callerID) {
		return pimpl->makeRequest(callerID);
	};

	std::string_view getName() override { return nameof(GetSoftCeilingData); }




};