#include "pch.h"
#include "PointerManager.h"
#include "curl/curl.h" // to get PointerData.xml from github
#include <pugixml.hpp> // parsing xml
#include "InjectRequirements.h"
#include "MultilevelPointer.h"
#include "MidhookContextInterpreter.h"
#include "MidhookFlagInterpreter.h"
#define useDevPointerData 1
#define debugPointerManager 1

#ifndef HCM_DEBUG // don't use dev pointer data in release
#define useDevPointerData 0
#endif

typedef std::tuple<std::string, std::optional<GameState>> DataKey;



constexpr std::string_view xmlFileName = "InternalPointerData.xml";
constexpr std::string_view githubPath = "https://raw.githubusercontent.com/Burnt-o/HaloCheckpointManager/master/HCMInternal/InternalPointerData.xml";
                   

class PointerManager::PointerManagerImpl {


    private:
        std::shared_ptr<IGetMCCVersion> m_getMCCver;
        std::string m_dirPath;

        std::string pointerDataLocation;

        // Functions run by constructor, in order of execution
        void downloadXML(std::string_view url);
        std::string readLocalXML();
        void parseXML(std::string& xml);

        void processVersionedEntry(pugi::xml_node entry);
        void instantiateMultilevelPointer(pugi::xml_node entry, std::string entryType, DataKey dKey);
        void instantiateMidhookContextInterpreter(pugi::xml_node entry, DataKey dKey);
        template <typename T>
        void instantiateVectorFloat(pugi::xml_node entry, DataKey dKey);
        template <typename T>
        void instantiateVectorInteger(pugi::xml_node entry, DataKey dKey);
        void instantiateVectorString(pugi::xml_node entry, DataKey dKey);
        void instantiateInjectRequirements(pugi::xml_node entry, DataKey dKey);
        void instantiatePreserveLocations(pugi::xml_node entry, DataKey dKey);
        void instantiateOffsetLengthPair(pugi::xml_node entry, DataKey dKey);
        void instantiateInt64_t(pugi::xml_node entry, DataKey dKey);
        void instantiateDynStructOffsetInfo(pugi::xml_node entry, DataKey dKey);
        void instantiateMidhookFlagInterpreter(pugi::xml_node entry, DataKey dKey);



    public:
        PointerManagerImpl(std::shared_ptr<IGetMCCVersion> ver, std::string dirPath);
        ~PointerManagerImpl() = default;

        // data mapped by strings
        std::map<DataKey, std::any> mAllData{};
};



PointerManager::PointerManager(std::shared_ptr<IGetMCCVersion> ver, std::string dirPath) : impl(new PointerManagerImpl(ver, dirPath)) { PLOG_DEBUG << "PointerManager con"; };
PointerManager::~PointerManager() { PLOG_DEBUG << "~PointerManager"; }; // https://www.fluentcpp.com/2017/09/22/make-pimpl-using-unique_ptr/

PointerManager::PointerManagerImpl::PointerManagerImpl(std::shared_ptr<IGetMCCVersion> ver, std::string dirPath) : m_getMCCver(ver), m_dirPath(dirPath)
{

#if debugPointerManager == 0
    // set plog severity to info temporarily
    auto oldSeverity = plog::get()->getMaxSeverity();
    plog::get()->setMaxSeverity(plog::info);
#endif

    // Set pointerDataLocation 
    pointerDataLocation = m_dirPath;
    pointerDataLocation += xmlFileName;

    HCMInitException downloadFailure("not attempted");
#ifndef HCM_DEBUG

    try
    {
        PLOG_INFO << "downloading pointerData.xml";
        downloadXML(githubPath);
        downloadFailure = HCMInitException("download succeeded!");
    }
    catch (HCMInitException ex)
    {
        downloadFailure = ex;
        PLOG_ERROR << "Failed to download HCM PointerData xml, trying local backup";
    }
#endif

    std::string pointerData;
    try
    {
        pointerData = readLocalXML();
    }
    catch (HCMInitException ex)
    {
        throw HCMInitException(std::format("Failed to download or read local pointer data backup:\n"
            "Download attempt info: {}\n"
            "File read attempt info: {}\n"
            "", downloadFailure.what(), ex.what()
        ));
    }


    parseXML(pointerData);
    PLOG_DEBUG << "pointer parsing complete";
    //for (const auto& [key, value] : mMultilevelPointerData)
    //{
    //    PLOG_DEBUG << std::format("stored key: {}", key);
    //}


#if debugPointerManager == 0
    // set severity back to normal
    plog::get()->setMaxSeverity(oldSeverity);
#endif

}

