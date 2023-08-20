#pragma once
#include "HayesTapeDelayAudioProcessor.h"
#include "../../Common/BaseAudioProcessorEditor.h"
#include "../../Common/CustomLookAndFeel.h"
#include "../../Common/FreqSlider.h"
#include "../../Common/PercentSlider.h"
#include "../../Common/TimeSlider.h"
#include "../../Common/DbSlider.h"
#include "../../Common/DepthSlider.h"
#include "../../Common/PresetBar.h"

constexpr int NUM_SLIDERS = 10;

class HayesTapeDelayAudioProcessorEditor : public BaseAudioProcessorEditor         
{
public:
    HayesTapeDelayAudioProcessorEditor (HayesTapeDelayAudioProcessor&);
    ~HayesTapeDelayAudioProcessorEditor();
    void paint (Graphics&) override;
    void resized() override;

private:
    CustomLookAndFeel customLookAndFeel;

    HayesTapeDelayAudioProcessor& processor;

    juce::Image image;
    
    PresetBar presetBar;

    std::unique_ptr<juce::Slider> sliders[NUM_SLIDERS]; // time feedback wetdry filterlo filter hi flutterfreq flutterdepth wowfreq wowdepth
    std::unique_ptr<juce::Label> labels[NUM_SLIDERS];
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> attachments[NUM_SLIDERS];

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HayesTapeDelayAudioProcessorEditor)
};
