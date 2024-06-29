#pragma once
#include "pugixml.hpp"
#include "IGetMCCVersion.h"
#include "GameState.h"
#include "PointerDataStore.h"
// s parses the InternalPointerData.xml into the data that will go into PointerDataStore.
class PointerDataParser
{
public:
	typedef std::tuple<std::string, std::optional<GameState>> DataKey;
private:

    // to prevent getting mixed up during dev
    //class VersionEntry : public pugi::xml_node {};
    //class VersionCollection : public pugi::xml_node {};

    typedef pugi::xml_node VersionEntry;
    typedef pugi::xml_node VersionCollection;
    
	// sub functions
    bool entryIsCorrectMCCVersion(VersionEntry versionEntry);
    bool entryIsCorrectProcessType(VersionEntry versionEntry);
    std::optional<GameState> getEntryGame(VersionEntry versionEntry);

    template <typename T> void emplaceObject(T& object, VersionEntry versionEntry);


    void processVersionedEntry(VersionEntry versionEntry, std::string versionEntryType, std::string versionEntryName, std::string versionEntryVectorType);

    enum class MultilevelPointerType {
        ExeOffset,
        ModuleOffset,
        BaseOffset,
        Invalid
    };

    typedef std::tuple<MultilevelPointerType, std::vector<int64_t>, std::string, bitOffsetT> MLPConstructionArgs;
    MLPConstructionArgs getConstructionArgs(VersionEntry versionEntry);
    void instantiateMultilevelPointer(VersionEntry versionEntry);

    void instantiateMidhookContextInterpreter(VersionEntry versionEntry);
    template <typename T>  void instantiateVectorFloat(VersionEntry versionEntry);
    template <typename T> void instantiateVectorInteger(VersionEntry versionEntry);
    void instantiateVectorString(VersionEntry versionEntry);
    void instantiateInjectRequirements(VersionEntry versionEntry);
    void instantiatePreserveLocations(VersionEntry versionEntry);
    void instantiateOffsetLengthPair(VersionEntry versionEntry);
    void instantiateInt64_t(VersionEntry versionEntry);
    void instantiateDynStructOffsetInfo(VersionEntry versionEntry);
    void instantiateMidhookFlagInterpreter(VersionEntry versionEntry);
    void instantiateLevelMapStringVector(VersionEntry versionEntry);


    //template <typename T> 
    //T instantiateObject(VersionEntry versionEntry);

    //template <typename T>  
    //std::vector<T> instantiateVectorFloat(VersionEntry versionEntry);
    //template <typename T> 
    //std::vector<T> instantiateVectorInteger(VersionEntry versionEntry);

	// data members
	std::shared_ptr<IGetMCCVersion> mGetMCCver;
	std::map<DataKey, std::any>& mDataStoreToLoad;
	const std::string& mXMLDocument;
public: 


	PointerDataParser(std::shared_ptr<IGetMCCVersion> ver, std::shared_ptr< PointerDataStore> pointerStore, const std::string& xmlDocument)
		: mGetMCCver(ver), mDataStoreToLoad(pointerStore->mDataStore), mXMLDocument(xmlDocument){}

	void parse();
};

