#include "pch.h"
#include "BumpPlayerStruct.h"
#include <numeric>




constexpr float diagonalMag = 0.7071067691;

std::unordered_map<int, SimpleMath::Vector2> dirBitsToVec2Map =
{
	// 0 bits set
	{InpMask(Input::None).value, {0.f, 0.f} },

	// 1 bit set
	{InpMask(Input::Forward).value, {1.f, 0.f} },
	{InpMask(Input::Backward).value, {-1.f, 0.f} },
	{InpMask(Input::Left).value, {0.f, 1.f} },
	{InpMask(Input::Right).value, {0.f, -1.f} },

	// 2 bits set (diagonal movement)
	{(InpMask(Input::Forward) | InpMask(Input::Left)).value, {diagonalMag, diagonalMag} },
	{(InpMask(Input::Forward) | InpMask(Input::Right)).value, {diagonalMag, -diagonalMag} },
	{(InpMask(Input::Backward) | InpMask(Input::Left)).value, {-diagonalMag, diagonalMag} },
	{(InpMask(Input::Backward) | InpMask(Input::Right)).value, {-diagonalMag, -diagonalMag} },

	// cancelled movement
	{(InpMask(Input::Forward) | InpMask(Input::Backward)).value, {0.f, 0.f} },
	{(InpMask(Input::Left) | InpMask(Input::Right)).value, {0.f, 0.f} },

	// 3 bits set (opposite movements cancel out)
	{(InpMask(Input::Forward) | InpMask(Input::Left) | InpMask(Input::Right)).value, {1.f, 0.f} },
	{(InpMask(Input::Backward) | InpMask(Input::Left) | InpMask(Input::Right)).value, {-1.f, 0.f} },
	{(InpMask(Input::Left) | InpMask(Input::Forward) | InpMask(Input::Backward)).value, {0.f, 1.f} },
	{(InpMask(Input::Right) | InpMask(Input::Forward) | InpMask(Input::Backward)).value, {0.f, -1.f} },

	// 4 bits set
		{(InpMask(Input::Right) | InpMask(Input::Forward) | InpMask(Input::Backward) | InpMask(Input::Left)).value, {0.f, 0.f} },
};

constexpr std::array<SimpleMath::Vector2, 16> dirBitsToVec2Vec =
{
	// in numeric ascending order of bitmask value

	SimpleMath::Vector2{0.f, 0.f}, // no input

	SimpleMath::Vector2{1.f, 0.f}, // Forward

	SimpleMath::Vector2{-1.f, 0.f}, // Backward

	SimpleMath::Vector2{0.f, 0.f}, // Forward and backward


	SimpleMath::Vector2{0.f, 1.f}, // Left

	SimpleMath::Vector2{diagonalMag, diagonalMag}, // Left and forward

	SimpleMath::Vector2{-diagonalMag, diagonalMag}, // Left and backward

	SimpleMath::Vector2{0.f, 1.f}, // Left and forward and backward


	SimpleMath::Vector2{0.f, -1.f}, // Right

	SimpleMath::Vector2{diagonalMag, -diagonalMag}, // Right and forward

	SimpleMath::Vector2{-diagonalMag, -diagonalMag}, // Right and backward

	SimpleMath::Vector2{0.f, -1.f}, // Right and forward and backward


	SimpleMath::Vector2{0.f, 0.f}, // Right and left

	SimpleMath::Vector2{1.f, 0.f}, // Right and left and forward

	SimpleMath::Vector2{-1.f, 0.f}, // Right and left and backward

	SimpleMath::Vector2{0.f, 0.f}, // Right and forward and backward and left

};



static bool crouchInputLastTick = false;

void BumpPlayerObject::setInput(InpMask input)
{

//#define DEBUG_INPUT_ARRAY

#ifdef DEBUG_INPUT_ARRAY

		for (auto& [key, vec2] : dirBitsToVec2Map)
		{
			if (dirBitsToVec2Vec.at(key) != vec2)
			{
				PLOG_ERROR << "mismatch between dirBitsToVec2Vec and dirBitsToVec2Map for input mask value: " << key;
				assert(false && "mismatch between dirBitsToVec2Vec and dirBitsToVec2Map");
			}
		}

#endif



	auto moveinputmask = input & ( InpMask(Input::Forward) | InpMask(Input::Backward) | InpMask(Input::Left) | InpMask(Input::Right));
	movementInput = dirBitsToVec2Vec.at(moveinputmask.value);

	// CROUCH
	// So there's a weird input mechanic where crouch inputs always last one extra tick more than you hold them
	// we need to replicate that here
	bool crouchInputThisTick = contains(input, Input::Crouch);

	crouchJumpFlag = crouchInputThisTick || (crouchInputThisTick == false && crouchInputLastTick == true)
		? crouchJumpFlag | (1 << 0)
		: crouchJumpFlag & ~(1 << 0);

	crouchInputLastTick = crouchInputThisTick;


	// JUMP
	crouchJumpFlag = contains(input, Input::Jump)
		? crouchJumpFlag | (1 << 1)
		: crouchJumpFlag & ~(1 << 1);


	LOG_ONCE_CAPTURE(PLOG_DEBUG << "playerObjectcrouchJumpFlag: " << std::hex << p, p = &crouchJumpFlag);
	LOG_ONCE_CAPTURE(PLOG_DEBUG << "playerObjectMovementInput: " << std::hex << p, p = &movementInput);


}

void BumpPlayerObject::setViewAngle(float viewAngle)
{
	LOG_ONCE_CAPTURE(PLOG_DEBUG << "playerFacingDirection: " << std::hex << p, p = &facingDirection);

	facingDirection = SimpleMath::Vector2(std::cosf(viewAngle), std::sinf(viewAngle));

	LOG_ONCE_CAPTURE(PLOG_DEBUG << "given view angle: " << va, va = viewAngle);
	LOG_ONCE_CAPTURE(PLOG_DEBUG << "facex: " << x, x = std::cosf(viewAngle));
	LOG_ONCE_CAPTURE(PLOG_DEBUG << "facey: " << y, y = std::sinf(viewAngle));

//#define DEBUG_FACING

#ifdef DEBUG_FACING
	float test = 3.053390741f;
	SimpleMath::Vector2 expectedFacing = { -0.9961127043f, 0.088087596f };
	SimpleMath::Vector2 calculatedFacing = { std::cosf(test), std::sinf(test) };
	float dist = SimpleMath::Vector2::Distance(expectedFacing, calculatedFacing);

	if (dist != 0.f)
	{
		PLOG_ERROR << "calculated facing dir did not match expected";
		PLOG_ERROR << "distance: " << dist;
		PLOG_ERROR << "expected: " << expectedFacing;
		PLOG_ERROR << "calculated: " << calculatedFacing;
	}
#endif

}