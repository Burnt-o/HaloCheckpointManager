#include "pch.h"
#include "PointerDataParser.h"
#include "PointerDataParserInstantiators.h"
using namespace pugi;

bool PointerDataParser::entryIsCorrectMCCVersion(VersionEntry entry)
{
    std::string versionEntryMCCVersion = entry.attribute("Version").value();
    bool out = versionEntryMCCVersion == "All" || versionEntryMCCVersion == mGetMCCver->getMCCVersionAsString();

    if (!out) 
         PLOG_DEBUG << "Version mismatch: " << versionEntryMCCVersion; 
    return out;
}

bool PointerDataParser::entryIsCorrectProcessType(VersionEntry entry)
{
    if (entry.attribute("ProcessType").empty() == true) return true;

    std::string processTypeString = entry.attribute("ProcessType").as_string();
    bool out = mGetMCCver->getMCCProcessTypeAsString() == processTypeString;

    if (!out) 
        PLOG_DEBUG << "ProcessType mismatch: " << processTypeString; 
    return out;
}

std::optional<GameState> PointerDataParser::getEntryGame(VersionEntry entry)
{
    if (!entry.attribute("Game").empty())
    {
        std::string gameString = entry.attribute("Game").value();
        return GameState::fromString(gameString);
    }
    else
    {
        return std::nullopt;
    }
}


template<class T>
struct is_shared_ptr : std::false_type {};

template<class T>
struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};

template <typename T>  
void PointerDataParser::emplaceObject(T& object, VersionEntry versionEntry)
{
    VersionCollection versionCollection = versionEntry.parent();

    // key for map
    auto entryGame = getEntryGame(versionEntry);
    auto entryGameString = entryGame.has_value() ? entryGame.value().toString() : "All versions";
    auto dKey = DataKey(versionCollection.attribute("Name").value(), entryGame);

    // check for duplicates
    if (mDataStoreToLoad.contains(dKey))
    {
        std::string err = std::format("Entry already exists in pointerData, entryName: {}, entryGame: {}\n", std::get<std::string>(dKey), entryGameString);
#ifdef HCM_DEBUG
        throw HCMInitException(err);
#else
        PLOG_ERROR << err;
#endif
    }

    PLOG_DEBUG << "Loaded " << versionCollection.attribute("Type").value() << " " << std::get<std::string>(dKey) << " for game " << entryGameString;

    
    if constexpr (is_shared_ptr<T>::value == true)
    {
        mDataStoreToLoad.try_emplace(dKey, object);
    }
    else
    {
        auto shared = std::make_shared<T>(object);
        mDataStoreToLoad.try_emplace(dKey, shared);
    }

}


void PointerDataParser::processVersionedEntry(VersionEntry versionEntry,std::string versionEntryType, std::string versionEntryName, std::string versionEntryVectorType)
{
    // Switch on type of the data and add it to the data store!
    enum class PointerDataType
    {
        MultilevelPointer,
        MidhookContextInterpreter,
        MidhookFlagInterpreter,
        Vector,
        injectRequirements,
        preserveLocations,
        offsetLengthPair,
        int64_t,
        DynStructOffsetInfo,
        Invalid,
        LevelMapStringVector
    } currentEntryType = magic_enum::enum_cast<PointerDataType>(versionEntryType).value_or(PointerDataType::Invalid);


    {
        // MultilevelPointers subtypes actually won't work with above enum cast but always start with "MultilevelPointer"
        if (versionEntryType.starts_with("MultilevelPointer"))
            currentEntryType = PointerDataType::MultilevelPointer;
    }

    switch (currentEntryType)
    {
    case PointerDataType::MultilevelPointer:
        instantiateMultilevelPointer(versionEntry);
        break;

    case PointerDataType::MidhookContextInterpreter:
        instantiateMidhookContextInterpreter(versionEntry);
        break;

    case PointerDataType::MidhookFlagInterpreter:
        instantiateMidhookFlagInterpreter(versionEntry);
        break;

    case PointerDataType::Vector:
        // I'll have to manually add each type I want to support here.
        if (versionEntryVectorType == nameof(byte))
            instantiateVectorInteger<byte>(versionEntry);
        else if (versionEntryVectorType == nameof(int))
            instantiateVectorInteger<int>(versionEntry);
        else if (versionEntryVectorType == nameof(long))
            instantiateVectorInteger<long>(versionEntry);
        else if (versionEntryVectorType == nameof(float))
            instantiateVectorFloat<float>(versionEntry);
        else if (versionEntryVectorType == nameof(double))
            instantiateVectorFloat<double>(versionEntry);
        else if (versionEntryVectorType == nameof(long double))
            instantiateVectorFloat<long double>(versionEntry);
        else if (versionEntryVectorType == nameof(std::string))
            instantiateVectorString(versionEntry);
        else
            throw HCMInitException(std::format("Unsupported typename passed to instantiateVector {}: {}", versionEntryName, versionEntryVectorType));

        break;

    case PointerDataType::injectRequirements:
        instantiateInjectRequirements(versionEntry);
        break;

    case PointerDataType::preserveLocations:
        instantiatePreserveLocations(versionEntry);
        break;

    case PointerDataType::offsetLengthPair:
        instantiateOffsetLengthPair(versionEntry);
        break;

    case PointerDataType::int64_t:
        instantiateInt64_t(versionEntry);
        break;

    case PointerDataType::DynStructOffsetInfo:
        instantiateDynStructOffsetInfo(versionEntry);
        break;

    case PointerDataType::LevelMapStringVector:
        instantiateLevelMapStringVector(versionEntry);
        break;

    default:
    case PointerDataType::Invalid:
        throw HCMInitException(std::format("Bad Type in pointer data!: {}, on entry named {}", versionEntryType, versionEntryName));
        break;
    }
}




