#include "pch.h"
#include "ModuleHook.h"
//#include "pointer.h"
#include "ModuleCache.h"
#include "ModuleHookManager.h"

std::unique_ptr<ModuleInlineHook> ModuleInlineHook::make(const std::wstring associatedModule, std::shared_ptr<MultilevelPointer> original_func, void* new_func, bool startEnabled)
{
	auto ptr = std::unique_ptr< ModuleInlineHook>(new ModuleInlineHook(associatedModule, original_func, new_func, startEnabled));
	ModuleHookManager::addHook(associatedModule, ptr.get());
	return ptr;

}

std::unique_ptr<ModuleMidHook> ModuleMidHook::make(const std::wstring associatedModule, std::shared_ptr<MultilevelPointer> original_func, safetyhook::MidHookFn new_func, bool startEnabled)
{
	auto ptr = std::unique_ptr< ModuleMidHook>(new ModuleMidHook(associatedModule, original_func, new_func, startEnabled));
	ModuleHookManager::addHook(associatedModule, ptr.get());
	return ptr;
}

std::unique_ptr<ModulePatch> ModulePatch::make(const std::wstring associatedModule, std::shared_ptr<MultilevelPointer> original_func, std::vector<byte> patchedBytes, bool startEnabled)
{
	auto ptr = std::unique_ptr< ModulePatch>(new ModulePatch(associatedModule, original_func, patchedBytes, startEnabled));
	ModuleHookManager::addHook(associatedModule, ptr.get());
	return ptr;
}

const std::wstring& ModuleHookBase::getAssociatedModule() const
{
	return this->mAssociatedModule;
}

ModuleInlineHook::~ModuleInlineHook()
{
	detach();
	ModuleHookManager::removeHook(getAssociatedModule(), this);

}

ModuleMidHook::~ModuleMidHook()
{
	detach();
	ModuleHookManager::removeHook(getAssociatedModule(), this);
}

ModulePatch::~ModulePatch()
{
	detach();
	ModuleHookManager::removeHook(getAssociatedModule(), this);
}



void ModuleInlineHook::attach()
{
	PLOG_DEBUG << "inline_hook attempting attach: " << getAssociatedModule();
	PLOG_VERBOSE << "hookFunc " << this->mHookFunction;
	if (this->isHookInstalled()) {
		PLOG_DEBUG << "attach failed: hook already installed";
		return;
	}

	if (this->mOriginalFunction == nullptr)
	{
		PLOG_DEBUG << "attach failed: no pointer to original function";
		return;
	}

	uintptr_t pOriginalFunction;
	if (!this->mOriginalFunction->resolve(&pOriginalFunction))
	{
		PLOG_ERROR << "attach failed: pOriginalFunction pointer failed to resolve";
		return;
	}

	PLOG_VERBOSE << "pOriginalFunction " << pOriginalFunction;

	this->mInlineHook = safetyhook::create_inline((void*)pOriginalFunction, this->mHookFunction);

	PLOG_DEBUG << "inline_hook successfully attached: " << this->getAssociatedModule();

}

void ModuleMidHook::attach()
{
	PLOG_DEBUG << "mid_hook attempting attach: " << this->getAssociatedModule();
	if (this->isHookInstalled()) {
		PLOG_DEBUG << "attach failed: hook already installed";
		return;
	}

	if (this->mOriginalFunction == nullptr)
	{
		PLOG_DEBUG << "attach failed: no pointer to original function";
		return;
	}

	uintptr_t pOriginalFunction;
	if (!this->mOriginalFunction->resolve(&pOriginalFunction))
	{
		PLOG_ERROR << "attach failed: pOriginalFunction pointer failed to resolve";
		return;
	}

	PLOG_VERBOSE << "pOriginalFunction " << pOriginalFunction;
	// per https://github.com/cursey/safetyhook/commit/77243791d72bfe49b94349922710c443db1df0fa
	// mid hooks don't freeze the threads. 
	// I don't know why they made this change but it can obv cause crashes with hooks that run frequently
	auto freeze = safetyhook::ThreadFreezer();
	this->mMidHook = safetyhook::create_mid((void*)pOriginalFunction, this->mHookFunction);
	freeze.~ThreadFreezer();
	PLOG_DEBUG << "mid_hook successfully attached: " << this->getAssociatedModule();
}

