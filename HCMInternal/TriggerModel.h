#pragma once
#include "pch.h"


struct TriggerModel
{
	std::array<SimpleMath::Vector3, 8> vertices; 		// point order: http://people.eecs.berkeley.edu/~jfc/cs174lecs/lec11/Image127.gif
	TriggerModel(std::array<SimpleMath::Vector3, 8>& in) : vertices(in) {}
	TriggerModel() {}
	
	
};