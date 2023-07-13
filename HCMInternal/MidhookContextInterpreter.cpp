#include "pch.h"
#include "MidhookContextInterpreter.h"
#include "OptionsState.h"
// return ref to the ctx register we want


uintptr_t* MidhookContextInterpreter::getParameterRef(SafetyHookContext& ctx, int parameterIndex)
{

	if (parameterIndex > mParameterRegisterIndices.size())
	{
		HCMRuntimeException exception(std::format("MidhookContextInterpreter had invalid access of parameter! paramArray.size(): {}, accessed parameter index: {}", mParameterRegisterIndices.size(), parameterIndex));
		RuntimeExceptionHandler::handleMessage(exception); 
		return nullptr; // this will cause an imminent game crash. But at least we logged why. This error only happens when Burnt makes a typo. 
		// (I should figure a way out to make typos not possible hm)
	}

	ParameterLocation& thisParameter = mParameterRegisterIndices.at(parameterIndex);
	uintptr_t* ctxArray = reinterpret_cast<uintptr_t*>(&ctx);

	if (thisParameter.getOffsets().empty())
	{
		return &ctxArray[thisParameter.getRegIndex()];
	}
	else
	{
		uintptr_t ptr = ctxArray[thisParameter.getRegIndex()];
		auto &offsets = thisParameter.getOffsets();



		for (int i = 0; i < offsets.size(); i++)
		{
			int offset = offsets[i];
			uintptr_t follow = (uintptr_t)(ptr + offset);
			if (IsBadReadPtr((void*)follow, 8))
			{
				HCMRuntimeException ex(std::format("bad read in midhookcontextinterpreter! next: {}, ptr: {}, offset: {}", follow, ptr, offset));
				RuntimeExceptionHandler::handleMessage(ex);
				return nullptr;
			}

			if (i == 0)
			{
				ptr = follow;
			}
			else
			{
				ptr = *(uintptr_t*)follow; // dereference
			}
		}

		return (uintptr_t*)ptr;
	}
}

