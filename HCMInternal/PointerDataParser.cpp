#include "pch.h"
#include "PointerDataParser.h"
#include "PointerDataParserInstantiators.h"

// Pugi is our xml library https://pugixml.org/

namespace PointerDataParser
{

    std::expected<std::vector<std::string>, std::string> parseSupportedMCCVersions(const std::string& xmlDocument) noexcept
    {
        using namespace pugi;
        xml_document doc;
        xml_parse_result result = doc.load_string(xmlDocument.c_str());
        if (!result)
        {
            std::stringstream er;
            er << "XML parsed with errors, attr value: [" << doc.child("node").attribute("attr").value() << "]\n";
            er << "Error description: " << result.description() << "\n";
            er << "Error offset: " << result.offset << " (error at [..." << (xmlDocument.c_str() + result.offset) << "]\n\n";
            er << "XML contents: \n\n" << xmlDocument.c_str();
            return std::unexpected(er.str());
        }

        xml_node root = doc.child("root");
        xml_node supportedGameVersions = root.child("SupportedGameVersions");

        if (!supportedGameVersions)
            return std::unexpected("Could not find document element by name SupportedGameVersions");

        std::vector<std::string> out;
        for (xml_node supportedVersion = supportedGameVersions.first_child(); supportedVersion; supportedVersion = supportedVersion.next_sibling())
        {
            out.push_back(supportedVersion.text().as_string());
        }
        return out;
    }

    std::expected<std::vector<std::string>, std::string> parseSupportedHCMVersions(const std::string& xmlDocument) noexcept
    {
        using namespace pugi;
        xml_document doc;
        xml_parse_result result = doc.load_string(xmlDocument.c_str());
        if (!result)
        {
            std::stringstream er;
            er << "XML parsed with errors, attr value: [" << doc.child("node").attribute("attr").value() << "]\n";
            er << "Error description: " << result.description() << "\n";
            er << "Error offset: " << result.offset << " (error at [..." << (xmlDocument.c_str() + result.offset) << "]\n\n";
            er << "XML contents: \n\n" << xmlDocument.c_str();
            return std::unexpected(er.str());
        }

        xml_node root = doc.child("root");
        xml_node supportedHCMVersions = root.child("SupportedHCMVersions");

        if (!supportedHCMVersions)
            return std::unexpected("Could not find document element by name SupportedGameVersions");

        std::vector<std::string> out;
        for (xml_node supportedVersion = supportedHCMVersions.first_child(); supportedVersion; supportedVersion = supportedVersion.next_sibling())
        {
            out.push_back(supportedVersion.text().as_string());
        }
        return out;
    }


