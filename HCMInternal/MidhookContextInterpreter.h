#pragma once
#include "safetyhook.hpp"

enum Register {
	// This is the order safetyhook stores registers in a SafetyHookContext. Important that the index matches how safetyhook stores it.
	xmm0a, xmm0b, xmm1a, xmm1b, xmm2a, xmm2b, xmm3a, xmm3b, xmm4a, xmm4b, xmm5a, xmm5b,
	xmm6a, xmm6b, xmm7a, xmm7b, xmm8a, xmm8b, xmm9a, xmm9b, xmm10a, xmm10b,
	xmm11a, xmm11b, xmm12a, xmm12b, xmm13a, xmm13b, xmm14a, xmm14b, xmm15a, xmm15b,
	rflags, r15, r14, r13, r12, r11, r10, r9, r8, rdi, rsi, rdx, rcx, rbx, rax, rbp, rsp, trampoline_rsp, rip,



};



class ParameterLocation
{
private:
	Register mRegIndex;
	std::vector<int> mOffsets;
public:

	ParameterLocation(Register reg, std::vector<int> offsets = {}) : mRegIndex(reg), mOffsets(offsets) {}

	Register getRegIndex() { return mRegIndex;  }
	std::vector<int>& getOffsets() { return mOffsets; }
};

// One of these interpreter objects will be associated with each MidHook callback function
// This allows us to figure out at runtime which registers contains the parameters that the callback wants to manipulate
// eg depending what version of MCC we've been injected into
class MidhookContextInterpreter
{
private:
	std::vector<ParameterLocation> mParameterRegisterIndices; //ie index 0 contains the RegisterIndex of the FIRST parameter that the midhook callback function cares about
	std::vector<safetyhook::Context64> mpri;
public:
	explicit MidhookContextInterpreter(std::vector<ParameterLocation> parameterRegisterIndices) : mParameterRegisterIndices(parameterRegisterIndices) {}

	// return ref to the ctx register we want
	uintptr_t* getParameterRef(SafetyHookContext& ctx, int parameterIndex);

};




// Used by pointer manager to convert strings to register enum
const std::map<std::string, Register> stringToRegister = {
	{"rflags", Register::rflags },
	{"r15", Register::r15 },
	{"r14", Register::r14 },
	{"r13", Register::r13 },
	{"r12", Register::r12 },
	{"r11", Register::r11 },
	{"r10", Register::r10 },
	{"r9", Register::r9 },
	{"r8", Register::r8 },
	{"rdi", Register::rdi },
	{"rsi", Register::rsi },
	{"rdx", Register::rdx },
	{"rcx", Register::rcx },
	{"rbx", Register::rbx },
	{"rax", Register::rax },
	{"rbp", Register::rbp },
	{"rsp", Register::rsp },

	{"xmm0a", Register::xmm0a },
	{"xmm0b", Register::xmm0b },
	{"xmm1a", Register::xmm1a },
	{"xmm1b", Register::xmm1b },
	{"xmm2a", Register::xmm2a },
	{"xmm2b", Register::xmm2b },
	{"xmm3a", Register::xmm3a },
	{"xmm3b", Register::xmm3b },
	{"xmm4a", Register::xmm4a },
	{"xmm4b", Register::xmm4b },
	{"xmm5a", Register::xmm5a },
	{"xmm5b", Register::xmm5b },
	{"xmm6a", Register::xmm6a },
	{"xmm6b", Register::xmm6b },
	{"xmm7a", Register::xmm7a },
	{"xmm7b", Register::xmm7b },
	{"xmm8a", Register::xmm8a },
	{"xmm8b", Register::xmm8b },
	{"xmm9a", Register::xmm9a },
	{"xmm9b", Register::xmm9b },
	{"xmm10a", Register::xmm10a },
	{"xmm10b", Register::xmm10b },
	{"xmm11a", Register::xmm11a },
	{"xmm11b", Register::xmm11b },
	{"xmm12a", Register::xmm12a },
	{"xmm12b", Register::xmm12b },
	{"xmm13a", Register::xmm13a },
	{"xmm13b", Register::xmm13b },
	{"xmm14a", Register::xmm14a },
	{"xmm14b", Register::xmm14b },
	{"xmm15a", Register::xmm15a },
	{"xmm15b", Register::xmm15b },
};