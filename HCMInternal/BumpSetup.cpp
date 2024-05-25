#include "pch.h"
#include "BumpSetup.h"

std::ostream& operator<<(std::ostream& out, const BumpSetup& setup)
{
	out << "Complexity level: " << setup.complexityLevel() << std::endl;

	out << "Go Idle Input: " << (setup.goIdleInput == InpMask(Input::Crouch) ? "Crouch" : "Stand") << std::endl;

	out << "Setup Inputs: " << std::endl;
	for (auto& inputAndLengthAndBool : setup.setupInputs)
	{
		auto& inputAndLength = inputAndLengthAndBool.first;
		out << "Setup Idle Input: " << (inputAndLengthAndBool.second ? "Crouch" : "Stand") << std::endl;
		out << "Press {" << InpMaskToString(inputAndLength.first) << "} for " << inputAndLength.second << " ticks." << std::endl;
	}

#ifdef SHIELD_BUMPS
	out << "Shield Inputs: " << std::endl;
	out << "ticksAfterGoForShieldOn: " << setup.shieldOnTickLastResult << std::endl;
#endif

	out << "Go Inputs: " << std::endl;
	for (auto& inputAndLength : setup.GetCollapsedGoInputVector())
	{
		out << "Press {" << InpMaskToString(inputAndLength.first) << "} for " << inputAndLength.second << " ticks." << std::endl;
	}
	return out;
}



template <>
struct std::hash<GoInputVector>
{
	std::size_t operator()(GoInputVector const& vec) const {
		std::size_t seed = vec.size();
		for (auto& i : vec) {
			seed ^= i.value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}
		return seed;
	}
};

template <>
struct std::hash<SetupInputVector>
{
	std::size_t operator()(SetupInputVector const& vec) const {
		std::size_t seed = vec.size();
		for (auto& i : vec) {
			seed ^= i.first.first.value + i.first.second + i.second + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}
		return seed;
	}
};

template <>
 struct std::hash<BumpSetup>
 {
	 std::size_t operator()(const BumpSetup& k) const
	{
		using std::size_t;
		using std::hash;

		return
			hash<SetupInputVector>()(k.setupInputs)
			^ (hash<GoInputVector>()(k.goInputs) << 1)
			^ (k.goIdleInput.value << 2)
#ifdef SHIELD_BUMPS
			^ (k.shieldOnTickLastResult << 3)
#endif
			;
	}
};





std::size_t BumpSetupHasher::operator()(const BumpSetup& k) const
{
	return std::hash<BumpSetup>()(k);
}