void PointerDataParser::parse()
{
    using namespace pugi;
    xml_document doc;
    xml_parse_result result = doc.load_string(mXMLDocument.c_str());
    if (!result)
    {
        std::stringstream er;
        er << "XML parsed with errors, attr value: [" << doc.child("node").attribute("attr").value() << "]\n";
        er << "Error description: " << result.description() << "\n";
        er << "Error offset: " << result.offset << " (error at [..." << (mXMLDocument.c_str() + result.offset) << "]\n\n";
        er << "XML contents: \n\n" << mXMLDocument.c_str();
        throw HCMInitException(er.str());
    }

    xml_node root = doc.child("root");


    for (xml_node entry = root.first_child(); entry; entry = entry.next_sibling())
    {
        std::string entryName = entry.name();
        PLOG_DEBUG << "Processing entry : " << entryName;

        if (entryName == "SupportedGameVersions")
        {
            bool foundSupportedVersion = false;
            for (xml_node supportedVersion = entry.first_child(); supportedVersion; supportedVersion = supportedVersion.next_sibling())
            {
                std::string versionString = supportedVersion.text().as_string();
                if (versionString == mGetMCCver->getMCCVersionAsString())
                {
                    foundSupportedVersion = true;
                    break;
                }
            }

            if (!foundSupportedVersion)
            {
                throw HCMInitException(std::format("The current version of MCC ({}) is not yet supported by HCM! ", mGetMCCver->getMCCVersionAsString()) +
                    "\nYou'll have to wait for Burnt to update it if HCM just got a patch," +
                    "\nor if you're on an old MCC patch, kindly ask him to add support.");
            }
        }
        else if (entryName == "VersionedEntry")
        {
            PLOG_DEBUG << "Processing Versioned Entry, name: " << entry.attribute("Name").value();
            // loop over all sub (version) entries, filter to correct MCC version, instantiate
            for (VersionEntry versionEntry = entry.first_child(); versionEntry; versionEntry = versionEntry.next_sibling())
            {
                // Check that the entries MCC-version matches the current actual MCC version
                // Check for Steam vs Winstore process type specificty
                if (!entryIsCorrectMCCVersion(versionEntry) || !entryIsCorrectProcessType(versionEntry)) 
                    continue;

                try
                {
                    processVersionedEntry(versionEntry, entry.attribute("Type").value(), entry.attribute("Name").value(), entry.attribute("Typename").value());
                }
                // rethrow if HCM_DEBUG, otherwise continue processing
                catch (HCMInitException ex)
                {
                    std::stringstream xmlContent;
                    versionEntry.print(xmlContent);
                    std::stringstream extraInfo;
                    extraInfo << "Failed to process pointer data: " << std::endl
                        << "Name: " << entry.attribute("Name").value() << std::endl
                        << "Type : " << entry.attribute("Type").value() << std::endl
                        << "Version : " << versionEntry.attribute("Version").value() << std::endl
                        << "ProcessType : " << versionEntry.attribute("ProcessType").value() << std::endl
                        << "Game: " << versionEntry.attribute("Game").value() << std::endl
                        << "Error: " << ex.what() << std::endl
                        << "Data: " << xmlContent.str();
                    PLOG_ERROR << extraInfo.str();
#ifdef HCM_DEBUG
                    ex.append(extraInfo.str());
                    throw ex;
#endif
                }
            }


        }
        else
        {
            std::string err = std::format("Unexpected item in pointer data: {}", entry.name());
#ifdef HCM_DEBUG
            throw HCMInitException(err);
#else
            PLOG_ERROR << err;
#endif
        }

    }

}