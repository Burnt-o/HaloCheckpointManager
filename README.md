# HaloCheckpointManager
*A practice tool for [Halo: The Master Chief Collection](https://store.steampowered.com/app/976730/Halo_The_Master_Chief_Collection/)*

This is a work in progress rewrite of HCM. The goal is to move all of the cheat implementation from HCMExternal to HCMInternal. And generally simplifly/cleanup code. 
The role of HCMExternal will be exclusively to view and manage your checkpoint library, and to inject HCMInternal.dll into MCC. Everything else will be handled by HCMInternal.

**Building from source**: To build the source yourself you'll need the following libraries from vcpkg.

    plog:x64-windows
    plog:x64-windows-static
    eventpp:x64-windows-static
    imgui[dx11-binding,win32-binding]:x64-windows-static
    pugixml:x64-windows-static
    curl[non-http,schannel,ssl,sspi]:x64-windows-static
    rpclib:x64-windows
    rpclib:x64-windows-static
    boost-stacktrace:x64-windows-static
    boost-algorithm:x64-windows-static
    boost-iostreams:x64-windows-static
    boost-assign:x64-windows-static
    boost-bimap:x64-windows-static
    magic-enum:x64-windows-static
    directxtk:x64-windows-static
    stb:x64-windows-static


This project also makes heavy use of the amazing SafetyHook by cursey. You can think of it as like "Microsoft Detours but if it didn't suck". I've included the release I'm using in the solution files. You'll also need the Windows 10 SDK.
