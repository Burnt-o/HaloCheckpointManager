#include "pch.h"
#include "BumpSetupGenerator.h"

constexpr int jumpTickLength = 41; // how long after pressing jump until you land on the ground again


constexpr int bumpWaitTime = 80; // how long to wait from main post setup inputs for tele




// for inside shield bumps, generated later
std::vector<InpMask> inputPossibiilites =
{




InpMask(Input::Forward),


InpMask(Input::Right),

InpMask(Input::Left),

InpMask(Input::Backward),


// adding two inputs at once is more than enough
InpMask(Input::Forward | Input::Right),

InpMask(Input::Forward | Input::Left),

InpMask(Input::Backward | Input::Right),

InpMask(Input::Backward | Input::Left),

	InpMask(Input::Crouch),
	InpMask(Input::Jump),
	InpMask(Input::Crouch | Input::Jump),

InpMask(Input::Forward | Input::Crouch),

InpMask(Input::Right | Input::Crouch),
InpMask(Input::Left | Input::Crouch),
InpMask(Input::Backward | Input::Crouch),


InpMask(Input::Forward | Input::Right | Input::Crouch),

InpMask(Input::Forward | Input::Left | Input::Crouch),

InpMask(Input::Backward | Input::Right | Input::Crouch),
InpMask(Input::Backward | Input::Left | Input::Crouch),


InpMask(Input::Forward | Input::Jump),

InpMask(Input::Right | Input::Jump),
InpMask(Input::Left | Input::Jump),
InpMask(Input::Backward | Input::Jump),


InpMask(Input::Forward | Input::Right | Input::Jump),

InpMask(Input::Forward | Input::Left | Input::Jump),


InpMask(Input::Backward | Input::Right | Input::Jump),
InpMask(Input::Backward | Input::Left | Input::Jump),


InpMask(Input::Forward | Input::Crouch | Input::Jump),

InpMask(Input::Right | Input::Crouch | Input::Jump),
InpMask(Input::Left | Input::Crouch | Input::Jump),
InpMask(Input::Backward | Input::Crouch | Input::Jump),

InpMask(Input::Forward | Input::Right | Input::Crouch | Input::Jump),

InpMask(Input::Forward | Input::Left | Input::Crouch | Input::Jump),


InpMask(Input::Backward | Input::Right | Input::Crouch | Input::Jump),
InpMask(Input::Backward | Input::Left | Input::Crouch | Input::Jump),

};










class BumpSetupGenerator::BumpSetupGeneratorImpl
{
private:
	long long getCount = -1;
	std::set<std::vector<uint32_t>> usedInputCombos;

public:

