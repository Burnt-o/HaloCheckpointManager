#pragma once
class IAnchorPoint {
public:
	virtual Vec2 getAnchorPoint() = 0;
	virtual ~IAnchorPoint() = default;
};