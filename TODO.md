## 3.1.8 TODO:
* Finish Project Cartographer Save Management support
	* Inject/Dump checkpoint
		* inject requirements
		* Preserve locations
		* Offsets for checkpoint location
		* Offsets for loaded bsp in cp and exe
	* Hotkeys (adapt from https://github.com/Burnt-o/HaloCheckpointManager/blob/HCM2/HCM3/Services/HotkeyManager.cs)*
* Fix shield input printer messageGUI
* Fix subpixel ID copy paste/ button/ enter value etc bugs




## Bugs to fix (eventually):
* Pause game-advance ticks occasionally breaks (ScopedServiceRequest stuff bugging out probably).
* h3/odst triggers/camera flickering (did I fix this? I can't remember. prob an issue with camera data pointer or setCameraData hook).
* Freecam/speedhack occasional crashes - try step with debugger. Probably a hooking-while-executed issue that would require a more complicated hooking-rework (see code cleanup)
* Freecam still doesn't properly interact with FPS 
* Fix modal dialogs preventing alt tabs
* Fix coop issues (display info doesn't work according to harc) - this requires a larger rework of how we handle player data (see code cleanup)
* Fix GUIElement height calculation issues (eg freecamera simple/advanced subheadings cause the gui to freak out). There's gotta be a better structural solution than the manual calculation I do rn? Need to read the imgui docs more lol. Will probably need to rework how my "group" GUI elements work.
* Settings don't serialise on forceful shutdown or crash.

## Features to implement (eventually):
* Investigate project cartographer support. This would require a LARGE rework of a lot of code.
* AI ignore player cheat
* FreeCamera automation stuff 
* 3d object info overlay (like what Scales old HaloTAS used to have)
* HCMExternal filterSearch thingy
* Add loading the correct level if possible on inject / change level feature in general
* Display2dinfo H2 ghosting info (desync/difference between quake and havok positions - I've tried but still can't figure out how exactly ghosting works in H2)



## Code cleanup to do (eventually):
* Unmacro the OptionalCheatManager/HotkeyManager shennanigans. That code is seriously bad. 
	* Seriously just remove any macros more complicated than a single line. BOOST_PP shit all needs to go.
	* It makes code impossible to read or edit, all for the "benefit" of saving me having to copy paste a few lines every time I add a new feature.
	* We can templateify the enumToType lookup
* Folderize source files in a sensible hierarchy instead of just using vstudio filters. This will make the github repo easier to read too. 
* Add a "Presenter" layer between OptionalCheats and SettingsStateAndEvents. Right now OC's are tightly coupled to setting events (ie toggling a functionality on a bool setting switching state via the valueChanged event). This is poor design and makes it impossible for anyone else to reuse my code.
	* Instead OC's should expose all their functionality in their header files. "Presenter" service should subscribe to the events, and will then call the appropiate functionality in the OC.
	* Will need to make a decision on where error and how error handling should happen. Probably at the OC level, code it as IF someone else were using the code (ie they would want to know that the ForceCheckpoint they did didn't work if it didn't work. So either throw or return std::expected<void, T>)
		* But also how do we properly handle "toggled" services having an error? Some sort of scoped mechanism? 
			* Services that constantly run (ie toggle services) should expose an error event that the presenter can subscribe to and decide what to do on error
	* services with internal state can impl a base class with getter/setters for state, templated by state object (perhaps a default state too)
	* optinalCheatStore should return std::expected<mccService, InitException> instead of throwing InitException. Let caller throw if they choose to. 
	* instead of all our optionalCheats storing weak_ptrs to dependent services, maybe they should just hold the optionalCheatStore and call that when they want something instead of lockOrThrow
		* this removes boilerplate, and prevents uninitialized weak_ptrs
	* Once we finish this we can think about bundling off a namespace that other MCC developers can easily use
		* Maybe rename OptionalCheatService to MCCService or something
* Ought to rename a bunch of misappropiately named classes for clarity.
* Make a decision on how to represent error states. std::expected<T, E> but what should E be that allows us to:
	* A) do "functional" things with it (composition)
	* B) not kill performance with string allocs
	* C) Provide specific error information but also general error information? Honestly there's a lot of different "consumers" of error info with different needs we need to design around:
		* User getting a message via the messagesGUI that their action or service didn't work
		* User getting some more specific info that *broadly* explains why it broke
		* Detailed information to the log files
		* Consuming services that need to make a decision (branch) on how to handle the error (this is where error codes are useful) - ie graceful continue with reduced functionality v.s. bail this iteration v.s. bail entire toggled service
	* Solutions could involve either polymorphism (virtual what() but for different levels of detail), OR
	* std::variant & visitor pattern. This seems complicated though..
* Several of my settings enums would be better represented as bitmasks eg SoftCeilingRenderTypes (Bipeds, Vehicles, BipedsOrVehicles)
* Should probably do a cleanup pass on SerialisableSetting
	* Settings GetValue should not be used to set value (unreference, make seperate set value method)
	* Cleanup how GetValue vs GetValueDisplay work and who can use them (clean up unarySettings vs binarySettings)
* Refactor how PlayerData/PlayerDatum works entirely to account for there being multiple different players
* Add a class that's basically just a wrapper for "GetPointerToX"
	* Or not idk
* Rework Multilevelpointers w/ std::expected
* Rework hook system so that hooks will not apply during loading screens
	* Thread safety-fy with scoped mutexes in game tick calls? That can get complicated but is probably "correct"
* TagNameTable to replace GetTagName.
* Figure out what's broken with advanceTicks/pause scopedService. Probably our pause-override-request system is dumb af.
* templated scenario tag struct etc etc
* resolveDependentCheat macro should take game as arg
* formatter pass
* compiler warning pass
* linter pass
* update unit tests to actually work
* Investigate alternate build environments - I'd like to get HCM building entirely in a github Runner eventually
	* This could lead to code-signing in the future which would stop all the Windows Defender issues people have
* I don't like the spamming of "if (game-this-implementation-is-for == not-currently-playing) return;"
	* Need to come up with a solution at the event/gui level so that only the appropiate game-impl gets called 
* A lot of boilerplate shitty "features" can be merged. Ie force cp/revert/mission restart could all be GameEngine stuff, instead of duplicating game engine stuff
	* We really need to put more faith that are optionalCheat construction won't fail cos yknow. For current patch it never will. For other patches we should start implementing more stuff anyway. And mcc is never gonna get updated again prolly
	* That doesn't mean we should switch to a hard-coded system though. I kinda like the abstraction of the dynamic system as long as you hide the impl details from the user


## Low priority stuff that I would like to implement if it weren't a lot of work and I weren't really lazy
* Random practice mode (Select multiple checkpoints and have them randomly injected - including cross level loading)
* HCM Synchronisation in theater & coop