	std::vector<TestSetup> getTestCases()
	{
		std::vector<TestSetup> out;

//#ifdef SHIELD_BUMPS
//
//
//		{
//			// This is the teleport from the first shield into the cliff wall
//			BumpSetup bumpSetup;
//			// i thought it was 29 and 7 but it was actually 29 and 6. hard to calculate by hand.
//			//bumpSetup.ticksBeforeGoToJump = 29;
//			//bumpSetup.ticksAfterGoForShieldOn = 6;
//
//			bumpSetup.ticksBeforeGoToJump = 29;
//			bumpSetup.ticksAfterGoForShieldOn = 6;
//
//
//			bumpSetup.setupInputs = SetupInputVector{ };
//			bumpSetup.goInputs = GoInputVector{ InputAndLength{Input::Forward, bumpSetup.ticksAfterGoForShieldOn + 5} };
//			bumpSetup.goIdleInput = InpMask(Input::None);
//
//
//
//			SimpleMath::Vector3 expectedTeleportPosition = { -43.40472412109f, -0.30976089835f, 11.91835308075f };
//			std::vector<DirectX::BoundingBox> checkVolume = { DirectX::BoundingBox(expectedTeleportPosition, { 1.f, 1.f, 1.f }) };
//
//			out.push_back(TestSetup(bumpSetup, Nook("Shield1", { -53.50214385986f, 8.69638347626f, 4.18378925323f }), expectedTeleportPosition, 3.05326843262f, checkVolume));
//
//		}
//
//
//		//static_assert(false && "TODO: the tele into the wall");
//#else
//		{
//			// This is the first pog "reachable" teleport that I found from the cliff wall to the shafted area.
//			BumpSetup bumpSetup;
//			bumpSetup.setupInputs = SetupInputVector{ { InputAndLength{Input::Backward, 3}, false } };
//			bumpSetup.goInputs = GoInputVector{ InputAndLength{Input::Forward, bumpWaitTime} };
//			bumpSetup.goIdleInput = InpMask(Input::None);
//
//			SimpleMath::Vector3 expectedTeleportPosition = { -3.84005355835f, -30.00557327271f, 9.86668014526f };
//			DirectX::BoundingBox checkVolume = DirectX::BoundingBox(expectedTeleportPosition, { 1.f, 1.f, 1.f });
//
//			out.push_back(TestSetup(bumpSetup, Nook("Tau", { -47.54022598267f, -5.89266777039f, 3.81493878365f }), expectedTeleportPosition, 1.92306518555f, checkVolume));
//
//		}
//
//
//
//		{
//			// this is a useless teleport from a high nook in the cliff wall, crouching backwards and falling into a low nook. Good test of crouch inputs working properly tho.
//			BumpSetup bumpSetup;
//			bumpSetup.goInputs = GoInputVector{ InputAndLength{InpMask(Input::Crouch | Input::Backward), 1}, InputAndLength{Input::Backward, bumpWaitTime * 3} };
//			bumpSetup.goIdleInput = InpMask(Input::None);
//
//			SimpleMath::Vector3 expectedTeleportPosition = { -33.76911926270f, 0.14532114565f, 4.29699230194f };
//			DirectX::BoundingBox checkVolume = DirectX::BoundingBox(expectedTeleportPosition, { 1.f, 1.f, 1.f });
//
//			out.push_back(TestSetup(bumpSetup, Nook("Beta High", { -41.88874816895f, 2.43111014366f, 7.75332784653f }), expectedTeleportPosition, 4.25261735916f, checkVolume));
//
//		}
//#endif
//

		return out;


	}

