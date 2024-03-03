# HaloCheckpointManager
*A practice tool for [Halo: The Master Chief Collection](https://store.steampowered.com/app/976730/Halo_The_Master_Chief_Collection/)*

---**Download**---  
From the [Releases page.](https://github.com/Burnt-o/HaloCheckpointManager/releases)  

---**Features**---
* Back up (dump) to file the games checkpoints, so you can inject them again at any later time. 
* UI for managing your checkpoint collection (ordering, sorting, renaming etc).
* Single-player cheats to help you practice and route the games, such as
   * Force custom checkpoints & back them up
   * Invincibility
   * Speedhack (both slow-down and fast-forward)
   * Block the games natural checkpoints
   * Teleport and Launch the player
   * And more (specific games have different features)
 * Support for multiple downpatched versions of MCC - Season 7 (2448), Season 8 (2645), and of course current patch.

---**Building from source**--- 
To build the source yourself, I recommended that you use the latest Visual Studio. You'll also need the following libraries: (it'll be easiest to grab them from vcpkg)

Install [vcpkg](https://learn.microsoft.com/en-us/vcpkg/get_started/get-started?source=recommendations&pivots=shell-cmd), then run the following commands:

    plog:x64-windows
    plog:x64-windows-static
    eventpp:x64-windows-static
    imgui[dx11-binding,win32-binding]:x64-windows-static
    pugixml:x64-windows-static
    curl[non-http,schannel,ssl,sspi]:x64-windows-static
    boost-stacktrace:x64-windows-static
    boost-algorithm:x64-windows-static
    boost-iostreams:x64-windows-static
    boost-assign:x64-windows-static
    boost-bimap:x64-windows-static
    magic-enum:x64-windows-static
    directxtk:x64-windows-static


This project also makes heavy use of the amazing SafetyHook by cursey. You can think of it as like "Microsoft Detours but if it didn't suck". I've included the release I'm using in the solution files. You'll also need the Windows 10 SDK.