void ModuleInlineHook::detach()
{
	PLOG_DEBUG << "detaching hook: " << this->getAssociatedModule();
	if (!this->isHookInstalled()) {
		PLOG_DEBUG << "already detached";
		return;
	}

	this->mInlineHook = {};
	PLOG_DEBUG << "successfully detached " << this->getAssociatedModule();
}

void ModuleMidHook::detach()
{
	PLOG_DEBUG << "detaching hook: " << this->getAssociatedModule();
	if (!this->isHookInstalled()) {
		PLOG_DEBUG << "already detached";
		return;
	}

	this->mMidHook = {};
	PLOG_DEBUG << "successfully detached " << this->getAssociatedModule();
}



void ModulePatch::attach()
{
	PLOG_VERBOSE << " ModulePatch::attach()";
#define logErrorReturn(x, y) if (x) { PLOG_ERROR << y; return; }
	if (mOriginalBytes.empty())
	{
		mOriginalBytes.resize(mPatchedBytes.size());
		logErrorReturn(mOriginalFunction->readArrayData(mOriginalBytes.data(), mPatchedBytes.size()) == false, "Could not resolve original function");
	}

	std::vector<byte> currentBytes;
	currentBytes.resize(mPatchedBytes.size());
	logErrorReturn(mOriginalFunction->readArrayData(currentBytes.data(), mPatchedBytes.size()) == false, "Could not resolve original function");

	logErrorReturn(currentBytes != mOriginalBytes, "Current bytes did not match original bytes");

	uintptr_t addy;
	mOriginalFunction->resolve(&addy);
	PLOG_DEBUG << "mOriginalFunction loc: " << std::hex << (uint64_t)addy;
		PLOG_DEBUG << "mPatchedBytes size: " << mPatchedBytes.size();

	logErrorReturn(mOriginalFunction->writeArrayData(mPatchedBytes.data(), mPatchedBytes.size(), true) == false, "Failed to patch new bytes");
}

void ModulePatch::detach()
{

	PLOG_VERBOSE << " ModulePatch::detach()";
#define logErrorReturn(x, y) if (x) { PLOG_ERROR << y; return; }
	logErrorReturn(mOriginalBytes.empty(), "No original bytes saved to restore");

	PLOG_VERBOSE << "mOriginalBytes size: " << mOriginalBytes.size();

	std::vector<byte> currentBytes;
	currentBytes.resize(mPatchedBytes.size());
	logErrorReturn(mOriginalFunction->readArrayData(currentBytes.data(), mPatchedBytes.size()) == false, "Failed to read current bytes");

	logErrorReturn(currentBytes != mPatchedBytes, "Current bytes did not match patched bytes")

	logErrorReturn(mOriginalFunction->writeArrayData(mOriginalBytes.data(), mOriginalBytes.size(), true) == false, "Failed to restore original bytes");

}





void ModuleHookBase::updateHookState()
{
	if (this->mWantsToBeAttached)
	{
		this->attach();
	}
	else
	{
		this->detach();
	}
}


// Getter/setter for mWantsToBeAttached
void ModuleHookBase::setWantsToBeAttached(bool value)
{
	if (this->mWantsToBeAttached != value)
	{
		this->mWantsToBeAttached = value;
		this->updateHookState();
	}
}


bool ModuleHookBase::getWantsToBeAttached() const
{
	return this->mWantsToBeAttached;
}

// Gets a ref to the safetyhook object, mainly used for calling the original function from within the new function
safetyhook::InlineHook& ModuleInlineHook::getInlineHook() 
{
	return this->mInlineHook;
}



bool ModuleInlineHook::isHookInstalled() const
{
	return this->mInlineHook.operator bool();
}

bool ModuleMidHook::isHookInstalled() const
{
	return this->mMidHook.operator bool();
}

bool ModulePatch::isHookInstalled() const
{
	std::vector<byte> currentBytes;
	if (!mOriginalFunction->readArrayData(&currentBytes, mOriginalBytes.size())) return false;
	return currentBytes == mPatchedBytes;
}