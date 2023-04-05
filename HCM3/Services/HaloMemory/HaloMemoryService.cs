using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BurntMemory;
using HCM3.Models;
using System.Diagnostics;
using HCM3.Helpers;
using System.Threading;
using System.IO;
using System.Xml.Linq;
using HCM3.Services.Internal;


namespace HCM3.Services.HaloMemory
{
    // Partial class so we can split off the detour methods to seperate files
    public partial class HaloMemoryService
    {
        public HaloMemoryManager HaloMemoryManager;
        public ReadWrite ReadWrite;
        public SpeedhackManager SpeedhackManager;
        public DataPointersService DataPointersService;
        public PatchManager PatchManager;
        public InternalServices InternalServices; // passed by InternalServices constructor

        public HaloMemoryService(DataPointersService dataPointersService, HotkeyManager hotkeyManager)
        {
            this.DataPointersService = dataPointersService;
            HaloMemoryManager = new HaloMemoryManager(dataPointersService, this, hotkeyManager);
        ReadWrite = new ReadWrite(HaloMemoryManager);
        SpeedhackManager = new SpeedhackManager(HaloMemoryManager, ReadWrite);
            this.PatchManager = new(this, dataPointersService);
        }

        public IntPtr GetPlayerVehiObjectAddress(HaloState haloState)
        {

            if (haloState.GameState == GameStateEnum.Halo1)
            {
                List<string> requiredPointerNames = new();
                requiredPointerNames.Add($"{haloState.GameState.To2Letters()}_PlayerDatum");
                requiredPointerNames.Add($"{haloState.GameState.To2Letters()}_PlayerData_VehicleIndex");
                requiredPointerNames.Add($"{haloState.GameState.To2Letters()}_PlayerData_VehicleNullValue");

                Dictionary<string, object> requiredPointers = this.DataPointersService.GetRequiredPointers(requiredPointerNames, haloState);

                uint playerDatum = this.ReadWrite.ReadInteger((ReadWrite.Pointer)requiredPointers["PlayerDatum"]).Value;
                IntPtr playerAddy = GetAddressFromDatum(haloState, playerDatum);


                //check if player in vehicle, if so we want to modify that vehicle instead of the player
                uint playerVehicleDatum = (uint)this.ReadWrite.ReadInteger(IntPtr.Add(playerAddy, (int)requiredPointers["PlayerData_VehicleIndex"]));
                if (playerVehicleDatum != (uint)requiredPointers["PlayerData_VehicleNullValue"])
                {
                    playerAddy = GetAddressFromDatum(haloState, playerVehicleDatum);
                }

                Trace.WriteLine("PlayerVehiObjectAddress: " + playerAddy.ToString("X"));

                return playerAddy;
            }
            else
            {
                if (this.PatchManager.ListOfPatches[$"{haloState.GameState.To2Letters()}PlayerData"].PatchHandle == null || !this.PatchManager.IsPatchApplied(haloState, $"{haloState.GameState.To2Letters()}PlayerData", true))
                {
                    if (this.PatchManager.IsPatchApplied(haloState, $"{haloState.GameState.To2Letters()}PlayerData", true))
                    {
                        this.PatchManager.RemovePatch(haloState, $"{haloState.GameState.To2Letters()}PlayerData", true);
                    }
                    this.PatchManager.ApplyPatch(haloState, $"{haloState.GameState.To2Letters()}PlayerData", true);
                    Thread.Sleep(30);
                }

                IntPtr? detourHandle = this.PatchManager.ListOfPatches[$"{haloState.GameState.To2Letters()}PlayerData"].PatchHandle;
                if (detourHandle == null) throw new Exception($"Couldn't get detourHandle of {haloState.GameState.To2Letters()}PlayerData");
                IntPtr? playerAddy = (IntPtr?)ReadWrite.ReadQword(detourHandle.Value + 0x50);
                if (playerAddy == null) throw new Exception($"couldn't parse player addy from detour handle, t. {haloState.GameState.To2Letters()}PlayerData");
                Trace.WriteLine($"{haloState.GameState.To2Letters()} player info test: " + playerAddy.Value);
                return playerAddy.Value;
            }
           


        }

