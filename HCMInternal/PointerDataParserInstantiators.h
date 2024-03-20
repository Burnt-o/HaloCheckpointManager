#include "pch.h"
#include "PointerDataParser.h"
#include "InjectRequirements.h"
#include "MultilevelPointer.h"
#include "MidhookContextInterpreter.h"
#include "MidhookFlagInterpreter.h"
#include "MultilevelPointer.h"
#include "DynamicStructFactory.h"
using namespace pugi;

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


std::vector<int64_t> getOffsetsFromXML(xml_node versionEntry)
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


PointerDataParser::MLPConstructionArgs PointerDataParser::getConstructionArgs(VersionEntry versionEntry)
{
    bitOffsetT bitOffset = versionEntry.child("BitOffset").text().as_int(0); // defaults to zero if "BitOffset" not present

    std::string entryType = versionEntry.parent().attribute("Type").value();
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

        // safety check that module == game
        auto entryGame = getEntryGame(versionEntry);
        if (entryGame.has_value() && entryGame.value().toModuleName() != str_to_wstr(moduleString))
        {
            PLOG_ERROR << "Module name did not match version entry game type! This is probably a typo on Burnts part : observed:"
                << moduleString << ", expected: " << entryGame.value().toString();
            return MLPConstructionArgs(MultilevelPointerType::Invalid, {}, "", bitOffset);
        }


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


void PointerDataParser::instantiateMultilevelPointer(VersionEntry versionEntry)
{
    MLPConstructionArgs constructionArgs;
    VersionCollection versionCollection = versionEntry.parent();


    if (std::string(versionCollection.attribute("Type").value()) == "MultilevelPointer::Derived")
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
        VersionEntry baseVersionEntry = (VersionEntry)baseMLP.find_child([this, derivedVersionEntry = versionEntry](VersionEntry baseVersionEntry)
            {

                if (entryIsCorrectMCCVersion(baseVersionEntry) == false) return false;

                if (entryIsCorrectProcessType(baseVersionEntry) == false) return false;

                auto baseGame = getEntryGame(baseVersionEntry);
                if (baseGame.has_value() && baseGame.value() != getEntryGame(derivedVersionEntry)) return false;

                return true;
            });

        if (baseVersionEntry == NULL)
        {
            PLOG_ERROR << "Could not find matching base version entry";
            return;
        }

        // pass base MLP over to getConstructionArgs
        std::string baseEntryType = baseMLP.attribute("Type").value(); // Convert to std::string
        constructionArgs = getConstructionArgs(baseVersionEntry);



        // loop over entries and apply adjustments/overrides to constructionArgs
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
                std::string bitOffsetAdjustmentStr = adjustmentXML.text().get();
                int bitOffsetAdjustment = stringToInt(bitOffsetAdjustmentStr);

                // apply adjustment
                std::get<bitOffsetT>(constructionArgs) = std::get<bitOffsetT>(constructionArgs) + bitOffsetAdjustment;
            }
            else if (adjustmentXMLNameString == "BitOffsetOverride")
            {
                std::string bitOffsetOverrideStr = adjustmentXML.text().get();
                int bitOffsetOverride = stringToInt(bitOffsetOverrideStr);

                // apply override
                std::get<bitOffsetT>(constructionArgs) = bitOffsetOverride;
            }
        }

    }
    else
    {
        constructionArgs = getConstructionArgs(versionEntry);
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
        PLOG_ERROR << "Invalid multilevelpointer type for entry " << versionCollection.attribute("Name").value() << ": " << versionCollection.attribute("Type").value();
        return;
    }

    emplaceObject(result, versionEntry);

}


void PointerDataParser::instantiateMidhookContextInterpreter(VersionEntry versionEntry)
{
    std::shared_ptr<MidhookContextInterpreter> result;
    std::vector<ParameterLocation> parameterRegisters;


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
            throw HCMInitException(std::format("invalid parameter string {}", parameterLocationText));
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
        throw HCMInitException("no parameter strings found");
    }

    result = std::make_shared<MidhookContextInterpreter>(parameterRegisters);

    emplaceObject(result, versionEntry);

}



void PointerDataParser::instantiateMidhookFlagInterpreter(VersionEntry versionEntry)
{
    std::shared_ptr<MidhookFlagInterpreter> result;

    using namespace pugi;

    std::string rflagText = versionEntry.child("rflag").text().as_string();
    bool boolValueToSet = versionEntry.child("value").text().as_bool();

    if (rflagText.empty())
        throw HCMInitException("empty value for midhookFlagInterpreter rflagText");

    auto rflagParsed = magic_enum::enum_cast<MidhookFlagInterpreter::RFlag>(rflagText);
    if (!rflagParsed.has_value())
        throw HCMInitException(std::format("bad value for midhookFlagInterpreter: {}", rflagText));


    result = std::make_shared<MidhookFlagInterpreter>(rflagParsed.value(), boolValueToSet);

    emplaceObject(result, versionEntry);

}

