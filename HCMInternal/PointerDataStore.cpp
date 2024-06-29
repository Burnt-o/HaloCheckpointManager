#include "pch.h"
#include "PointerDataStore.h"
#include "DynamicStructFactory.h"
#include "InjectRequirements.h"
#include "MultilevelPointer.h"
#include "MidhookContextInterpreter.h"
#include "MidhookFlagInterpreter.h"
#include "MCCState.h"

template <typename T>
T PointerDataStore::getData(std::string dataName, std::optional<GameState> game)
{

    auto key = DataKey(dataName, game);
    auto altkey = DataKey(dataName, std::nullopt); // alternate key for non-game specific data
    // Check data exists
    if (!mDataStore.contains(key))
    {
        if (!mDataStore.contains(altkey))
        {
            PLOG_ERROR << "no valid pointer data for " << dataName;
            throw HCMInitException(std::format("pointerData was null for {}", dataName));
        }
        else // grab altkey data
        {
            // Check correct type
            auto& type = mDataStore.at(altkey).type();
            if (!(typeid(T) == type))
            {
                throw HCMInitException(std::format("Invalid type access for {}\nType was {} but {} was requested", dataName, type.name(), typeid(T).name()));
            }

            return std::any_cast<T>(mDataStore.at(altkey));
        }

    }

    // Check correct type
    auto& type = mDataStore.at(key).type();
    if (!(typeid(T) == type))
    {
        throw HCMInitException(std::format("Invalid type access for {}\nType was {} but {} was requested", dataName, type.name(), typeid(T).name()));
    }

#ifdef HCM_DEBUG
    if (std::is_same_v<T, std::shared_ptr<MultilevelPointer>>
        && game != std::nullopt)
    {
        auto realOut = std::any_cast<std::shared_ptr<MultilevelPointer>>(mDataStore.at(key));

        // check if is module type
        auto moduleSpec = std::dynamic_pointer_cast<MultilevelPointerSpecialisation::ModuleOffset>(realOut);
        if (moduleSpec)
        {
            PLOG_DEBUG << "verifying module type of MultilevelPointerSpecialisation::ModuleOffset for game: " << game.value().toString();
            PLOG_DEBUG << "Module type is: " << moduleSpec->getModuleName();

            if (moduleSpec->getModuleName() != game.value().toModuleName()) throw HCMInitException(std::format("MODULE NAME MISMATCH: Game was {}, but module name was {}", game.value().toString(), wstr_to_str(moduleSpec->getModuleName().data())));

        }

    }
#endif

    return std::any_cast<T>(mDataStore.at(key));
}

template <>
std::shared_ptr<MultilevelPointerSpecialisation::BaseOffset> PointerDataStore::getData(std::string dataName, std::optional<GameState> game)
{
    auto out = std::dynamic_pointer_cast<MultilevelPointerSpecialisation::BaseOffset>(getData<std::shared_ptr<MultilevelPointer>>(dataName, game));
    if (out)
        return out;
    else
        throw HCMInitException("Bad multilevel pointer access! Was not BaseOffset specialisation");
}

template <>
std::shared_ptr<MultilevelPointerSpecialisation::ExeOffset> PointerDataStore::getData(std::string dataName, std::optional<GameState> game)
{
    auto out = std::dynamic_pointer_cast<MultilevelPointerSpecialisation::ExeOffset>(getData<std::shared_ptr<MultilevelPointer>>(dataName, game));
    if (out)
        return out;
    else
        throw HCMInitException("Bad multilevel pointer access! Was not ExeOffset specialisation");
}

template <>
std::shared_ptr<MultilevelPointerSpecialisation::ModuleOffset> PointerDataStore::getData(std::string dataName, std::optional<GameState> game)
{
    auto out = std::dynamic_pointer_cast<MultilevelPointerSpecialisation::ModuleOffset>(getData<std::shared_ptr<MultilevelPointer>>(dataName, game));
    if (out)
        return out;
    else
        throw HCMInitException("Bad multilevel pointer access! Was not ModuleOffset specialisation");
}

template <>
std::shared_ptr<MultilevelPointerSpecialisation::Resolved> PointerDataStore::getData(std::string dataName, std::optional<GameState> game)
{
    auto out = std::dynamic_pointer_cast<MultilevelPointerSpecialisation::Resolved>(getData<std::shared_ptr<MultilevelPointer>>(dataName, game));
    if (out)
        return out;
    else
        throw HCMInitException("Bad multilevel pointer access! Was not Resolved specialisation");
}


template <typename T>
std::shared_ptr<std::vector<T>> PointerDataStore::getVectorData(std::string dataName, std::optional<GameState> game)
{

    auto key = DataKey(dataName, game);
    auto altkey = DataKey(dataName, std::nullopt); // alternate key for non-game specific data
    // Check data exists
    if (!mDataStore.contains(key))
    {
        if (!mDataStore.contains(altkey))
        {
            PLOG_ERROR << "no valid pointer data for " << dataName;
            throw HCMInitException(std::format("pointerData was null for {}", dataName));
        }
        else // grab altkey data
        {
            // Check correct type
            auto& type = mDataStore.at(altkey).type();
            if (!(typeid(std::shared_ptr<std::vector<T>>) == type))
            {
                throw HCMInitException(std::format("Invalid type access for {}\nType was {} but {} was requested", dataName, type.name(), typeid(std::shared_ptr<std::vector<T>>).name()));
            }



            return std::any_cast<std::shared_ptr<std::vector<T>>>(mDataStore.at(altkey));
        }

    }

    // Check correct type
    auto& type = mDataStore.at(key).type();
    if (!(typeid(std::shared_ptr<std::vector<T>>) == type))
    {
        throw HCMInitException(std::format("Invalid type access for {}\nType was {} but {} was requested", dataName, type.name(), typeid(std::shared_ptr<std::vector<T>>).name()));
    }

    return std::any_cast<std::shared_ptr<std::vector<T>>>(mDataStore.at(key));
}

// explicit template instantiations of PointerDataStore::getData
template
std::shared_ptr<MultilevelPointer> PointerDataStore::getData(std::string dataName, std::optional<GameState> game);
template
std::shared_ptr<InjectRequirements> PointerDataStore::getData(std::string dataName, std::optional<GameState> game);
template
std::shared_ptr<PreserveLocations> PointerDataStore::getData(std::string dataName, std::optional<GameState> game);
template
std::shared_ptr<offsetLengthPair> PointerDataStore::getData(std::string dataName, std::optional<GameState> game);
template
std::shared_ptr<int64_t> PointerDataStore::getData(std::string dataName, std::optional<GameState> game);
template
std::shared_ptr<MidhookContextInterpreter> PointerDataStore::getData(std::string dataName, std::optional<GameState> game);
template
std::shared_ptr<MidhookFlagInterpreter> PointerDataStore::getData(std::string dataName, std::optional<GameState> game);
template
std::shared_ptr<DynStructOffsetInfo> PointerDataStore::getData(std::string dataName, std::optional<GameState> game);

template
std::shared_ptr<std::vector<byte>> PointerDataStore::getVectorData(std::string dataName, std::optional<GameState> game);

template
std::shared_ptr<std::map<LevelID, std::vector<std::pair<std::string, std::string>>>> PointerDataStore::getData(std::string dataName, std::optional<GameState> game);