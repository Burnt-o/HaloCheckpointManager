#pragma once
class IAnchorPoint {
public:
	virtual SimpleMath::Vector2 getAnchorPoint() = 0;
	virtual ~IAnchorPoint() = default;
};