#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"
#include "SoftCeilingData.h"
#include "CsLibGuarded\cs_plain_guarded.h"

typedef std::vector<SoftCeilingData> SoftCeilingVector;
typedef libguarded::plain_guarded<SoftCeilingVector>::handle SoftCeilingVectorLock;



class GetSoftCeilingDataUntemplated
{

public:
	virtual ~GetSoftCeilingDataUntemplated() = default;
	virtual std::expected<SoftCeilingVectorLock, HCMRuntimeException> getSoftCeilings() = 0;

};

class GetSoftCeilingData : public IOptionalCheat
{
private:
	std::unique_ptr<GetSoftCeilingDataUntemplated> pimpl;
public:

	GetSoftCeilingData(GameState gameImpl, IDIContainer& dicon);
	~GetSoftCeilingData();

	std::expected<SoftCeilingVectorLock, HCMRuntimeException> getSoftCeilings() { return pimpl->getSoftCeilings(); }

	std::string_view getName() override { return nameof(GetSoftCeilingData); }




};