#pragma once
#include "MultilevelPointer.h"


// A small wrapper for safetyhooks InlineHook and MidHook, for hooks to dynamically loaded/unloaded modules
// For use with ModuleHookManager
// Hooks attach automatically on module load, if mWantsToBeAttached. 
class ModuleHookBase {
private:
	std::wstring mAssociatedModule;
	bool mWantsToBeAttached = false; // This flag determines if the hook will try to *attach* or *detach* when its module is loaded in
	void updateHookState(); // just called by setWantsToBeAttached, tries to force attach if value is true

protected:
	ModuleHookBase(const std::wstring associatedModule, bool startEnabled)
		: mAssociatedModule(associatedModule), mWantsToBeAttached(startEnabled)
	{
	}

	virtual ~ModuleHookBase() { } // child classes will override to detach their hook objects



public:
	// Contracts - must be overridden
	virtual void attach() = 0;
	virtual void detach() = 0;

	virtual bool isHookInstalled() const = 0;

	// Common functions
	const std::wstring& getAssociatedModule() const;

	// get/set for mWantsToBeAttached
	bool getWantsToBeAttached() const;
	void setWantsToBeAttached(bool value);




};

class ModuleInlineHook : public ModuleHookBase {
private:
	std::shared_ptr<MultilevelPointer> mOriginalFunction;
	void* mHookFunction;
	safetyhook::InlineHook mInlineHook;

	ModuleInlineHook(const std::wstring associatedModule, std::shared_ptr<MultilevelPointer> original_func, void* new_func, bool startEnabled = false)
		: ModuleHookBase(associatedModule, startEnabled), mOriginalFunction(original_func), mHookFunction(new_func)
	{
		if (startEnabled)
		{
			attach();
		}
	}

public:

	static std::unique_ptr<ModuleInlineHook> make(const std::wstring associatedModule, std::shared_ptr<MultilevelPointer> original_func, void* new_func, bool startEnabled = false);

	~ModuleInlineHook() final;
	ModuleInlineHook(const ModuleInlineHook&) = delete;
	ModuleInlineHook& operator=(const ModuleInlineHook&) = delete;
	void attach() final;
	void detach() final;



	safetyhook::InlineHook& getInlineHook(); // Useful for calling original function

	bool isHookInstalled() const final;


};




class ModuleMidHook : public ModuleHookBase {

private:
	std::shared_ptr<MultilevelPointer> mOriginalFunction;
	safetyhook::MidHookFn mHookFunction;
	safetyhook::MidHook mMidHook;

	ModuleMidHook(const std::wstring associatedModule, std::shared_ptr<MultilevelPointer> original_func, safetyhook::MidHookFn new_func, bool startEnabled = false)
		: ModuleHookBase(associatedModule, startEnabled), mOriginalFunction(original_func), mHookFunction(new_func)
	{
		if (startEnabled)
		{
			attach();
		}
	}

public:

	static std::unique_ptr<ModuleMidHook> make(const std::wstring associatedModule, std::shared_ptr<MultilevelPointer> original_func, safetyhook::MidHookFn new_func, bool startEnabled = false);

	~ModuleMidHook() final;
	ModuleMidHook(const ModuleMidHook&) = delete;
	ModuleMidHook& operator=(const ModuleMidHook&) = delete;

	void attach() final;
	void detach() final;

	bool isHookInstalled() const final;
};


// IAT = InternalAddressTable. This hook rewrites the IAT to point to our new_func instead of old_func
class ModuleIATHook : public ModuleHookBase
{
	// I don't have any plans to use this at the moment, it's mostly functionally equivalent to ModuleInlineHook
	// except that it can pre-empt other software that hook the same functions. 
	// But I don't know of any module-functions that I plan to IAT hook, only global ones.. well d3d is a module. hm. I was just treating it as global.
	// Maybe that would explain some of the crashes people would experience. 
	// If d3d unloads then how would I tie hook destruction to d3d resource cleanup?
		// Need a callback. Should add callback to the other hook types too

	// TODO: whether doing it globally or module-ally, we will need a IAThook class. could just extend safetyhook?
};


class ModulePatch : public ModuleHookBase {
private:
	std::shared_ptr<MultilevelPointer> mOriginalFunction;
	std::vector<byte> mOriginalBytes;
	std::vector<byte> mPatchedBytes;

	ModulePatch(const std::wstring associatedModule, std::shared_ptr<MultilevelPointer> original_func, std::vector<byte> patchedBytes, bool startEnabled = false)
		: ModuleHookBase(associatedModule, startEnabled), mOriginalFunction(original_func), mPatchedBytes(patchedBytes)
	{
		if (startEnabled)
		{
			attach();
		}
	}


public:
	static std::unique_ptr<ModulePatch> make(const std::wstring associatedModule, std::shared_ptr<MultilevelPointer> original_func, std::vector<byte> patchedBytes, bool startEnabled = false);
	static std::unique_ptr<ModulePatch> makeNOPCall(const std::wstring associatedModule, std::shared_ptr<MultilevelPointer> original_func, bool startEnabled = false);

	~ModulePatch() final;
	ModulePatch(const ModulePatch&) = delete;
	ModulePatch& operator=(const ModulePatch&) = delete;

	void attach() final;
	void detach() final;

	bool isHookInstalled() const final;
};