template <typename T>
T PointerManager::getData(std::string dataName, std::optional<GameState> game)
{

    auto key = DataKey(dataName, game);
    auto altkey = DataKey(dataName, std::nullopt); // alternate key for non-game specific data
    // Check data exists
    if (!impl->mAllData.contains(key) )
    {
        if (!impl->mAllData.contains(altkey))
        {
            PLOG_ERROR << "no valid pointer data for " << dataName;
            throw HCMInitException(std::format("pointerData was null for {}", dataName));
        }
        else // grab altkey data
        {
            // Check correct type
            auto& type = impl->mAllData.at(altkey).type();
            if (!(typeid(T) == type))
            {
                throw HCMInitException(std::format("Invalid type access for {}\nType was {} but {} was requested", dataName, type.name(), typeid(T).name()));
            }

            return std::any_cast<T>(impl->mAllData.at(altkey));
        }

    }

    // Check correct type
    auto& type = impl->mAllData.at(key).type();
    if (!(typeid(T) == type))
    {
        throw HCMInitException(std::format("Invalid type access for {}\nType was {} but {} was requested", dataName, type.name(), typeid(T).name()));
    }

#ifdef HCM_DEBUG
    if (std::is_same_v<T, std::shared_ptr<MultilevelPointer>>
        && game != std::nullopt)
    {
            auto realOut = std::any_cast<std::shared_ptr<MultilevelPointer>>(impl->mAllData.at(key));

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
    
    return std::any_cast<T>(impl->mAllData.at(key));
}

template <>
std::shared_ptr<MultilevelPointerSpecialisation::BaseOffset> PointerManager::getData(std::string dataName, std::optional<GameState> game)
{
    auto out = std::dynamic_pointer_cast<MultilevelPointerSpecialisation::BaseOffset>(getData<std::shared_ptr<MultilevelPointer>>(dataName, game));
    if (out)
        return out;
    else
        throw HCMInitException("Bad multilevel pointer access! Was not BaseOffset specialisation");
}

template <>
std::shared_ptr<MultilevelPointerSpecialisation::ExeOffset> PointerManager::getData(std::string dataName, std::optional<GameState> game)
{
    auto out = std::dynamic_pointer_cast<MultilevelPointerSpecialisation::ExeOffset>(getData<std::shared_ptr<MultilevelPointer>>(dataName, game));
    if (out)
        return out;
    else
        throw HCMInitException("Bad multilevel pointer access! Was not ExeOffset specialisation");
}

template <>
std::shared_ptr<MultilevelPointerSpecialisation::ModuleOffset> PointerManager::getData(std::string dataName, std::optional<GameState> game)
{
    auto out = std::dynamic_pointer_cast<MultilevelPointerSpecialisation::ModuleOffset>(getData<std::shared_ptr<MultilevelPointer>>(dataName, game));
    if (out)
        return out;
    else
        throw HCMInitException("Bad multilevel pointer access! Was not ModuleOffset specialisation");
}

template <>
std::shared_ptr<MultilevelPointerSpecialisation::Resolved> PointerManager::getData(std::string dataName, std::optional<GameState> game)
{
    auto out = std::dynamic_pointer_cast<MultilevelPointerSpecialisation::Resolved>(getData<std::shared_ptr<MultilevelPointer>>(dataName, game));
    if (out)
        return out;
    else
        throw HCMInitException("Bad multilevel pointer access! Was not Resolved specialisation");
}


template <typename T>
std::shared_ptr<std::vector<T>> PointerManager::getVectorData(std::string dataName, std::optional<GameState> game)
{

    auto key = DataKey(dataName, game);
    auto altkey = DataKey(dataName, std::nullopt); // alternate key for non-game specific data
    // Check data exists
    if (!impl->mAllData.contains(key))
    {
        if (!impl->mAllData.contains(altkey))
        {
            PLOG_ERROR << "no valid pointer data for " << dataName;
            throw HCMInitException(std::format("pointerData was null for {}", dataName));
        }
        else // grab altkey data
        {
            // Check correct type
            auto& type = impl->mAllData.at(altkey).type();
            if (!(typeid(std::shared_ptr<std::vector<T>>) == type))
            {
                throw HCMInitException(std::format("Invalid type access for {}\nType was {} but {} was requested", dataName, type.name(), typeid(std::shared_ptr<std::vector<T>>).name()));
            }

  

            return std::any_cast<std::shared_ptr<std::vector<T>>>(impl->mAllData.at(altkey));
        }

    }

    // Check correct type
    auto& type = impl->mAllData.at(key).type();
    if (!(typeid(std::shared_ptr<std::vector<T>>) == type))
    {
        throw HCMInitException(std::format("Invalid type access for {}\nType was {} but {} was requested", dataName, type.name(), typeid(std::shared_ptr<std::vector<T>>).name()));
    }

    return std::any_cast<std::shared_ptr<std::vector<T>>>(impl->mAllData.at(key));
}

// explicit template instantiations of PointerManager::getData
template
std::shared_ptr<MultilevelPointer> PointerManager::getData(std::string dataName, std::optional<GameState> game);
template
std::shared_ptr<InjectRequirements> PointerManager::getData(std::string dataName, std::optional<GameState> game);
template
std::shared_ptr<PreserveLocations> PointerManager::getData(std::string dataName, std::optional<GameState> game);
template
std::shared_ptr<offsetLengthPair> PointerManager::getData(std::string dataName, std::optional<GameState> game);
template
std::shared_ptr<int64_t> PointerManager::getData(std::string dataName, std::optional<GameState> game);
template
std::shared_ptr<MidhookContextInterpreter> PointerManager::getData(std::string dataName, std::optional<GameState> game);
template
std::shared_ptr<MidhookFlagInterpreter> PointerManager::getData(std::string dataName, std::optional<GameState> game);
template
std::shared_ptr<DynStructOffsetInfo> PointerManager::getData(std::string dataName, std::optional<GameState> game);

template
std::shared_ptr<std::vector<byte>> PointerManager::getVectorData(std::string dataName, std::optional<GameState> game);

std::string PointerManager::PointerManagerImpl::readLocalXML()
{
    std::string pathToFile;

    pathToFile = pointerDataLocation;

    std::ifstream inFile(pathToFile.c_str());
    if (inFile.is_open())
    {
        std::stringstream buffer;
        buffer << inFile.rdbuf();
        inFile.close();
        PLOG_INFO << "local XML read succesfully!";
        return buffer.str();
    }
    else
    {
        std::stringstream er;
        er << "Failed to open file : " << GetLastError() << std::endl << "at: " << pathToFile;
        throw HCMInitException(er.str().c_str());
    }

}


#pragma region libcurl helpers
// write the data into a `std::string` rather than to a file.
std::size_t write_data(void* buf, std::size_t size, std::size_t nmemb,
    void* userp)
{
    if (auto sp = static_cast<std::string*>(userp))
    {
        sp->append(static_cast<char*>(buf), size * nmemb);
        return size * nmemb;
    }

    return 0;
}

// A deleter to use in the smart pointer for automatic cleanup
struct curl_dter {
    void operator()(CURL* curl) const
    {
        if (curl) curl_easy_cleanup(curl);
    }
};

// A smart pointer to automatically clean up out CURL session
using curl_uptr = std::unique_ptr<CURL, curl_dter>;
#pragma endregion //libcurl helpers

void PointerManager::PointerManagerImpl::downloadXML(std::string_view url)
{
    auto curl = curl_uptr(curl_easy_init());
    // Download from the intertubes
    std::string xmlString;
    if (curl)
    {
        curl_easy_setopt(curl.get(), CURLOPT_URL, url.data()); // curl is a c library, needs c strings
        curl_easy_setopt(curl.get(), CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, &xmlString);

        CURLcode ec;
        if ((ec = curl_easy_perform(curl.get())) != CURLE_OK)
            throw HCMInitException(std::format("Failed to download pointer data from url:\n{}\nError: {}", url, curl_easy_strerror(ec)));

    }
    else
    {
        throw HCMInitException(("Failed to init curl service!"));
    }

    // Write to local file
    std::string pathToFile = pointerDataLocation;
    std::ofstream outFile(pathToFile.c_str());
    if (outFile.is_open())
    {
        outFile << xmlString;
        outFile.close();
        PLOG_INFO << "local XML file written!";
        return;
    }
    else
    {
        std::stringstream er;
        er << "Failed to write pointer data file after downloading: " << GetLastError() << std::endl << "at: " << pathToFile;
        throw HCMInitException(er.str().c_str());
    }
    
}








void PointerManager::PointerManagerImpl::parseXML(std::string& xml)
{
    using namespace pugi;
    xml_document doc;
    xml_parse_result result = doc.load_string(xml.c_str());
    if (!result)
    {
        std::stringstream er;
        er << "XML [" << xml << "] parsed with errors, attr value: [" << doc.child("node").attribute("attr").value() << "]\n";
        er << "Error description: " << result.description() << "\n";
        er << "Error offset: " << result.offset << " (error at [..." << (xml.c_str() + result.offset) << "]\n\n";
        er << "xml in question: \n\n" << xml;
        throw HCMInitException(er.str());
    }

    xml_node root = doc.child("root");

    for (xml_node entry = root.first_child(); entry; entry = entry.next_sibling())
    //for (xml_node entry : root.children())
    {
        std::string entryName = entry.name();
        PLOG_DEBUG << "Processing entry, name: " << entryName;
        if (entryName == "VersionedEntry")
        {
                processVersionedEntry(entry);
        }
        else if (entryName == "SupportedGameVersions")
        {
            bool foundSupportedVersion = false;
            for (xml_node supportedVersion = entry.first_child(); supportedVersion; supportedVersion = supportedVersion.next_sibling())
            {
                std::string versionString = supportedVersion.text().as_string();
                if (versionString == m_getMCCver->getMCCVersionAsString())
                {
                    foundSupportedVersion = true;
                    break;
                }
            }

            if (!foundSupportedVersion)
            {
                throw HCMInitException(std::format("The current version of MCC ({}) is not yet supported by HCM! ", m_getMCCver->getMCCVersionAsString()) +
                    "\nYou'll have to wait for Burnt to update it if HCM just got a patch," +
                    "\nor if you're on an old MCC patch, kindly ask him to add support.");
            }
        }
        else
        {
            PLOG_ERROR << "Unexpected item in pointer data: " << entry.name();
        }

    }



}


void PointerManager::PointerManagerImpl::processVersionedEntry(pugi::xml_node entry)
{
    using namespace pugi;
    std::string entryName = entry.attribute("Name").value();
    std::optional<GameState> entryGame = std::nullopt;

    PLOG_DEBUG << "processing versionedEntry, name: " << entry.name();
    for (xml_node versionEntry = entry.first_child(); versionEntry; versionEntry = versionEntry.next_sibling())
    {


        if (strcmp(versionEntry.attribute("Version").value(), m_getMCCver->getMCCVersionAsString().data()) != 0 && (strcmp(versionEntry.attribute("Version").value(), "All") != 0)) // We only want the versionEntries of the current MCC version
        {
                PLOG_VERBOSE << "No version match";
                continue;
        }
        PLOG_DEBUG << "Matching version found";

        // check for steam vs winstore flag
        if (!versionEntry.attribute("ProcessType").empty())
        {
            std::string processTypeString = versionEntry.attribute("ProcessType").as_string();
            constexpr std::string_view steamString = "Steam";
            constexpr std::string_view winString = "WinStore";
            PLOG_DEBUG << "processTypeString.length()" << processTypeString.size();



            PLOG_DEBUG << "Checking process type";
            if (
                (m_getMCCver->getMCCProcessType() == MCCProcessType::Steam && strcmp(processTypeString.c_str(), steamString.data()) != 0)
                || (m_getMCCver->getMCCProcessType() == MCCProcessType::WinStore && strcmp(processTypeString.c_str(), winString.data()) != 0)
                )
            {
                PLOG_VERBOSE << "currentProcessType: " << magic_enum::enum_name(m_getMCCver->getMCCProcessType());
                PLOG_DEBUG << "wrong process type, value read: " << versionEntry.attribute("ProcessType").value();
                continue;
            }
        }

        // check for game specificity
        if (!versionEntry.attribute("Game").empty())
        {
            std::string gameString = versionEntry.attribute("Game").value();
            entryGame = GameState::fromString(gameString);
        }

        // Construct dataKey
        auto dKey = DataKey(entryName, entryGame);

                // check for duplicates
        if (mAllData.contains(dKey))
        {
            throw HCMInitException(std::format("Entry already exists in pointerData, entryName: {}, entryGame: {}\n", entryName, entryGame.has_value() ? entryGame.value().toString() : "AllVersions"));
        }

        std::string entryType = entry.attribute("Type").value(); // Convert to std::string
        // Check what type it is
        if (entryType.starts_with("MultilevelPointer"))
        {
            PLOG_DEBUG << "instantiating MultilevelPointer";
            instantiateMultilevelPointer(versionEntry, entryType, dKey);

        }
        else if (entryType.starts_with("MidhookContextInterpreter"))
        {
            PLOG_DEBUG << "instantiating MidhookContextInterpreter";
            instantiateMidhookContextInterpreter(versionEntry, dKey);
        }
        else if (entryType.starts_with("MidhookFlagInterpreter"))
        {
            PLOG_DEBUG << "instantiating MidhookFlagInterpreter";
            instantiateMidhookFlagInterpreter(versionEntry, dKey);
        }
        else if (entryType.starts_with("Vector"))
        {
            PLOG_DEBUG << "instantiating Vector";
            std::string typeName = entry.attribute("Typename").as_string();
            // I'll have to manually add each type I want to support here.
            if (typeName == nameof(byte))
                instantiateVectorInteger<byte>(versionEntry, dKey);
            else if (typeName == nameof(int))
                instantiateVectorInteger<int>(versionEntry, dKey);
            else if (typeName == nameof(long))
                instantiateVectorInteger<long>(versionEntry, dKey);
            else if (typeName == nameof(float))
                instantiateVectorFloat<float>(versionEntry, dKey);
            else if (typeName == nameof(double))
                instantiateVectorFloat<double>(versionEntry, dKey);
            else if (typeName == nameof(long double))
                instantiateVectorFloat<long double>(versionEntry, dKey);
            else if (typeName == nameof(std::string))
                instantiateVectorString(versionEntry, dKey);
            else
                throw HCMInitException(std::format("Unsupported typename passed to instantiateVector {}: {}", entryName, typeName));

        }
        else if (entryType.starts_with("injectRequirements"))
        {
            instantiateInjectRequirements(versionEntry, dKey);
        }
        else if (entryType.starts_with("preserveLocations"))
        {
            instantiatePreserveLocations(versionEntry, dKey);
        }
        else if (entryType.starts_with("offsetLengthPair"))
        {
            instantiateOffsetLengthPair(versionEntry, dKey);
        }
        else if (entryType.starts_with("int64_t"))
        {
            instantiateInt64_t(versionEntry, dKey);
        }
        else if (entryType.starts_with("DynStructOffsetInfo"))
        {
            instantiateDynStructOffsetInfo(versionEntry, dKey);
        }
    }
}

// Checks for "0x" to know if it's a hex string. Also checks for negatives ("-" at start)
int64_t stringToInt(std::string& string)
{
    std::string justNumbers = string;
    bool negativeFlag = string.starts_with("-");
    if (negativeFlag) justNumbers = justNumbers.substr(1); // remove the negative sign from string

    bool hexFlag = justNumbers.starts_with("0x");
    if (hexFlag) justNumbers = justNumbers.substr(2); // remove the hex sign from string


    try
    {
        int64_t result = stoi(justNumbers, 0, hexFlag ? 16 : 10);
        if (negativeFlag) result = -result;
        PLOG_VERBOSE << std::format("stringToInt converted {0} to {1}", string, result);
        return result;
    }
    catch (std::invalid_argument ex)
    {
        throw HCMInitException(std::format("Error parsing string to int: {}", string));
    }

}


std::vector<int64_t> getOffsetsFromXML(pugi::xml_node versionEntry)
{
    using namespace pugi;
    xml_node offsetArray = versionEntry.child("Offsets");
    std::vector<int64_t> result;

    for (xml_node offsetElement = offsetArray.first_child(); offsetElement; offsetElement = offsetElement.next_sibling())
    {
        std::string offsetString = offsetElement.text().get();
        result.emplace_back(stringToInt(offsetString));
    }

    return result;

}

enum class MultilevelPointerType {
    ExeOffset,
    ModuleOffset,
    BaseOffset,
    Invalid
};

typedef std::tuple<MultilevelPointerType, std::vector<int64_t>, std::string, bitOffsetT> MLPConstructionArgs;
MLPConstructionArgs getConstructionArgs(pugi::xml_node versionEntry, std::string entryType)
{
    bitOffsetT bitOffset = versionEntry.child("BitOffset").text().as_int(0); // defaults to zero if "BitOffset" not present

    if (entryType == "MultilevelPointer::ExeOffset")
    {
        PLOG_DEBUG << "exeOffset";
        auto offsets = getOffsetsFromXML(versionEntry);
        return MLPConstructionArgs(MultilevelPointerType::ExeOffset, offsets, "", bitOffset);

    }
    else if (entryType == "MultilevelPointer::ModuleOffset")
    {
        PLOG_DEBUG << "moduleOffset";
        std::string moduleString = versionEntry.child("Module").text().get();
        auto offsets = getOffsetsFromXML(versionEntry);

        //if (dKey._Get_rest()._Myfirst._Val.has_value() && dKey._Get_rest()._Myfirst._Val.value().toModuleName() != str_to_wstr(moduleString))
        //{
        //    PLOG_ERROR << "Module name did not match version entry game type! This is probably a typo on Burnts part: observed: "
        //        << moduleString << ", expected: " << dKey._Get_rest()._Myfirst._Val.value().toModuleName() << ", entryName: " << dKey._Myfirst._Val;
        //}

        return MLPConstructionArgs(MultilevelPointerType::ModuleOffset, offsets, moduleString, bitOffset);

    }
    else if (entryType == "MultilevelPointer::BaseOffset")
    {
        PLOG_DEBUG << "baseOffset";
        auto offsets = getOffsetsFromXML(versionEntry);
        return MLPConstructionArgs(MultilevelPointerType::BaseOffset, offsets, "", bitOffset);
    }
    else
    {
        PLOG_ERROR << "INVALID MULTILEVEL POINTER TYPE: " << entryType;
        return MLPConstructionArgs(MultilevelPointerType::Invalid, {}, "", bitOffset);
    }
}


void PointerManager::PointerManagerImpl::instantiateMultilevelPointer(pugi::xml_node versionEntry, std::string entryType, DataKey dKey)
{
    MLPConstructionArgs constructionArgs;
    if (entryType == "MultilevelPointer::Derived")
    {
        PLOG_VERBOSE << "constructing MultilevelPointer::Derived";

        // what's the name of the base MLP we're deriving from?
        std::string baseName = versionEntry.child("Base").text().get();
        auto baseMLP = versionEntry.root().first_child().find_child_by_attribute("Name", baseName.c_str()); // search the document to find it
        if (baseMLP == NULL)
        {
            PLOG_ERROR << "No base MLP existed by name " << baseName;
            return;
        }

        // filter base MLP to correct version/processType/game
        auto baseMLPVersionedEntry = baseMLP.find_child([this, derivedNode = versionEntry](pugi::xml_node baseNode)
            {
                PLOG_VERBOSE << "checking version";
                // version valid? 

                if (strcmp(baseNode.attribute("Version").value(), m_getMCCver->getMCCVersionAsString().data()) != 0 && (strcmp(baseNode.attribute("Version").value(), "All") != 0)) // We only want the versionEntries of the current MCC version
                {
                    return false;
                }

                PLOG_VERBOSE << "checking process";
                // process type valid?
                if (baseNode.attribute("ProcessType").empty() == false)
                {
                    std::string processTypeString = baseNode.attribute("ProcessType").as_string();
                    constexpr std::string_view steamString = "Steam";
                    constexpr std::string_view winString = "WinStore";

                    if (
                        (m_getMCCver->getMCCProcessType() == MCCProcessType::Steam && strcmp(processTypeString.c_str(), steamString.data()) != 0)
                        || (m_getMCCver->getMCCProcessType() == MCCProcessType::WinStore && strcmp(processTypeString.c_str(), winString.data()) != 0)
                        )
                    {
                        return false;
                    }
                }

                PLOG_VERBOSE << "checking game";
                if (baseNode.attribute("Game").empty() == false)
                {
                    PLOG_VERBOSE << "baseNode.attribute(Game).value()" << baseNode.attribute("Game").value();
                    PLOG_VERBOSE << "derivedNode.attribute(Game).value()" << derivedNode.attribute("Game").value();

                    if (strcmp(baseNode.attribute("Game").value(), derivedNode.attribute("Game").value()) != 0)
                        return false;
                }

                return true;
            });

        if (baseMLPVersionedEntry == NULL)
        {
            PLOG_ERROR << "Could not find matching base version entry";
            return;
        }

        // pass base MLP over to getConstructionArgs
        std::string baseEntryType = baseMLP.attribute("Type").value(); // Convert to std::string
        constructionArgs = getConstructionArgs(baseMLPVersionedEntry, baseEntryType);
#ifdef HCM_DEBUG
        std::stringstream ss;
        baseMLPVersionedEntry.print(ss);
        PLOG_VERBOSE << "baseMLPVersionedEntry: " << std::endl << ss.str();
        PLOG_VERBOSE << "baseEntryType: " << std::endl << baseEntryType;
#endif


        // loop over entries and apply adjustments/overrides
        for (auto adjustmentXML = versionEntry.first_child(); adjustmentXML; adjustmentXML = adjustmentXML.next_sibling())
        {
            std::string adjustmentXMLNameString = adjustmentXML.name();
            PLOG_VERBOSE << "interpreting derived entry: " << adjustmentXML.name();
            if (adjustmentXMLNameString == "IndexedOffsetAdjustment")
            {
                std::string offsetIndexString = adjustmentXML.child("Index").text().get();
                int offsetIndex = stringToInt(offsetIndexString);

                std::string offsetAdjustmentString = adjustmentXML.child("Adjustment").text().get();
                int offsetAdjustment = stringToInt(offsetAdjustmentString);

                // pad offsets to necessary size
                while (std::get<std::vector<int64_t>>(constructionArgs).size() < (offsetIndex + 1))
                {
                    std::get<std::vector<int64_t>>(constructionArgs).emplace_back(0);
                }

                // apply adjustment at index
                std::get<std::vector<int64_t>>(constructionArgs).at(offsetIndex) = std::get<std::vector<int64_t>>(constructionArgs).at(offsetIndex) + offsetAdjustment;
            }
            else if (adjustmentXMLNameString == "IndexedOffsetOverride")
            {
                std::string offsetIndexString = adjustmentXML.child("Index").text().get();
                int offsetIndex = stringToInt(offsetIndexString);

                std::string offsetOverrideString = adjustmentXML.child("Override").text().get();
                int offsetOverride = stringToInt(offsetOverrideString);

                // pad offsets to necessary size
                while (std::get<std::vector<int64_t>>(constructionArgs).size() < (offsetIndex + 1))
                {
                    std::get<std::vector<int64_t>>(constructionArgs).emplace_back(0);
                }

                // apply override at index
                std::get<std::vector<int64_t>>(constructionArgs).at(offsetIndex) = offsetOverride;
            }
            else if (adjustmentXMLNameString == "LastOffsetAdjustment")
            {
                PLOG_VERBOSE << "LastOffsetAdjustment: ";
                std::string offsetAdjustmentString = adjustmentXML.text().get();
                int offsetAdjustment = stringToInt(offsetAdjustmentString);
                PLOG_VERBOSE << offsetAdjustment;

                // apply override at last entry in offset vector
                PLOG_VERBOSE << "prev value: " << std::get<std::vector<int64_t>>(constructionArgs).back();
                std::get<std::vector<int64_t>>(constructionArgs).back() = std::get<std::vector<int64_t>>(constructionArgs).back() + offsetAdjustment;
                PLOG_VERBOSE << "new value: " << std::get<std::vector<int64_t>>(constructionArgs).back();
            }
            else if (adjustmentXMLNameString == "BitOffsetAdjustment")
            {
                std::string bitOffsetAdjustmentStr = adjustmentXML.child("Adjustment").text().get();
                int bitOffsetAdjustment = stringToInt(bitOffsetAdjustmentStr);

                // apply adjustment
                std::get<bitOffsetT>(constructionArgs) = std::get<bitOffsetT>(constructionArgs) + bitOffsetAdjustment;
            }
            else if (adjustmentXMLNameString == "BitOffsetOverride")
            {
                std::string bitOffsetOverrideStr = adjustmentXML.child("Override").text().get();
                int bitOffsetOverride = stringToInt(bitOffsetOverrideStr);

                // apply override
                std::get<bitOffsetT>(constructionArgs) = bitOffsetOverride;
            }
        }

    }
    else
    {
        constructionArgs = getConstructionArgs(versionEntry, entryType);
#ifdef HCM_DEBUG
        std::stringstream ss;
        versionEntry.print(ss);
        PLOG_VERBOSE << "versionEntry: " << std::endl << ss.str();
        PLOG_VERBOSE << "entryType: " << std::endl << entryType;
#endif
    }


    auto mlpType = std::get<MultilevelPointerType>(constructionArgs);
    std::shared_ptr<MultilevelPointer> result;
    switch (mlpType) // construction switch
    {
    case MultilevelPointerType::ExeOffset:
        result = std::make_shared<MultilevelPointerSpecialisation::ExeOffset>(std::get<std::vector<int64_t>>(constructionArgs), std::get<bitOffsetT>(constructionArgs));
        break;

    case MultilevelPointerType::ModuleOffset:
        result = std::make_shared<MultilevelPointerSpecialisation::ModuleOffset>(str_to_wstr(std::get<std::string>(constructionArgs)), std::get<std::vector<int64_t>>(constructionArgs), std::get<bitOffsetT>(constructionArgs));
        break;

    case MultilevelPointerType::BaseOffset:
        result = std::make_shared<MultilevelPointerSpecialisation::BaseOffset>(nullptr, std::get<std::vector<int64_t>>(constructionArgs), std::get<bitOffsetT>(constructionArgs));
        break;

    case MultilevelPointerType::Invalid:
        PLOG_ERROR << "Invalid multilevelpointer type for entry " << dKey._Myfirst._Val << ": " << entryType;
        return;
    }


    PLOG_DEBUG << entryType << " added to map: " << dKey._Myfirst._Val;
    mAllData.try_emplace(dKey, result);

}


void PointerManager::PointerManagerImpl::instantiateMidhookContextInterpreter(pugi::xml_node versionEntry, DataKey dKey)
{
    std::shared_ptr<MidhookContextInterpreter> result;
    std::vector<ParameterLocation> parameterRegisters;

    using namespace pugi;

    xml_node paramArray = versionEntry.first_child();
    for (xml_node parameter = paramArray.first_child(); parameter; parameter = parameter.next_sibling())
    {
        std::string parameterLocationText = parameter.text().as_string();

        auto mathSymbol = parameterLocationText.find_first_of('+');
        if (mathSymbol == std::string::npos)
        {
            mathSymbol = parameterLocationText.find_first_of('-');
        }
        
        std::string registerText = mathSymbol == std::string::npos ? parameterLocationText : parameterLocationText.substr(0, mathSymbol);
        PLOG_VERBOSE << "registerText: " << registerText;


        if (!stringToRegister.contains(registerText))
        {
            throw HCMInitException(std::format("invalid parameter string when parsing MidhookContextInterpreter->{}: {}", dKey._Myfirst._Val, parameterLocationText));
        }

        if (mathSymbol != std::string::npos)
        {
            PLOG_VERBOSE << "parsing RSP/RBP offset";
            // parsing the number from the string is a pain in the ass
            std::string offsetText = parameterLocationText.substr(mathSymbol, parameterLocationText.size());

            PLOG_VERBOSE << "offsetText " << offsetText;
            int offset = offsetText.contains("0x") ? stoi(offsetText, 0, 16) : stoi(offsetText);
            PLOG_VERBOSE << "offset " << offset;
            std::vector<int> offsets = { offset }; // TODO: rewrite this section to be capable of handling multiple levels of offsets
            parameterRegisters.push_back(ParameterLocation(stringToRegister.at(registerText), offsets));

        }
        else
        {
            parameterRegisters.push_back(ParameterLocation(stringToRegister.at(registerText)));
        }


    }

    if (parameterRegisters.empty())
    {
        throw HCMInitException(std::format("no parameter strings found when parsing MidhookContextInterpreter {}", dKey._Myfirst._Val));
    }

    result = std::make_shared<MidhookContextInterpreter>(parameterRegisters);

    PLOG_DEBUG << "MidhookContextInterpreter added to map: " << dKey._Myfirst._Val;
    mAllData.try_emplace(dKey, result);
   
}



void PointerManager::PointerManagerImpl::instantiateMidhookFlagInterpreter(pugi::xml_node versionEntry, DataKey dKey)
{
    std::shared_ptr<MidhookFlagInterpreter> result;

    using namespace pugi;

    std::string rflagText = versionEntry.child("rflag").text().as_string();
    bool boolValueToSet = versionEntry.child("value").text().as_bool();

    if (rflagText.empty()) throw HCMInitException(std::format("bad value for midhookFlagInterpreter, {}::{}", dKey._Get_rest()._Myfirst._Val.value().toString(), dKey._Myfirst._Val.c_str()));

    auto rflagParsed = magic_enum::enum_cast<MidhookFlagInterpreter::RFlag>(rflagText);
    if (!rflagParsed.has_value()) throw HCMInitException(std::format("bad value ({}) for midhookFlagInterpreter, {}::{}", rflagText, dKey._Get_rest()._Myfirst._Val.value().toString(), dKey._Myfirst._Val.c_str()));


    result = std::make_shared<MidhookFlagInterpreter>(rflagParsed.value(), boolValueToSet);


    PLOG_DEBUG << "MidhookFlagInterpreter added to map: " << dKey._Myfirst._Val;
    mAllData.try_emplace(dKey, result);

}

template <typename T>
void PointerManager::PointerManagerImpl::instantiateVectorInteger(pugi::xml_node versionEntry, DataKey dKey)
{
    std::shared_ptr<std::vector<T>> out = std::make_shared<std::vector<T>>();


    std::string s = versionEntry.first_child().text().as_string();
    PLOG_INFO << "instantiateVectorNumber: " << s;
    std::stringstream ss(s);

    PLOG_DEBUG << "instantiateVectorInteger processing string: " << s;
    for (std::string entry; std::getline( ss, entry, ',');)
    {
        std::erase_if(entry, [](const char& c) { return std::isspace(c); }); // remove whitespace
        PLOG_DEBUG << "instantiateVectorInteger processing split string: " << entry;
        auto number = stringToInt(entry);
        out->push_back((T)number);
    }
    PLOG_DEBUG << "instantiateVectorInteger emplacing vector of size: " << out->size();
    
    mAllData.try_emplace(dKey, out);

   

}


template <typename T>
void PointerManager::PointerManagerImpl::instantiateVectorFloat(pugi::xml_node versionEntry, DataKey dKey)
{
    std::shared_ptr<std::vector<T>> out = std::make_shared<std::vector<T>>();

    std::string s = versionEntry.first_child().text().as_string();
    PLOG_INFO << "instantiateVectorNumber: " << s;
    std::stringstream ss(s);

  
    for (std::string entry; std::getline(ss, entry, ',');)
    {
        std::erase_if(entry, [](const char& c) { return std::isspace(c); }); // remove whitespace
        // string to long double which we dynamic_cast down to T
        auto number = stold(entry);
        try
        {

            out->push_back((T)number);
        
        }
        catch (const std::bad_cast& e)
        {
            throw HCMInitException(std::format("Could not convert number to typename for entry {}: {}", dKey._Myfirst._Val, e.what()));
        }
    }
   
   
    PLOG_DEBUG << "instantiateVectorFloat emplacing vector of size: " << out->size();
    mAllData.try_emplace(dKey, out);



}


void PointerManager::PointerManagerImpl::instantiateVectorString(pugi::xml_node versionEntry, DataKey dKey)
{
    std::shared_ptr<std::vector<std::string>> out = std::make_shared<std::vector<std::string>>();


    for (pugi::xml_node stringEntry = versionEntry.first_child(); stringEntry; stringEntry = stringEntry.next_sibling())
    {
        std::string s = stringEntry.text().as_string();
        out->push_back(s);
    }

    mAllData.try_emplace(dKey, out);
}

void PointerManager::PointerManagerImpl::instantiateInjectRequirements(pugi::xml_node versionEntry, DataKey dKey)
{
    InjectRequirements working;

    enum string_to_enum {
        singleCheckpoint, preserveLocations, SHA, BSP
    };

    const static std::unordered_map<std::string, int> string_to_case{
        {"singleCheckpoint", singleCheckpoint},
        { "preserveLocations", preserveLocations },
        { "SHA", SHA },
        { "BSP", BSP },
    };

    for (pugi::xml_node boolEntry = versionEntry.first_child(); boolEntry; boolEntry = boolEntry.next_sibling())
    {
        std::string s = boolEntry.name();
        
        if (!string_to_case.contains(s)) throw HCMInitException(std::format("InjectionRequirements had a bad string: {}", s));

        switch (string_to_case.at(s))
        {
        case singleCheckpoint:
            working.singleCheckpoint = boolEntry.text().as_bool();
        case preserveLocations:
            working.preserveLocations = boolEntry.text().as_bool();
        case SHA:
            working.SHA = boolEntry.text().as_bool();
        case BSP:
            working.BSP = boolEntry.text().as_bool();
        }
    }

    mAllData.emplace(dKey, std::make_shared<InjectRequirements>(working));

}

void PointerManager::PointerManagerImpl::instantiateInt64_t(pugi::xml_node versionEntry, DataKey dKey)
{
    std::string text = versionEntry.first_child().text().as_string();
    auto working = stringToInt(text);

    mAllData.emplace(dKey, std::make_shared<int64_t>(working));
}

void PointerManager::PointerManagerImpl::instantiateOffsetLengthPair(pugi::xml_node versionEntry, DataKey dKey)
{
    std::string offsetText = versionEntry.child("offset").text().as_string();
    std::string lengthText = versionEntry.child("length").text().as_string();
    auto offset = stringToInt(offsetText);
    auto length = stringToInt(lengthText);

    if (length <= 0) throw HCMInitException("Cannot have non-positive offsetLengthPair length");

    offsetLengthPair working{ offset, length };

    mAllData.emplace(dKey, std::make_shared<offsetLengthPair>(working));
}

void PointerManager::PointerManagerImpl::instantiatePreserveLocations(pugi::xml_node versionEntry, DataKey dKey)
{

    std::shared_ptr<PreserveLocations> result = std::make_shared<PreserveLocations>();
    auto& plmap = result->locations; // ref to PreserveLocation map that we will insert into

    for (pugi::xml_node locationEntry = versionEntry.first_child(); locationEntry; locationEntry = locationEntry.next_sibling())
    {
        std::string offsetText = locationEntry.child("Offset").text().as_string(); //capitalised strings since I can't be bothered undoing from HCM2 pointerdata
        std::string lengthText = locationEntry.child("Length").text().as_string();
        auto offset = stringToInt(offsetText);
        auto length = stringToInt(lengthText);

        if (length <= 0) throw HCMInitException("Cannot have non-positive preserveLocation length");

        // construct a (empty) vector of length's length and emplace into map with offset as key
        std::vector<byte> vec;
        vec.resize(length, 0); // fill with zeroes
        plmap.emplace(offset, length);
    }

    mAllData.emplace(dKey, result);
}


void PointerManager::PointerManagerImpl::instantiateDynStructOffsetInfo(pugi::xml_node versionEntry, DataKey dKey)
{


    std::shared_ptr<DynStructOffsetInfo> result = std::make_shared<DynStructOffsetInfo>();

    for (pugi::xml_node locationEntry = versionEntry.first_child(); locationEntry; locationEntry = locationEntry.next_sibling())
    {
        std::string fieldName = locationEntry.name();
        std::string offsetText = locationEntry.text().as_string(); //capitalised strings since I can't be bothered undoing from HCM2 pointerdata
        int offset = stringToInt(offsetText);

        PLOG_VERBOSE << "fieldName: " << fieldName;
        PLOG_VERBOSE << "offset: " << offset;
        if (result->contains(fieldName)) throw HCMInitException(std::format("Duplicate fieldname: {}", fieldName));
        result->operator[](fieldName) = offset;

    }

    PLOG_DEBUG << "DynStructOffsetInfo added to map: " << dKey._Myfirst._Val;
    mAllData.try_emplace(dKey, result);
}
