using Serilog;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Linq;

namespace HCMExternal.Services.PointerData.Impl
{
    public class PointerDataService : IPointerDataService
    {
        private static bool xTypeIsYTypeOrDerived(Type x, Type y)
        {
            return x == y || x.IsAssignableFrom(y);
        }


        public Dictionary<string, Dictionary<string, object>> PointerData { get; init; }

        public Dictionary<(HaloGame, HaloProcessType, string), Dictionary<string, object>> GameProcessPointerData { get; init; }


        public string HighestSupportedMCCVersion { get; init; }
        public string HighestSupportedCartographerVersion { get; init; }
        public List<string> LatestHCMVersions { get; init; }
        public List<string> ObsoleteHCMVersions { get; init; }


        /// <summary>
        /// Public accessor for PointerData.
        /// </summary>
        /// <param name="pointerName">Name of the pointer.</param>
        /// <param name="pointerVersion">Which version of MCC do you need the data for?</param>
        /// <returns></returns>
        public T GetData<T>(string pointerName, string pointerVersion)
        {

            // find pointer name in data
            bool success = PointerData.TryGetValue(pointerName, out Dictionary<string, object>? pointerVersionDictionary);
            if (success && pointerVersionDictionary != null)
            {
                // test "All" version first
                success = pointerVersionDictionary.TryGetValue("All", out object? allPointer);
                if (success && allPointer != null)
                {
                    // check Type
                    if (xTypeIsYTypeOrDerived(typeof(T), allPointer.GetType()))
                        return (T)allPointer;
                    else
                        throw new Exception("Data type mismatch! You asked for " + typeof(T) + " but the pointer here was " + allPointer.GetType());
                }
                else
                {
                    // test specific requested version
                    success = pointerVersionDictionary.TryGetValue(pointerVersion, out object? pointer);
                    if (success && pointer != null)
                    {
                        // check Type
                        if (xTypeIsYTypeOrDerived(typeof(T), pointer.GetType()))
                            return (T)pointer;
                        else
                            throw new Exception("Data type mismatch! You asked for " + typeof(T) + " but the pointer here was " + pointer.GetType());
                    }
                }
            }

            throw new Exception("Could not find pointerData for name: " + pointerName + " ver: " + pointerVersion);
        }


        public T GetGameProcessData<T>(HaloProcessType processType, HaloGame game, string? pointerName, string? pointerVersion)
        {
            // find key in data
            bool success = GameProcessPointerData.TryGetValue((game, processType, pointerName), out Dictionary<string, object>? pointerVersionDictionary);
            if (success && pointerVersionDictionary != null)
            {
                // test "All" version first
                success = pointerVersionDictionary.TryGetValue("All", out object? allPointer);
                if (success && allPointer != null)
                {
                    // check Type
                    if (xTypeIsYTypeOrDerived(typeof(T), allPointer.GetType()))
                        return (T)allPointer;
                    else
                        throw new Exception("Data type mismatch! You asked for " + typeof(T) + " but the pointer here was " + allPointer.GetType());
                }
                else
                {
                    // test specific requested version
                    success = pointerVersionDictionary.TryGetValue(pointerVersion, out object? pointer);
                    if (success && pointer != null)
                    {
                        // check Type
                        if (xTypeIsYTypeOrDerived(typeof(T), pointer.GetType()))
                            return (T)pointer;
                        else
                            throw new Exception("Data type mismatch! You asked for " + typeof(T) + " but the pointer here was " + pointer.GetType());
                    }
                }
            }

            // failure
            throw new Exception("Could not find pointerData for name: " + pointerName + " ver: " + pointerVersion + " game: " + game + " processType: " + processType);
        }

    }

    public static class PointerDataServiceFactory
    {

        private static object ParseObject(XElement entry, string type, HaloGame? haloGame = null)
        {
            switch (type)
            {
                case "int":
                    return ParseHexNumberInt(entry.Element("Offset")?.Value ?? throw new Exception("Null offset string value!"));

                case "string":
                    return entry.Element("Value")?.Value ?? throw new Exception("Null string value!");

                case "MultilevelPointer":
                    return MultilevelPointer.FromXML(entry, haloGame ?? throw new Exception("Missing HaloGame field"));

                default:
                    throw new ArgumentException("Didn't recognise type of PointerData entry, " + type);
            }
        }



        private static int ParseHexNumberInt(string s)
        {
            if (s == "0x0" || s == "0")
                return 0;

            int sign = 1;
            if (s.StartsWith("-"))
            {
                sign = -1;
                s = s.Substring(1);
            }

            return s.StartsWith("0x") ?
                Convert.ToInt32(s.Substring(2), 16) * sign :
                Convert.ToInt32(s) * sign;
        }

