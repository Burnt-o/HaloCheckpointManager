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
To build the source yourself, grab the latest version of [Visual Studio](https://visualstudio.microsoft.com/) and the following workloads/components:  
 * .NET desktop development (and .NET 7.0 runtime)  
 * Desktop development with C++  
 * Windows 10 SDK (or higher)
    
You'll also need several C++ libraries for HCMInternal and HCMInterproc to compile. It'll be easiest to install them using vcpkg.
       
[Install vcpkg](https://learn.microsoft.com/en-us/vcpkg/get_started/get-started?source=recommendations&pivots=shell-cmd) by:   
cloning the vcpkg repository to some new folder (you'll probably need a few dozen gigs of space free),  
running the bootstrap script,   
adding vcpkg to your system path variable,  
and running "vcpkg integrate install" to integrate it with your Visual Studio.  

Then run the following commands one at a time:

vcpkg install plog:x64-windows  
vcpkg install boost-interprocess:x64-windows
vcpkg install openssl:x64-windows-static
vcpkg install ms-gsl:x64-windows-static
vcpkg install plog:x64-windows-static  
vcpkg install eventpp:x64-windows-static  
vcpkg install imgui[dx11-binding,win32-binding]:x64-windows-static  
vcpkg install pugixml:x64-windows-static  
vcpkg install curl[non-http,schannel,ssl,sspi]:x64-windows-static  
vcpkg install boost-stacktrace:x64-windows-static  
vcpkg install boost-algorithm:x64-windows-static  
vcpkg install boost-iostreams:x64-windows-static  
vcpkg install boost-assign:x64-windows-static  
vcpkg install boost-bimap:x64-windows-static  
vcpkg install boost-interprocess:x64-windows-static  
vcpkg install magic-enum:x64-windows-static  
vcpkg install directxtk:x64-windows-static  

This project also makes heavy use of the amazing SafetyHook by cursey. You can think of it as like "Microsoft Detours but if it didn't suck". I've included the release I'm using in this repository so you shouldn't need to worry about it.

After successfully building all projects (besides HCMInternalTests, that doesn't matter), make sure to rebuild the entire solution to ensure all build events fire correctly.

Known build issue: Your build events may fail to copy over certain files to the correct directory due to macro issues: fix this by adjusting the build event macros for each project to make sure they're outputting to HCMExternal's binary output directory.