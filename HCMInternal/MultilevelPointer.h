#pragma once
#include "pch.h"







// base factory class
class MultilevelPointer {
private:

	// Common to all multilevel_pointers
	static std::stringstream mLastError;
	bool readString(std::string& resolvedOut) const; 


protected:
	bool dereferencePointer(const void* const& base, const std::vector<int64_t>& offsets, uintptr_t* resolvedOut) const;
	static std::stringstream* SetLastErrorByRef()
	{
		mLastError.clear();
		mLastError.str("");
		return &mLastError;
	}
	virtual ~MultilevelPointer() = default;
public:


	// Factory methods
	// Pointer is relative to the exe address
	static std::shared_ptr<MultilevelPointer> make(const std::vector<int64_t> offsets);
	//static MultilevelPointer* make(const std::vector<int64_t>& offsets);

	// Pointer is relative to some void* baseAddress
	static std::shared_ptr<MultilevelPointer> make(void* const baseAddress, const std::vector<int64_t> offsets);

	// Pointer is relative to a module eg halo1.dll
	static std::shared_ptr<MultilevelPointer> make(const std::wstring_view moduleName, const std::vector<int64_t> offsets);

	// Pointer is already fully resolved (used for stuff that never changes address)
	static std::shared_ptr<MultilevelPointer> make(void* const baseAddress);

	// The useful stuff
	virtual bool resolve(uintptr_t* resolvedOut) const = 0; // Overriden in derived classes

	bool readMCCString(std::string& resolvedOut) const; // special case of readData that handles short-string-optimization

	template<typename T>
	bool readData(T* resolvedOut) const
	{
		if (typeid(T) == typeid(std::string))
		{
			return readString(*(std::string*)resolvedOut);
		}

		uintptr_t address;
		if (!this->resolve(&address)) return false;

		if (IsBadReadPtr((void*)address, sizeof(T)))
		{
			*SetLastErrorByRef() << std::format("Bad read address at 0x{:X}, size: 0x{:X}", address, sizeof(T)) << std::endl;
			return false;
		}

		*resolvedOut = *(T*)address;
		return true;
	}

	template<typename T>
	bool readArrayData(T* resolvedOut, size_t arraySize) const
	{
		if (typeid(T) == typeid(std::string))
		{
			return readString(*(std::string*)resolvedOut);
		}

		uintptr_t address;
		if (!this->resolve(&address)) return false;

		if (IsBadReadPtr((void*)address, arraySize))
		{
			*SetLastErrorByRef() << std::format("Bad read address at 0x{:X}, size: 0x{:X}", address, arraySize) << std::endl;
			return false;
		}

		memcpy(resolvedOut, (void*)address, arraySize);
		return true;
	}

	static std::string GetLastError()
	{
		if (mLastError)
			return mLastError.str();
		else
			return "error not set";
	}

	template<typename T>
	bool writeData(T* dataIn, bool protectedMemory = false)
	{
		if (typeid(T) == typeid(std::string))
		{
			return false;
		}

		uintptr_t address;
		if (!this->resolve(&address)) return false;

		if (IsBadWritePtr((void*)address, sizeof(T)))
		{
			*SetLastErrorByRef() << std::format("Bad write address at 0x{:X}, size: 0x{:X}", address, sizeof(T)) << std::endl;
			return false;
		}

		if (protectedMemory)
		{
			patch_memory((void*)address, dataIn, sizeof(T));
		}
		else
		{
			patch_memory((void*)address, dataIn, sizeof(T));
			memcpy((void*)address, dataIn, sizeof(T));
		}
		return true;
	}

	template<typename T>
	bool writeArrayData(T* dataIn, size_t arraySize, bool protectedMemory = false)
	{
		if (typeid(T) == typeid(std::string))
		{
			return false;
		}

		if (arraySize <= 0) return false;

		uintptr_t address;
		if (!this->resolve(&address)) return false;

		if (IsBadWritePtr((void*)address, arraySize))
		{
			*SetLastErrorByRef() << std::format("Bad write address at 0x{:X}, size: 0x{:X}", address, arraySize) << std::endl;
			return false;
		}

		if (protectedMemory)
		{
			patch_memory((void*)address, dataIn, arraySize);
		}
		else
		{
			memcpy((void*)address, dataIn, arraySize);
		}

		return true;

	}

};


namespace PointerTypes // So we don't pollute global namespace
{
	class ExeOffset : public MultilevelPointer {
	private:
		const std::vector<int64_t> mOffsets;
		static void* mEXEAddress;
	public:
		explicit ExeOffset(const std::vector<int64_t> offsets) : mOffsets(offsets) {}
		bool MultilevelPointer::resolve(uintptr_t* resolvedOut) const override;
	};

	class BaseOffset : public MultilevelPointer {
	private:
		void* mBaseAddress;
		const std::vector<int64_t> mOffsets;
	public:
		explicit BaseOffset(void* const& baseAddress, const std::vector<int64_t> offsets) : mBaseAddress(baseAddress), mOffsets(offsets) {}
		bool MultilevelPointer::resolve(uintptr_t* resolvedOut) const override;
		void updateBaseAddress(void* const& baseAddress);
	};

	class ModuleOffset : public MultilevelPointer {
	private:
		const std::wstring mModuleName;
		const std::vector<int64_t> mOffsets;
	public:
		explicit ModuleOffset(const std::wstring_view& moduleName, const std::vector<int64_t> offsets) : mModuleName(moduleName), mOffsets(offsets) {}
		bool MultilevelPointer::resolve(uintptr_t* resolvedOut) const override;
	};

	class Resolved : public MultilevelPointer {
	private:
		const void* mBaseAddress;
	public:
		explicit Resolved(void* const& baseAddress) : mBaseAddress(baseAddress) {}
		bool MultilevelPointer::resolve(uintptr_t* resolvedOut) const override;
	};



}