template <typename T>
void PointerDataParser::instantiateVectorInteger(VersionEntry versionEntry)
{
    std::shared_ptr<std::vector<T>> result = std::make_shared<std::vector<T>>();


    std::string s = versionEntry.first_child().text().as_string();
    PLOG_INFO << "instantiateVectorNumber: " << s;
    std::stringstream ss(s);

    PLOG_DEBUG << "instantiateVectorInteger processing string: " << s;
    for (std::string entry; std::getline(ss, entry, ',');)
    {
        std::erase_if(entry, [](const char& c) { return std::isspace(c); }); // remove whitespace
        PLOG_DEBUG << "instantiateVectorInteger processing split string: " << entry;
        auto number = stringToInt(entry);
        result->push_back((T)number);
    }
    PLOG_DEBUG << "instantiateVectorInteger emplacing vector of size: " << result->size();

    emplaceObject(result, versionEntry);



}


template <typename T>
void PointerDataParser::instantiateVectorFloat(VersionEntry versionEntry)
{
    std::shared_ptr<std::vector<T>> result = std::make_shared<std::vector<T>>();

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

            result->push_back((T)number);

        }
        catch (const std::bad_cast& e)
        {
            throw HCMInitException(std::format("Could not convert number to typename: {}", e.what()));
        }
    }


    PLOG_DEBUG << "instantiateVectorFloat emplacing vector of size: " << result->size();
    emplaceObject(result, versionEntry);



}


void PointerDataParser::instantiateVectorString(VersionEntry versionEntry)
{
    std::shared_ptr<std::vector<std::string>> result = std::make_shared<std::vector<std::string>>();


    for (pugi::xml_node stringEntry = versionEntry.first_child(); stringEntry; stringEntry = stringEntry.next_sibling())
    {
        std::string s = stringEntry.text().as_string();
        result->push_back(s);
    }

    emplaceObject(result, versionEntry);
}

void PointerDataParser::instantiateInjectRequirements(VersionEntry versionEntry)
{
    InjectRequirements result;

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
            result.singleCheckpoint = boolEntry.text().as_bool();
        case preserveLocations:
            result.preserveLocations = boolEntry.text().as_bool();
        case SHA:
            result.SHA = boolEntry.text().as_bool();
        case BSP:
            result.BSP = boolEntry.text().as_bool();
        }
    }

    emplaceObject(result, versionEntry);

}

void PointerDataParser::instantiateInt64_t(VersionEntry versionEntry)
{
    std::string text = versionEntry.first_child().text().as_string();
    auto result = stringToInt(text);

    emplaceObject(result, versionEntry);
}

void PointerDataParser::instantiateOffsetLengthPair(VersionEntry versionEntry)
{
    std::string offsetText = versionEntry.child("offset").text().as_string();
    std::string lengthText = versionEntry.child("length").text().as_string();
    auto offset = stringToInt(offsetText);
    auto length = stringToInt(lengthText);

    if (length <= 0) throw HCMInitException("Cannot have non-positive offsetLengthPair length");

    offsetLengthPair result{ offset, length };

    emplaceObject(result, versionEntry);
}

void PointerDataParser::instantiatePreserveLocations(VersionEntry versionEntry)
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

    emplaceObject(result, versionEntry);
}


void PointerDataParser::instantiateDynStructOffsetInfo(VersionEntry versionEntry)
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

    emplaceObject(result, versionEntry);
}
//
//template <typename T>
//T PointerDataParser::instantiateObject<std::shared_ptr<DynStructOffsetInfo>>(VersionEntry versionEntry)
//{
//    std::shared_ptr<DynStructOffsetInfo> result = std::make_shared<DynStructOffsetInfo>();
//
//    for (pugi::xml_node locationEntry = versionEntry.first_child(); locationEntry; locationEntry = locationEntry.next_sibling())
//    {
//        std::string fieldName = locationEntry.name();
//        std::string offsetText = locationEntry.text().as_string(); //capitalised strings since I can't be bothered undoing from HCM2 pointerdata
//        int offset = stringToInt(offsetText);
//
//        PLOG_VERBOSE << "fieldName: " << fieldName;
//        PLOG_VERBOSE << "offset: " << offset;
//        if (result->contains(fieldName)) throw HCMInitException(std::format("Duplicate fieldname: {}", fieldName));
//        result->operator[](fieldName) = offset;
//
//    }
//    return result;
//}

