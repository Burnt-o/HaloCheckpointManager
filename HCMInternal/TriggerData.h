#pragma once
#include "pch.h"
#include "TriggerModel.h"
#include <bitset>
struct TriggerData 
{
	std::string name;
	uint32_t tickLastChecked = 0;
	TriggerModel model;

	TriggerData(std::string triggerName, TriggerModel triggerModel) : name(triggerName), model(triggerModel) {}

#ifdef HCM_DEBUG
	TriggerData(SimpleMath::Vector3 origin, float size)
	{
		name = "debug_trigger";
		std::array<SimpleMath::Vector3, 8> vertices;

		// point order: http://people.eecs.berkeley.edu/~jfc/cs174lecs/lec11/Image127.gif
		for (uint32_t index = 0; index < 8; index++)
		{
			SimpleMath::Vector3 vertex = origin;
			std::bitset<3> indexBit {index};

			if (indexBit.test(0))
			{
				vertex.x += size;
			}
			if (indexBit.test(1))
			{
				vertex.y += size;
			}

			if (indexBit.test(2))
			{
				vertex.z += size;
			}
			
			vertices[index] = vertex;

		}

		model = TriggerModel(vertices);
	}
#endif

};