        public IntPtr GetAddressFromDatum(HaloState haloState, uint playerDatum)
        {
            switch (haloState.GameState)
            {
                case GameStateEnum.Halo1:
                    ReadWrite.Pointer EntityOffsetTablePointer_H1 = (ReadWrite.Pointer)this.DataPointersService.GetRequiredPointers($"H1_EntityOffsetTable", haloState);
                    IntPtr EntityOffsetTable_H1 = (IntPtr)this.ReadWrite.ResolvePointer(EntityOffsetTablePointer_H1);
                    Trace.WriteLine("EntityOffsetTable_H1: " + EntityOffsetTable_H1.ToString("X"));
                    ushort lowDatum_H1 = Convert.ToUInt16(playerDatum & 0xFFFF); // get lowest 2 bytes of playerDatum
                    int thingy_H1 = lowDatum_H1 * 0x0C;
                    Trace.WriteLine("lowDatum_H1: " + lowDatum_H1.ToString("X"));
                    Trace.WriteLine("thingy_H1: " + thingy_H1.ToString("X"));


                    int PlayerOffsetIndex_H1 = (int)this.DataPointersService.GetRequiredPointers($"H1_PlayerOffsetIndex", haloState); // 0x40
                    IntPtr PlayerOffsetPointer_H1 = IntPtr.Add(EntityOffsetTable_H1, (thingy_H1 + PlayerOffsetIndex_H1));
                    Trace.WriteLine("playerOffsetPointer_H1: " + PlayerOffsetPointer_H1.ToString("X"));
                    int PlayerOffset_H1 = (int)this.ReadWrite.ReadInteger(new ReadWrite.Pointer(PlayerOffsetPointer_H1)).Value;


                    Trace.WriteLine("PlayerOffset_H1: " + PlayerOffset_H1.ToString("X"));
                    ReadWrite.Pointer EntityTablePointer_H1 = (ReadWrite.Pointer)this.DataPointersService.GetRequiredPointers($"H1_EntityTable", haloState);
                    IntPtr EntityTable_H1 = (IntPtr)this.ReadWrite.ResolvePointer(EntityTablePointer_H1);

                    int PlayerAddyIndex_H1 = (int)this.DataPointersService.GetRequiredPointers($"H1_PlayerAddyIndex", haloState); // 0x34
                    IntPtr playerAddy_H1 = IntPtr.Add(EntityTable_H1, (PlayerOffset_H1 + PlayerAddyIndex_H1));
                    return playerAddy_H1;
                    break;


                case GameStateEnum.Halo2:
                    ReadWrite.Pointer EntityOffsetTablePointer_H2 = (ReadWrite.Pointer)this.DataPointersService.GetRequiredPointers($"H2_EntityOffsetTable", haloState); //  new ReadWrite.Pointer("halo2.dll", new int[] { 0x1783E50, 0x58});
                    IntPtr EntityOffsetTable_H2 = (IntPtr)this.ReadWrite.ResolvePointer(EntityOffsetTablePointer_H2);

                    ushort lowDatum2 = Convert.ToUInt16(playerDatum & 0xFFFF); // get lowest 2 bytes of playerDatum
                    EntityOffsetTable_H2 = IntPtr.Add(EntityOffsetTable_H2, ((lowDatum2 * 0x0C) + 08));

                    uint PlayerOffsetIndex_H2 = this.ReadWrite.ReadInteger(EntityOffsetTable_H2).Value;

                    ReadWrite.Pointer EntityTablePointer_H2 = (ReadWrite.Pointer)this.DataPointersService.GetRequiredPointers($"H2_EntityTable", haloState); // new ReadWrite.Pointer("halo2.dll", new int[] { 0x1783E68, 0x57 });
                    IntPtr EntityTable_H2 = (IntPtr)this.ReadWrite.ResolvePointer(EntityTablePointer_H2);
                    //round down EntityTable last 4 bits
                    EntityTable_H2 = (IntPtr)(EntityTable_H2.ToInt64() & ~0xF);

                    return IntPtr.Add(EntityTable_H2, (int)PlayerOffsetIndex_H2);
                    break;

                default:
                    throw new Exception("GetPlayerAddress fed invalid game");

            }

            throw new NotImplementedException();
        }

        public string GetCoreBinPath()
        {
            Dictionary<string, string> coreBinFolderData = new();

            string coreBinFolderDataLocation = Directory.GetCurrentDirectory() + "\\CoreBinFolder.xml";
            if (File.Exists(coreBinFolderDataLocation))
            {
                try
                {
                    string xml = File.ReadAllText(coreBinFolderDataLocation);
                    XDocument doc = XDocument.Parse(xml);

                    foreach (XElement entry in doc.Root.Elements())
                    {
                        string? key = entry.Name.ToString();
                        string? value = entry.Value.ToString();

                        if (key != null && value != null)
                        {
                            coreBinFolderData.Add(key, value);
                        }
                    }
                }
                catch
                { }
            }

            string? currentVersion = this.HaloMemoryManager.CurrentAttachedMCCVersion;

            if (currentVersion == null) throw new Exception("Failed to get corebinfolder path - wasn't attached to MCC");

            if (coreBinFolderData.ContainsKey(currentVersion))
            {
                return coreBinFolderData[currentVersion];
            }

            //else, let's try to autodetect the path
            Process mcc = Process.GetProcessById((int)this.HaloMemoryManager.ProcessID);
            string mccExePath = mcc.MainModule.FileName;

            Trace.WriteLine("MCC EXE PATH: " + mccExePath);
            string pathToLookFor = @"mcc\binaries\win64\MCC-Win64-Shipping.exe";
            if (!mccExePath.Contains(pathToLookFor, StringComparison.OrdinalIgnoreCase)) throw new Exception("Couldn't evaluate path of MCC process, actual: " + mccExePath + "\nThis is an error with trying to inject core saves. Let Burnt know, \nin the meanwhile use checkpoint injection instead.");


            mccExePath = mccExePath.Replace(pathToLookFor, @"core\core.bin", StringComparison.OrdinalIgnoreCase);

            if (!File.Exists(mccExePath)) throw new Exception("Couldn't autodetect core.bin folder path");

            coreBinFolderData.Add(currentVersion, mccExePath);

            SerialiseCoreBinData(coreBinFolderDataLocation, coreBinFolderData);

            return mccExePath;


        }

        private void SerialiseCoreBinData(string coreBinFolderDataLocation, Dictionary<string, string> coreBinFolderData)
        {
            if (File.Exists(coreBinFolderDataLocation)) File.Delete(coreBinFolderDataLocation);

            using (TextWriter writer = File.CreateText(coreBinFolderDataLocation))
            {
                writer.WriteLine("<?xml version=\"1.0\" encoding=\"utf - 8\" ?>");
                writer.WriteLine("<Root>");

                foreach (KeyValuePair<string, string> kvp in coreBinFolderData)
                {
                    writer.WriteLine($"<{kvp.Key}>{kvp.Value}</{kvp.Key}>");

                }
                writer.WriteLine("</Root>");
            }


        }

    }
}
