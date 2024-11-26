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
    
HCM uses vcpkg manifest files for C++ dependecies. With Visual Studio this should all be automatic. 
I haven't tried building outside of this VS environment so YMMV if you try otherwise. 

This project also makes heavy use of the amazing SafetyHook by cursey. You can think of it as like "Microsoft Detours but if it didn't suck". I've included the release I'm using in this repository so you shouldn't need to worry about it.

After successfully building all projects (besides HCMInternalTests, that doesn't matter), make sure to rebuild the entire solution to ensure all build events fire correctly.

Known build issue: Your build events may fail to copy over certain files to the correct directory due to macro issues: fix this by adjusting the build event macros for each project to make sure they're outputting to HCMExternal's binary output directory.