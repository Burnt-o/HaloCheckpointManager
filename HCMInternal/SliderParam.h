#pragma once


// for float/int/vec input fields
template <typename podT>
struct SliderParam
{
	ImGuiSliderFlags sliderFlags = ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_NoRoundToFormat;
	bool showSlider;
	podT minValue;
	podT maxValue;
	constexpr SliderParam(); // sets showSlider to false
	constexpr SliderParam(podT min, podT max, ImGuiSliderFlags sliderFlags = ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_NoRoundToFormat) : showSlider(true), minValue(min), maxValue(max), sliderFlags(sliderFlags) {}
};

/* since constructors are constexpr, minValue and maxValue must be initialised,
even if showSlider is false and those values won't be used.
Cannot hide in .cpp file since compiler needs to see them right away */

template<>
constexpr SliderParam<int>::SliderParam()
	: showSlider(false), minValue(0), maxValue(100)
{

}

template<>
constexpr SliderParam<float>::SliderParam()
	: showSlider(false), minValue(0.f), maxValue(100.f)
{

}

template<>
constexpr SliderParam<SimpleMath::Vector2>::SliderParam()
	: showSlider(false), minValue(SimpleMath::Vector2{ 0.f, 0.f }), maxValue(SimpleMath::Vector2{ 100.f, 100.f })
{

}

template<>
constexpr SliderParam<SimpleMath::Vector3>::SliderParam()
	: showSlider(false), minValue(SimpleMath::Vector3{ 0.f, 0.f, 0.f }), maxValue(SimpleMath::Vector3{ 100.f, 100.f, 100.f })
{

}