        public static (PointerDataService, string) MakePointerDataService()
        {

            // grab embedded resource file in assembly
            var stream = Assembly.GetExecutingAssembly().GetManifestResourceStream("HCMExternal.ExternalPointerData.xml") ?? throw new Exception("Couldn't find pointerdata! Null ManifestResourceStream");

            StreamReader reader = new StreamReader(stream);
            string xml = reader.ReadToEnd();

            if (!xml.Any())
                throw new Exception("Couldn't find pointerdata! empty xml");

            // Deserialise
            XDocument doc = XDocument.Parse(xml);

            if (doc.Root == null)
                throw new Exception("Something went wrong parsing the Pointer data xml file; 'doc.Root' was null.");



            string outFailedReads = ""; // will be returned with PointerDataService

            Dictionary<string, Dictionary<string, object>> _pointerData = new();
            foreach (XElement entry in doc.Root.Elements("Entry"))
            {
                try
                {
                    string entryName = entry.Element("Name")?.Value ?? throw new Exception("xml entry was missing a name element!"); ;
                    string entryType = entry.Attribute("Type")?.Value ?? throw new Exception("xml entry " + entryName + " was missing a type attribute!");


                    foreach (XElement VersionEntry in entry.Elements("Version"))
                    {
                        string? entryVersion = VersionEntry.Attribute("Version")?.Value ?? throw new Exception("entryObject was somehow null at StoreObject");
                        object? entryObject = ParseObject(VersionEntry, entryType) ?? throw new Exception("entryObject was somehow null at StoreObject");


                        if (_pointerData.ContainsKey(entryName))
                        {
                            _pointerData[entryName].Add(entryVersion, entryObject);
                        }
                        else
                        {
                            Dictionary<string, object> versionDictionary = new()
                            {
                                { entryVersion, entryObject }
                            };
                            _pointerData.Add(entryName, versionDictionary);
                        }

                        Log.Verbose("Added new object to pointer dictionary, name: " + entryName + ", version: " + entryVersion + ", type: " + entryObject.GetType().ToString());
                    }
                }
                catch (Exception ex)
                {
                    outFailedReads = outFailedReads + "\n" + "Error processing an entry in pointerdata, " + ex.ToString() + ", entry.name: " + entry.Element("Name")?.Value + ", entry.Value: " + entry.Value;
                    Log.Error("Error processing an entry in pointerdata, " + ex.ToString());
                    continue;
                }

            }



            Dictionary<(HaloGame, HaloProcessType, string), Dictionary<string, object>> _gameprocesspointerData = new();
            foreach (XElement entry in doc.Root.Elements("GameProcessEntry"))
            {
                try
                {
                    string entryName = entry.Element("Name")?.Value ?? throw new Exception("xml entry was missing a name element!");
                    string entryType = entry.Attribute("Type")?.Value ?? throw new Exception("xml entry " + entryName + " was missing the type attribute!");
                    string entryGame = entry.Attribute("HaloGame")?.Value ?? throw new Exception("xml entry " + entryName + " was missing the game attribute!");
                    string entryProcess = entry.Attribute("HaloProcessType")?.Value ?? throw new Exception("xml entry " + entryName + " was missing the process attribute!");

                    (HaloGame, HaloProcessType, string) key = (HaloGameMethods.FromAcronym(entryGame), HaloProcessTypeMethods.FromString(entryProcess), entryName);


                    foreach (XElement VersionEntry in entry.Elements("Version"))
                    {
                        string? entryVersion = VersionEntry.Attribute("Version")?.Value ?? throw new Exception("entryObject was somehow null at StoreObject");
                        object? entryObject = ParseObject(VersionEntry, entryType, HaloGameMethods.FromAcronym(entryGame)) ?? throw new Exception("entryObject was somehow null at StoreObject");


                        if (_gameprocesspointerData.ContainsKey(key))
                        {
                            _gameprocesspointerData[key].Add(entryVersion, entryObject);
                        }
                        else
                        {
                            Dictionary<string, object> versionDictionary = new()
                            {
                                { entryVersion, entryObject }
                            };
                            _gameprocesspointerData.Add(key, versionDictionary);
                        }

                        Log.Verbose("Added new object to gameprocesspointer dictionary, name: " + entryName + ", version: " + entryVersion + ", type: " + entryObject.GetType().ToString());
                    }
                }
                catch (Exception ex)
                {
                    outFailedReads = outFailedReads + "\n" + "Error processing an entry in pointerdata, " + ex.ToString() + ", entry.name: " + entry.Element("Name")?.Value + ", entry.Value: " + entry.Value;
                    Log.Error("Error processing an entry in pointerdata, " + ex.ToString());
                    continue;
                }

            }


            return (new PointerDataService
            {
                PointerData = _pointerData,
                GameProcessPointerData = _gameprocesspointerData,
                HighestSupportedCartographerVersion = doc.Root.Element("HighestSupportedCartographerVersion")?.Value ?? throw new Exception("Missing entry 'HighestSupportedCartographerVersion'"),
                HighestSupportedMCCVersion = doc.Root.Element("HighestSupportedVersion")?.Value ?? throw new Exception("Missing entry 'HighestSupportedVersion'"),
                LatestHCMVersions = doc.Root.Element("LatestHCMVersions")?.Elements().Select(x => x.Value).ToList() ?? throw new Exception("Missing entry 'LatestHCMVersions'"),
                ObsoleteHCMVersions = doc.Root.Element("ObsoleteHCMVersions")?.Elements().Select(x => x.Value).ToList() ?? throw new Exception("Missing entry 'ObsoleteHCMVersions'"),
            },
        outFailedReads);

        }

    }

}

