using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BurntMemory;

namespace HCM3.Services.Trainer
{
    public class DetourInfoObject
    {
        public ReadWrite.Pointer OriginalCodeLocation { get; init; }
        public byte[] OriginalCodeBytes { get; init; }

        public int SizeToAlloc { get; init; }
        public string DetourCodeASM { get; init; }

        public string HookCodeASM { get; init; }



        public Dictionary<string, ReadWrite.Pointer> SymbolPointers { get; init; }


        public DetourInfoObject(ReadWrite.Pointer originalCodeLocation, byte[] originalCodeBytes, int sizeToAlloc, string detourCodeASM, string hookCodeASM, Dictionary<string, ReadWrite.Pointer> symbolPointers)
        { 
        OriginalCodeBytes = originalCodeBytes;
            OriginalCodeLocation = originalCodeLocation;
            DetourCodeASM = detourCodeASM;
            HookCodeASM = hookCodeASM;
            SymbolPointers = symbolPointers;
            SizeToAlloc = sizeToAlloc;
        }

    }
}
