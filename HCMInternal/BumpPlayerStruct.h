#pragma once
#include "pch.h"
#include "BumpControlDefs.h"
#include "BumpInput.h"


// could use a dynamic struct for multi-game/version support,
// but a hard-coded struct is much more performant and I'm only ever planning to implement this for H1-currentpatch
struct BumpPlayerObject
{
private:
	char pad0[0x18];
public:
	SimpleMath::Vector3 position;
	SimpleMath::Vector3 velocity;
private:
	char pad1[0x1A8];
	byte crouchJumpFlag; // crouch in bit 0, jump in bit 1
	char pad2[0x2B];
	SimpleMath::Vector2 facingDirection;
	char pad3[0x4C];
	SimpleMath::Vector2 movementInput; // x is forwardback, y is left-right. -1 to +1.
	char pad4[0x2B8];
public:
	float crouchPercent;
private:
	char pad5[0x8C8];
public:


	void setInput(InpMask input);
	void setViewAngle(float viewAngle);
};


static_assert(sizeof(BumpPlayerObject) == 0xDE4);