	long long getSetupCount()
	{
		return usedInputCombos.size();
	}


BumpSetup getSetup()
{
	typedef std::pair<InpMask, int> InputAndTick;
	typedef std::vector<InputAndTick> OBSGoVector;
	SetupInputVector workingSetupVector = {};
	GoInputVector uncollapsedInputs = {};

	long long stuckCount = 0;
	while (true)
	{ 
		stuckCount++;


		if (stuckCount > 100000000) 
			throw HCMRuntimeException(std::format("Finished tests at setupcount: {}", usedInputCombos.size()));

		constexpr int totalInputLength = ISB_INPUT_LENGTH;
		getCount++;




		int firstIndex = getCount;


		InpMask goIdleInput = Input::None;
#ifdef ISB_IDLE_CROUCH
		if (getCount % 2 == 0)
			goIdleInput = Input::Crouch;
		firstIndex = getCount / 2;
#endif



#ifdef ISB_PRESETUPS
		//int preSetupPossibilities = (ISB_PRESETUPS * 4) + 1;
		//long long preSetupIndex = (firstIndex % preSetupPossibilities) - 1;
		//firstIndex = firstIndex / preSetupPossibilities;

#ifdef ISB_PRESETUPS_INCLUDE_CROUCH
		constexpr int comboCount = 8;
#else
		constexpr int comboCount = 4;
#endif


		std::vector<long long> presetupChoices;
		for (int i = 0; i < ISB_PRESETUP_COMBOS; i++)
		{

			int preSetupPossibilities = (ISB_PRESETUPS * comboCount) + 1;

			long long preSetupIndex = (firstIndex % preSetupPossibilities) - 1;
			firstIndex = firstIndex / preSetupPossibilities;
			presetupChoices.push_back(preSetupIndex);
		}

		workingSetupVector.clear();

		for (auto& presetupChoice : presetupChoices)
		{
			if (presetupChoice != -1)
			{

				InpMask whichInput = Input::None;


				switch ((presetupChoice % comboCount))
				{
				case 0: whichInput = Input::Forward; break;
				case 1: whichInput = Input::Right; break;
				case 2: whichInput = Input::Left; break;
				case 3: whichInput = Input::Backward; break;
				case 4: whichInput = Input::Forward | Input::Crouch; break;
				case 5: whichInput = Input::Right | Input::Crouch; break;
				case 6: whichInput = Input::Left | Input::Crouch; break;
				case 7: whichInput = Input::Backward | Input::Crouch; break;

				}

				int howManyTicks = (presetupChoice / comboCount) + 1;
				workingSetupVector.push_back(std::pair<InputAndLength, bool>(InputAndLength(whichInput, howManyTicks), false));
			}
		}
		
		LOG_ONCE(PLOG_DEBUG << "aight");
#endif

		int secondInputTick;
		int thirdInputTick;
		InpMask firstInput = Input::None;
		InpMask secondInput = Input::None;
		InpMask thirdInput = Input::None;
		{
			int firstInputToChoose = firstIndex % inputPossibiilites.size();
			int firstInputChooseRepeats = firstIndex / inputPossibiilites.size();
			// first input always at tick 0

			int secondInputToChoose = (firstInputChooseRepeats % (inputPossibiilites.size() + 1)) - 1;
			int secondInputChooseRepeats = firstInputChooseRepeats / (inputPossibiilites.size() + 1);
			secondInputTick = (secondInputChooseRepeats % (totalInputLength));
			int secondInputTickRepeats = secondInputChooseRepeats / (totalInputLength);



			int thirdInputToChoose = (secondInputTickRepeats % (inputPossibiilites.size() + 1)) - 1;
			int thirdInputChooseRepeats = secondInputTickRepeats / (inputPossibiilites.size() + 1);
			thirdInputTick = thirdInputChooseRepeats % (totalInputLength);

			firstInput = inputPossibiilites.at(firstInputToChoose);
			secondInput = secondInputToChoose == -1 ? InpMask(Input::None) : inputPossibiilites.at(secondInputToChoose);
			thirdInput = thirdInputToChoose == -1 ? InpMask(Input::None) : inputPossibiilites.at(thirdInputToChoose);
		}
		


#ifdef BEGIN_JUMP_ONLY
		firstInput = InpMask(Input::Jump);
#endif


#ifdef SPECIAL_OVERRIDE

		firstInput = InpMask(Input::Forward | Input::Right);
		secondInput = InpMask(Input::Jump);
		thirdInput = InpMask(Input::None);

		secondInputTick = 6;
		goIdleInput = InpMask(Input::None);

		//if (workingSetupVector.size() == 0)
		//	continue;
		//if (contains(workingSetupVector.at(0).first.first, Input::Right) == false)
		//	continue;

		//bool bad = false;
		//for (int i = 1; i < workingSetupVector.size(); i++)
		//{
		//	if (workingSetupVector.at(i).first.second > 5)
		//	{
		//		bad = true;
		//		break;
		//	}
		//}
		//if (bad)
		//	continue;

#endif

#ifdef OVERRIDE_SECOND_TICK

		if (secondInputTick < OVERRIDE_SECOND_TICK)
			secondInputTick = OVERRIDE_SECOND_TICK;

		if (firstInput == secondInput)
			continue;

#endif

		//if (thirdInputTick < secondInputTick && !secondInput.value && !thirdInput.value) // third input need to be after second input
		//	continue;

		//if (!thirdInput.value && secondInput.value) // can't have third input without a second input
		//	continue;




		//if (secondInputTick == 0) // second input can't happen on same tick as first input
		//	continue;

		InpMask endingInput = firstInput | secondInput | thirdInput;






		// cancelling inputs are bad
		if (contains(endingInput, Input::Forward) && contains(endingInput, Input::Backward))
			continue;

		if (contains(endingInput, Input::Left) && contains(endingInput, Input::Right))
			continue;

		// needs at least one movement input
		if (!contains(endingInput, Input::Forward) && !contains(endingInput, Input::Backward) && !contains(endingInput, Input::Left) && !contains(endingInput, Input::Right))
			continue;

		LOG_ONCE(PLOG_DEBUG << "aight2");

#ifndef INCLUDE_FORWARD
		if (contains(endingInput, Input::Forward))
			continue;
#endif

#ifndef INCLUDE_LEFT
		if (contains(endingInput, Input::Left))
			continue;
#endif

#ifndef INCLUDE_RIGHT
		if (contains(endingInput, Input::Right))
			continue;
#endif

#ifndef INCLUDE_BACKWARD
		if (contains(endingInput, Input::Backward))
			continue;
#endif


#ifndef INCLUDE_JUMP
		if (contains(endingInput, Input::Jump))
			continue;
#endif



#ifndef INCLUDE_CROUCH
		// slightly more complicated since toggling off crouch is a possibility
		if (contains(endingInput, Input::Crouch) ||
			contains(firstInput, Input::Crouch) ||
			(contains(secondInput, Input::Crouch)) ||
			(contains(thirdInput, Input::Crouch))
			)
			continue;
#endif








		LOG_ONCE(PLOG_DEBUG << "aight3");

		std::vector<uint32_t> asVals;
		asVals.push_back(goIdleInput.value);


		InpMask currentInput = firstInput;
		if (contains(goIdleInput, Input::Crouch) && contains(currentInput, Input::Crouch))
		{
			flip(currentInput, Input::Crouch);
		}

		uncollapsedInputs.clear();

		for (int tick = 0; tick < totalInputLength; tick++)
		{
			if (tick == secondInputTick)
			{
				if (contains(currentInput, Input::Crouch) && contains(secondInput, Input::Crouch))
				{
					currentInput |= secondInput;
					flip(currentInput, Input::Crouch);
				}
				else
				{
					currentInput |= secondInput;
				}
			}

			if (tick == thirdInputTick)
			{
				if (contains(currentInput, Input::Crouch) && contains(thirdInput, Input::Crouch))
				{
					currentInput |= thirdInput;
					flip(currentInput, Input::Crouch);
				}
				else
				{
					currentInput |= thirdInput;
				}
			}



			uncollapsedInputs.push_back(currentInput);
			asVals.push_back(currentInput.value);
		}







		LOG_ONCE(PLOG_DEBUG << "aight4");

#ifdef EXTRA_TICKS_AFTER_LAST_INPUT
		for (int i = 0; i < (EXTRA_TICKS_AFTER_LAST_INPUT); i++)
		{
			uncollapsedInputs.push_back(endingInput);
			asVals.push_back(endingInput.value);
		}
#endif


		for (auto& inpTick : workingSetupVector)
		{
			for (int t = 0; t < inpTick.first.second; t++)
			{
				asVals.push_back(inpTick.first.first.value);
			}

		}



		if (usedInputCombos.contains(asVals))
		{
			//PLOG_DEBUG << "collision, input size: " << usedInputCombos.size();
			continue;
		}
		else
		{
			PLOG_DEBUG << "ISB Setup Count: " << usedInputCombos.size();
			usedInputCombos.insert(asVals);
		}

		// Note: eventually you will exhaust all possible input combinations for a given totalInputLength
		// then the set is full and will always collide; thus causing this while loop to be stuck forever.

		if (usedInputCombos.size() < START_SETUP_COUNT)
		{
			continue;
		}


		BumpSetup out;
		out.setupInputs = workingSetupVector;
		out.goIdleInput = goIdleInput;
		out.goInputs = uncollapsedInputs;



		return out;
	}



}


	BumpSetupGeneratorImpl(int startIndex)
		: getCount(startIndex - 1) {}

};






// how many bits are different between a and b
int differentBetweenMasks(InpMask a, InpMask b)
{
	auto xormask = a ^ b;
	return std::popcount((uint32_t)xormask.value);
}









BumpSetupGenerator::BumpSetupGenerator(int startIndex)
{
	LOG_ONCE(PLOG_DEBUG << "BumpSetupGenerator()");
	pimpl = std::make_unique< BumpSetupGeneratorImpl>(startIndex);

	LOG_ONCE(PLOG_DEBUG << "BumpSetupGenerator() done");

}

std::vector<TestSetup> BumpSetupGenerator::getTestCases()
{
	return pimpl->getTestCases();
}

BumpSetupGenerator::~BumpSetupGenerator() = default;

BumpSetup BumpSetupGenerator::getSetup()
{
	return pimpl->getSetup();
}


long long BumpSetupGenerator::getSetupCount()
{
	return pimpl->getSetupCount();
}