#pragma once
#include <JuceHeader.h>

class PercentSlider : public juce::Slider
{
public:
    PercentSlider(juce::Slider::SliderStyle style, juce::Slider::TextEntryBoxPosition textPosition)
        : juce::Slider{ style, textPosition }
    {
    }

    juce::String getTextFromValue(double value) override
    {
        return juce::String(static_cast<int>(roundToInt(value * 100))) + "%";
    }
};

