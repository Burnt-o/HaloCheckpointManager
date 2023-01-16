# HaloCheckpointManager
*A practice tool for Halo: The Master Chief Collection*

---**Download**---  
From the [Releases page.](https://github.com/Burnt-o/HaloCheckpointManager/releases)  

---**Install**---  
No installation necessary, just extract the zip file somewhere that you have read/write permissions to (so nearly anywhere).  


---**Usage Notes**---  

 You need to run this tool with admin privileges.   

 You need to run MCC with EAC disabled.
 
 If MCC crashes every time you try to run HCM, try the "Disable Overlay" option in the HCM Settings tab.

 Backup saves and config files are stored locally to this tools executable file, so move them together if you want to keep things working. 

 If MCC recieves an update, the tool will break, but once I fix it the update will be automatic (you won't have to download a new release, usually anyway). 

 Some (rare) MCC updates will break previous checkpoints/saves, and you won't be able to use those old saves in future versions of MCC. 

 If you run into any issues, just ping me on Discord!  @**BurntKurisu**#6253 

---**Building and Contribution**---  
In 2023 I rewrote HCM from the ground up, both to add a lot of extra features (like the old "Checkpoint Trainer" features), as well as to clean up the code, and as a learning experience. I still have a lot of code that I'd like to go back through and clean up a bit, but it's a significant improvement over the original HCM. 

Anyway, to get a build running you'll need a few DLL's. HCMInternal can be built from the code here. BurntMemory can be built from [here](https://github.com/Burnt-o/BurntMemory). And finally you'll need to snag the SpeedHack.dll off of one of the release zips (or you can grab all 3 like such). The speedhack dll is a slightly modified version of [Letomainy's speedhack](https://github.com/Letomaniy/Speed-Hack), and I plan to move its functionality over into HCMInternal in the near-future. 

HCMInternal implements the DirectX12 overlay, which was a large part of the learning curve in this rewrite. It uses [Kiero](https://github.com/Rebzzel/kiero) to grab the address of the games Present and ResizeBuffers function, but doesn't actually implement the hook itself. This is because I noticed that depending on the order certain programs hook the Present function, there would be unwanted behaviour. For example if [OBS](https://obsproject.com/) hooked Present first, the overlay would not be captured by OBS as its hook would be *after* it in the function. So my solution was to reverse engineer MCC's own pointer to the Present function, and [rewrite that pointer](https://github.com/Burnt-o/HaloCheckpointManager/blob/HCM2/HCM3/Services/Trainer/InternalHack/SetupInternal.cs#L209) to go to HCMInternal's hookPresent instead, pre-empting any other hooks entirely. I don't know if that idea already has a name but I thought it was neat!

Again, I still have some code tidy up to do, but the gist of it is a WPF app that mostly adheres to [MVVM principles](https://www.c-sharpcorner.com/uploadfile/nipuntomar/mvvm-in-wpf/) (as best I learnt them over the last year anyway), which a large service layer that handles actually interacting with the external MCC exe. If you would like to contribute, feel free to pull request or even just hit me up on discord for more information! 
