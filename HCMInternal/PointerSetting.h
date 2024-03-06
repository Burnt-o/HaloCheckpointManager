#pragma once
#include "GameState.h"
#include "PointerManager.h"

// TODO: hide inclusion of PointerManager (need factory pattern)
// Though to be honest I'm surprised I managed to get the templated virtual pimpl stuff to work so maybe I shouldn't push it
// (note to self; there's a big difference between virtual templates and templated virtuals)


/*	
PointerSetting<valueType>
	* Instead of the value being in our direct control like Unary/BinarySetting, it reads off a multilevel pointer of game memory.
	* Constructor creates an internal impl of itself for each required gamestate.
	* Constructor failure sets impl std::optional<MultilevelPointer> to std::nullopt, and sets an impl "last error" string(private, with const get accessor).
		* Constructor DOES NOT THROW!
	* When GUIElementConstructor constructs a GUIElement using a PointerSetting, the GUIElement constructor itself checks if the respective impl successfully constructed. 
		If not, GUIElement constructor throws HCMInitException, prepending the getLastError string.
	* Provides get and set accessors for the data, and more importantly a getRef that returns a direct raw pointer.
	* Accessors return std::nullopt on MultilevelPointer resolve failure (check getLastError for more info)
	* No serialisation since that would be stupid. (TODO: maybe add anyway for copy/paste clipboard funcs)
*/
template <typename valueType>
class PointerSetting
{
public:
	// unfortunately has to be public for stupid linkage reasons
	class IPointerSettingImpl
	{
	public:
		virtual bool successfullyConstructed() = 0;
		virtual const std::optional<valueType> getCopy() = 0;
		virtual std::optional<valueType*> getRef() = 0;
		virtual bool set(valueType value) = 0;
		virtual std::string_view getLastError() = 0;
	};


private:
	std::string optionName;

	std::map<GameState, std::shared_ptr<IPointerSettingImpl>> pimplMap;
	
public:
	PointerSetting(std::string pointerDataName, std::shared_ptr<PointerManager> pointerManager, std::vector<GameState> implsToConstruct) noexcept;
	~PointerSetting();

	bool successfullyConstructed(GameState gameImpl);
	const std::optional<valueType> getCopy(GameState gameImpl); // returns copy
	std::optional<valueType*> getRef(GameState gameImpl); // returns mutable pointer
	bool set(GameState gameImpl, valueType value); // returns whether successfully set
    std::string_view getLastError(GameState gameImpl); // if constructor fails, set returns false, or getRef/getCopy return nullopt, getLastError will explain why
	std::string_view getOptionName();
};