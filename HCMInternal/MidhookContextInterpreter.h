#pragma once

// This is the order safetyhook stores registers in a SafetyHookContext
enum Register {
	rflags, r15, r14, r13, r12, r11, r10, r9, r8, rdi, rsi, rdx, rcx, rbx, rax, rbp, rsp
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
};