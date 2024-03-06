#include "pch.h"
#include "PointerSetting.h"
#include "MultilevelPointer.h"

template <typename valueType>
class PointerSettingImpl : public PointerSetting<valueType>::IPointerSettingImpl
{
private:
	std::optional<std::shared_ptr<MultilevelPointer>> pointerToData = std::nullopt;
	std::string lastError = "no error set";
public:
	virtual bool successfullyConstructed() override
	{
		return pointerToData != std::nullopt;
	}
	virtual const std::optional<valueType> getCopy() override
	{
		return *getRef();
	}
	virtual std::optional<valueType*> getRef() override
	{
		if (successfullyConstructed())
		{
			uintptr_t resolvedOut;
			if (pointerToData.value()->resolve(&resolvedOut))
			{
				return (valueType*)resolvedOut;
			}
			else
			{
				lastError = MultilevelPointer::GetLastError();
				return std::nullopt;
			}
		}
	}
	virtual bool set(valueType value) override
	{
		auto ref = getRef();
		if (ref != std::nullopt)
		{
			*ref.value() = value;
			return true;
		}
		else
		{
			return false;
		}
	}
	virtual std::string_view getLastError() override
	{
		return lastError;
	}

	PointerSettingImpl(std::string pointerDataName, std::shared_ptr<PointerManager> pointerManager, GameState implToConstruct) noexcept
	{
		try
		{
			pointerToData = pointerManager->getData<std::shared_ptr<MultilevelPointer>>(pointerDataName, implToConstruct);
		}
		catch (HCMInitException ex)
		{
			lastError = ex.what();
		}
	}

};

template <typename valueType>
PointerSetting<valueType>::PointerSetting(std::string pointerDataName, std::shared_ptr<PointerManager> pointerManager, std::vector<GameState> implsToConstruct) noexcept
{
	optionName = pointerDataName; // just used for debugging anyway
	for (auto& game : implsToConstruct)
	{
		auto impl = std::make_shared<PointerSettingImpl<valueType>>(pointerDataName, pointerManager, game);
		// wish I knew how to make emplace or make_pair deduce the types, but whatever
		pimplMap.emplace(std::make_pair<GameState, std::shared_ptr<IPointerSettingImpl>>(game.operator GameState::Value(), std::dynamic_pointer_cast<IPointerSettingImpl>(impl)));
	}
}

template <typename valueType>
PointerSetting<valueType>::~PointerSetting() = default;

template <typename valueType>
bool PointerSetting<valueType>::successfullyConstructed(GameState gameImpl)
{
	if (pimplMap.contains(gameImpl))
		return pimplMap.at(gameImpl)->successfullyConstructed();
	else
		return false;
}

template <typename valueType>
const std::optional<valueType> PointerSetting<valueType>::getCopy(GameState gameImpl)
{
	if (pimplMap.contains(gameImpl))
		return pimplMap.at(gameImpl)->getCopy();
	else
		return std::nullopt;
}

template <typename valueType>
std::optional<valueType*> PointerSetting<valueType>::getRef(GameState gameImpl)
{
	if (pimplMap.contains(gameImpl))
		return pimplMap.at(gameImpl)->getRef();
	else
		return std::nullopt;
}

template <typename valueType>
bool PointerSetting<valueType>::set(GameState gameImpl, valueType value)
{
	if (pimplMap.contains(gameImpl))
		return pimplMap.at(gameImpl)->set(value);
	else
		return false;
}

template <typename valueType>
std::string_view PointerSetting<valueType>::getLastError(GameState gameImpl)
{
	if (pimplMap.contains(gameImpl))
		return pimplMap.at(gameImpl)->getLastError();
	else
		return "never attempted construction";
}

template <typename valueType>
std::string_view PointerSetting<valueType>::getOptionName()
{
	return optionName;
}

// explicit template instantiation
template class PointerSetting<bool>;