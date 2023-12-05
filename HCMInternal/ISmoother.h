#pragma once

template <typename valueType>
class ISmoother
{
public:
	virtual ~ISmoother() = default;
	virtual void smooth(valueType& currentValue, valueType desiredValue) = 0;
};
