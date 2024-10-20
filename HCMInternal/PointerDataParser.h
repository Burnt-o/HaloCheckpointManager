#pragma once
#include "pugixml.hpp"
#include "IGetMCCVersion.h"
#include "GameState.h"
#include "MultilevelPointer.h"

namespace PointerDataParser
{

	typedef std::tuple<std::string, std::optional<GameState>> DataKey;

	// statically parses the InternalPointerData.xml into the dataMap that will go into PointerDataStore.
	// throws HCMInitExceptions if something goes wrong
	std::map<DataKey, std::any> parseVersionedData(std::shared_ptr<IGetMCCVersion> getMCCVer, const std::string& xmlDocument);

	std::expected<std::set<std::string>, std::string> parseSupportedMCCVersions(const std::string& xmlDocument) noexcept;

	std::expected<std::set<std::string>, std::string> parseSupportedHCMVersions(const std::string& xmlDocument) noexcept;

	namespace detail
	{
		using namespace pugi;

		typedef std::tuple<std::string, std::optional<GameState>> DataKey;
		typedef std::map<DataKey, std::any> DataStore;
		typedef std::map<DataKey, std::any>& DataStoreRef;

		typedef pugi::xml_node VersionEntry;
		typedef pugi::xml_node VersionCollection;

		// sub functions
		bool entryIsCorrectMCCVersion(VersionEntry versionEntry, std::shared_ptr<IGetMCCVersion> getMCCVer);
		bool entryIsCorrectProcessType(VersionEntry versionEntry, std::shared_ptr<IGetMCCVersion> getMCCVer);
		std::optional<GameState> getEntryGame(VersionEntry versionEntry);

		// put an object into the data map
		template <typename T> void emplaceData(T& data, VersionEntry versionEntry, DataStoreRef dataStore);

		// this function is run on each versionEntry in the document, instantiating whichever data type is represented
		void processVersionedEntry(VersionEntry versionEntry, std::string versionEntryType, std::string versionEntryName, std::string versionEntryVectorType, std::shared_ptr<IGetMCCVersion> getMCCVer, DataStoreRef dataStore);


		// MultilevelPointers have sub-types that require different data fields & constructor args
		enum class MultilevelPointerType {
			ExeOffset,
			ModuleOffset,
			BaseOffset,
			Invalid
		};

		typedef std::tuple<MultilevelPointerType, std::vector<int64_t>, std::string, bitOffsetT> MLPConstructionArgs;
		MLPConstructionArgs getConstructionArgs(VersionEntry versionEntry);
		std::shared_ptr<MultilevelPointer> instantiateMultilevelPointer(VersionEntry versionEntry, std::shared_ptr<IGetMCCVersion> getMCCVer);


		template <typename T> T instantiateData(VersionEntry versionEntry);

		template <typename T> std::shared_ptr<std::vector<T>> instantiateVectorData(VersionEntry versionEntry);





	}

};