    std::map<DataKey, std::any> parseVersionedData(std::shared_ptr<IGetMCCVersion> getMCCVer, const std::string& xmlDocument)
    {
        std::map<PointerDataParser::DataKey, std::any> out;

        using namespace pugi;
        using namespace PointerDataParser::detail;
        xml_document doc;
        xml_parse_result result = doc.load_string(xmlDocument.c_str());
        if (!result)
        {
            std::stringstream er;
            er << "XML parsed with errors, attr value: [" << doc.child("node").attribute("attr").value() << "]\n";
            er << "Error description: " << result.description() << "\n";
            er << "Error offset: " << result.offset << " (error at [..." << (xmlDocument.c_str() + result.offset) << "]\n\n";
            er << "XML contents: \n\n" << xmlDocument.c_str();
            throw HCMInitException(er.str());
        }

        xml_node root = doc.child("root");


        for (xml_node entry = root.first_child(); entry; entry = entry.next_sibling())
        {
            std::string entryName = entry.name();
            PLOG_DEBUG << "Processing entry : " << entryName;

            if (entryName == "VersionedEntry")
            {
                PLOG_DEBUG << "Processing Versioned Entry, name: " << entry.attribute("Name").value();
                // loop over all sub (version) entries, filter to correct MCC version, instantiate
                for (VersionEntry versionEntry = entry.first_child(); versionEntry; versionEntry = versionEntry.next_sibling())
                {
                    // Check that the entries MCC-version matches the current actual MCC version
                    // Check for Steam vs Winstore process type specificty
                    if (!entryIsCorrectMCCVersion(versionEntry, getMCCVer) || !entryIsCorrectProcessType(versionEntry, getMCCVer))
                        continue;

                    try
                    {
                        processVersionedEntry(versionEntry, entry.attribute("Type").value(), entry.attribute("Name").value(), entry.attribute("Typename").value(), getMCCVer, out);
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
            else if (entryName == "SupportedHCMVersions")
            {
                // do nothing
                continue;
            }
            else if (entryName == "SupportedGameVersions")
            {
                // do nothing
                continue;
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
        return out;


    }


    namespace detail
    {


        bool entryIsCorrectMCCVersion(VersionEntry entry, std::shared_ptr<IGetMCCVersion> getMCCVer)
        {
            std::string versionEntryMCCVersion = entry.attribute("Version").value();
            bool out = versionEntryMCCVersion == "All" || versionEntryMCCVersion == getMCCVer->getMCCVersionAsString();

            if (!out)
                PLOG_DEBUG << "Version mismatch: " << versionEntryMCCVersion;
            return out;
        }

        bool entryIsCorrectProcessType(VersionEntry entry, std::shared_ptr<IGetMCCVersion> getMCCVer)
        {
            if (entry.attribute("ProcessType").empty() == true) return true;

            std::string processTypeString = entry.attribute("ProcessType").as_string();
            bool out = getMCCVer->getMCCProcessTypeAsString() == processTypeString;

            if (!out)
                PLOG_DEBUG << "ProcessType mismatch: " << processTypeString;
            return out;
        }

        std::optional<GameState> getEntryGame(VersionEntry entry)
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
        void emplaceData(T& object, VersionEntry versionEntry, DataStoreRef dataStore)
        {


            VersionCollection versionCollection = versionEntry.parent();

            // key for map
            auto entryGame = getEntryGame(versionEntry);
            auto entryGameString = entryGame.has_value() ? entryGame.value().toString() : "All versions";
            auto dKey = DataKey(versionCollection.attribute("Name").value(), entryGame);

            // check for duplicates
            if (dataStore.contains(dKey))
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
                dataStore.try_emplace(dKey, object);
            }
            else
            {
                auto shared = std::make_shared<T>(object);
                dataStore.try_emplace(dKey, shared);
            }

        }


        void processVersionedEntry(VersionEntry versionEntry, std::string versionEntryType, std::string versionEntryName, std::string versionEntryVectorType, std::shared_ptr<IGetMCCVersion> getMCCVer, DataStoreRef dataStore)
        {
#define instantiateAndEmplace(dataType, versionEnt, dataSt) { auto data = instantiateData<dataType>(versionEnt); emplaceData(data, versionEnt, dataSt); }
#define instantiateAndEmplaceVec(dataType, versionEnt, dataSt) { auto data = instantiateVectorData<dataType>(versionEnt); emplaceData(data, versionEnt, dataSt); }

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
            {
                auto data = instantiateMultilevelPointer(versionEntry, getMCCVer);
                emplaceData(data, versionEntry, dataStore);
            }
                break;

            case PointerDataType::MidhookContextInterpreter:
            { auto data = instantiateData<std::shared_ptr<MidhookContextInterpreter>>(versionEntry); emplaceData(data, versionEntry, dataStore); }
                break;

            case PointerDataType::MidhookFlagInterpreter:
                instantiateAndEmplace(std::shared_ptr<MidhookFlagInterpreter>, versionEntry, dataStore)
                break;

            case PointerDataType::Vector:
                // I'll have to manually add each type I want to support here.
                if (versionEntryVectorType == nameof(byte))
                    instantiateAndEmplaceVec(byte, versionEntry, dataStore)
                else if (versionEntryVectorType == nameof(int))
                    instantiateAndEmplaceVec(int, versionEntry, dataStore)
                else if (versionEntryVectorType == nameof(long))
                    instantiateAndEmplaceVec(long, versionEntry, dataStore)
                else if (versionEntryVectorType == nameof(float))
                    instantiateAndEmplaceVec(float, versionEntry, dataStore)
                else if (versionEntryVectorType == nameof(double))
                    instantiateAndEmplaceVec(double, versionEntry, dataStore)
                else if (versionEntryVectorType == nameof(long double))
                    instantiateAndEmplaceVec(long double, versionEntry, dataStore)
                else if (versionEntryVectorType == nameof(std::string))
                    instantiateAndEmplace(std::shared_ptr<std::vector<std::string>>, versionEntry, dataStore)
                else
                    throw HCMInitException(std::format("Unsupported typename passed to instantiateVector {}: {}", versionEntryName, versionEntryVectorType));

                break;

            case PointerDataType::injectRequirements:
                instantiateAndEmplace(std::shared_ptr<InjectRequirements>, versionEntry, dataStore)
                break;

            case PointerDataType::preserveLocations:
                instantiateAndEmplace(std::shared_ptr<PreserveLocations>, versionEntry, dataStore)
                break;

            case PointerDataType::offsetLengthPair:
                instantiateAndEmplace(std::shared_ptr<offsetLengthPair>, versionEntry, dataStore)
                break;

            case PointerDataType::int64_t:
                instantiateAndEmplace(std::shared_ptr<int64_t>, versionEntry, dataStore)
                break;

            case PointerDataType::DynStructOffsetInfo:
                instantiateAndEmplace(std::shared_ptr<DynStructOffsetInfo>, versionEntry, dataStore)
                break;

            case PointerDataType::LevelMapStringVector:
                instantiateAndEmplace(std::shared_ptr<LevelMapStringVector>, versionEntry, dataStore)
                break;

            default:
            case PointerDataType::Invalid:
                throw HCMInitException(std::format("Bad Type in pointer data!: {}, on entry named {}", versionEntryType, versionEntryName));
                break;
            }
        }

    }

}



