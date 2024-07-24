#pragma once
#include "pch.h"


// Goal: be cheap. Avoid string allocation/formatting on construction if you can (delay until call to what())
class HCMError
{
private:
	std::source_location mLoc;
public:
	HCMError(std::source_location loc) noexcept : mLoc(loc) {}

	virtual std::string what() const noexcept = 0;
	const std::string locStr() const noexcept { return std::format("@ {}::{}:{}", mLoc.file_name(), mLoc.function_name(), mLoc.line()).c_str(); }
	const std::source_location& loc() const noexcept { return mLoc; }
	virtual ~HCMError() = default;
};

using pHCMError = std::shared_ptr<HCMError>;




// Some common implementations. More can be defined elsewhere if they're too specific.

class BadWeakPtrError : public HCMError
{
private:
	const char* mBadWeakPtrName;
public:
	BadWeakPtrError(const char* badWeakPtrName, std::source_location loc = std::source_location::current()) 
		: HCMError(loc), mBadWeakPtrName(badWeakPtrName) 
	{}

	virtual std::string what() const noexcept override { return std::format("Bad weak ptr when locking {}", mBadWeakPtrName); };
};

class BadReadPtrError : public HCMError
{
private:
	const char* mBadReadPtrName;
	const uintptr_t mBadReadPtrAddress;
public:
	BadReadPtrError(const char* badReadPtrName, uintptr_t badReadPtrAddress, std::source_location loc = std::source_location::current())
		: HCMError(loc), mBadReadPtrName(badReadPtrName), mBadReadPtrAddress(badReadPtrAddress)
	{}

	BadReadPtrError(const char* badReadPtrName, void* badReadPtrAddress, std::source_location loc = std::source_location::current()) 
		: HCMError(loc), mBadReadPtrName(badReadPtrName), mBadReadPtrAddress((uintptr_t)badReadPtrAddress)
	{}

	virtual std::string what() const noexcept override { return std::format("Bad read of {} at {:X}", mBadReadPtrName, mBadReadPtrAddress); };
};

// temporary class to convert exceptions to errors. Once I replace throwing funcs (and funcs returning std::expected<T, HCMRuntimeException) with error type, I can get rid of this.
class RuntimeExceptionToError : public HCMError
{

	private:
		const HCMRuntimeException mEx;
	public:
		RuntimeExceptionToError(HCMRuntimeException ex, std::source_location loc = std::source_location::current())
			: HCMError(loc), mEx(ex)
		{}

		virtual std::string what() const noexcept override { return mEx.what(); };
};

class ErrorLiteralString : public HCMError
{
private:
	const char* mLiteralString;
public:
	ErrorLiteralString(const char* literalString, std::source_location loc = std::source_location::current())
		: HCMError(loc), mLiteralString(literalString)
	{}


	virtual std::string what() const noexcept override { return mLiteralString; };
};