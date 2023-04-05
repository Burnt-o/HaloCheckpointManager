using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BurntMemory;

namespace HCM3.Services.HaloMemory
{
    //Similiar to a DetourInfoObject but much simplified; contains just a ReadWrite.Pointer, the original code bytes, and the patched code bytes
    public class PatchInfo
    {
        public ReadWrite.Pointer OriginalCodeLocation { get; init; }
        public byte[] OriginalCodeBytes { get; init; }
        public byte[] PatchedCodeBytes { get; init; }

        public PatchInfo(ReadWrite.Pointer originalCodeLocation, byte[] originalCodeBytes, byte[] patchedCodeBytes)
        { 
        OriginalCodeBytes = originalCodeBytes;
            OriginalCodeLocation = originalCodeLocation;
            PatchedCodeBytes = patchedCodeBytes;
        }

    }
}
