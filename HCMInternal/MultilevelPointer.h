#pragma once
#include "pch.h"






// base class
class MultilevelPointer {

private:

	// Common to all multilevel_pointers
	static std::stringstream mLastError;
	bool readString(std::string& resolvedOut) const; 
	bool readWString(std::wstring& resolvedOut) const;


protected:
	bool dereferencePointer(const void* const& base, const std::vector<int64_t>& offsets, uintptr_t* resolvedOut) const;
	static std::stringstream* SetLastErrorByRef()
	{
		mLastError.clear();
		mLastError.str("");
		return &mLastError;
	}


public:

	virtual ~MultilevelPointer() = default;


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
		if (typeid(T) == typeid(std::wstring))
		{
			return readWString(*(std::wstring*)resolvedOut);
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
		static_assert(typeid(T) != typeid(std::string) && "cannot use writeData for std::string");
		static_assert(typeid(T) != typeid(std::wstring) && "cannot use writeData for std::wstring");

		uintptr_t address;
		if (!this->resolve(&address)) return false;

		if (IsBadReadPtr((void*)address, sizeof(T)))
		{
			*SetLastErrorByRef() << std::format("Bad read address at 0x{:X}, size: 0x{:X}", address, sizeof(T)) << std::endl;
			return false;
		}

		if (protectedMemory)
		{
			patch_memory((void*)address, dataIn, sizeof(T));
		}
		else
		{
			memcpy((void*)address, dataIn, sizeof(T));
		}
		return true;
	}

	template<typename T>
	bool writeArrayData(T* dataIn, size_t arraySize, bool protectedMemory = false)
	{
		static_assert(typeid(T) != typeid(std::string) && "cannot use writeArrayData for std::string");
		static_assert(typeid(T) != typeid(std::wstring) && "cannot use writeArrayData for std::wstring");


		if (arraySize <= 0)
		{
			*SetLastErrorByRef() << std::format("Bad array size! Must be more than zero. Was: {}", arraySize) << std::endl;
			return false;
		}

		uintptr_t address;
		if (!this->resolve(&address)) return false;


		if (IsBadReadPtr((void*)address, arraySize))
		{
			*SetLastErrorByRef() << std::format("Bad read address at 0x{:X}, size: 0x{:X}", address, arraySize) << std::endl;
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

	bitOffsetT mBitOffset;
	const bitOffsetT getBitOffset() { return mBitOffset; }
	MultilevelPointer(bitOffsetT bitOffset) : mBitOffset(bitOffset) {}
};


namespace MultilevelPointerSpecialisation // So we don't pollute global namespace
{
	class ExeOffset : public MultilevelPointer {
	private:
		const std::vector<int64_t> mOffsets;
		static void* mEXEAddress;

	public:
		explicit ExeOffset(const std::vector<int64_t> offsets, bitOffsetT bitOffset = 0) : mOffsets(offsets), MultilevelPointer(bitOffset) {}
		bool MultilevelPointer::resolve(uintptr_t* resolvedOut) const override;
		const bitOffsetT getBitOffset() { return mBitOffset; }
	};

	class BaseOffset : public MultilevelPointer {
	private:
		const std::vector<int64_t> mOffsets;
		void* mBaseAddress;
	public:
		explicit BaseOffset(void* const& baseAddress, const std::vector<int64_t> offsets, bitOffsetT bitOffset = 0) : mBaseAddress(baseAddress), mOffsets(offsets), MultilevelPointer(bitOffset) {}
		bool MultilevelPointer::resolve(uintptr_t* resolvedOut) const override;
		void updateBaseAddress(void* const& baseAddress);
		void updateBaseAddress(uintptr_t const& baseAddress) { return updateBaseAddress((void*)baseAddress); }
		const bitOffsetT getBitOffset() { return mBitOffset; }
	};

	class ModuleOffset : public MultilevelPointer {
	private:
		const std::wstring mModuleName;
		const std::vector<int64_t> mOffsets;
	public:
		explicit ModuleOffset(const std::wstring_view& moduleName, const std::vector<int64_t> offsets, bitOffsetT bitOffset = 0) : mModuleName(moduleName), mOffsets(offsets), MultilevelPointer(bitOffset) {}
		bool MultilevelPointer::resolve(uintptr_t* resolvedOut) const override;
		std::wstring_view getModuleName() { return mModuleName; }
		const bitOffsetT getBitOffset() { return mBitOffset; }
	};

	class Resolved : public MultilevelPointer {
	private:
		const void* mBaseAddress;
	public:
		explicit Resolved(void* const& baseAddress, bitOffsetT bitOffset = 0) : mBaseAddress(baseAddress), MultilevelPointer(bitOffset) {}
		bool MultilevelPointer::resolve(uintptr_t* resolvedOut) const override;
		const bitOffsetT getBitOffset() { return mBitOffset; }
	};




}

