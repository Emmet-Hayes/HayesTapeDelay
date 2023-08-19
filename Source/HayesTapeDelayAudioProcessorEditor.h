#pragma once

#include <JuceHeader.h>
#include "HayesTapeDelayAudioProcessor.h"
#include "../../Common/BaseAudioProcessorEditor.h"
#include "../../Common/CustomLookAndFeel.h"
#include "../../Common/FreqSlider.h"
#include "../../Common/PercentSlider.h"
#include "../../Common/TimeSlider.h"
#include "../../Common/DbSlider.h"
#include "../../Common/DepthSlider.h"
#include "../../Common/PresetBar.h"

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

    std::unique_ptr<TimeSlider> delayTimeSlider;
    std::unique_ptr<DbSlider> gainSlider;
    std::unique_ptr<DbSlider> feedbackSlider;
    std::unique_ptr<PercentSlider> wetDrySlider;
    std::unique_ptr<FreqSlider> filterCutOffSlider;
    std::unique_ptr<FreqSlider> filterCutOffSliderHi;
    std::unique_ptr<FreqSlider> flutterFreqSlider;
    std::unique_ptr<DepthSlider> flutterDepthSlider;
    std::unique_ptr<FreqSlider> wowFreqSlider;
    std::unique_ptr<DepthSlider> wowDepthSlider;

    std::unique_ptr<Label> delayTimeLabel;
    std::unique_ptr<Label> gainLabel;
    std::unique_ptr<Label> feedbackLabel;
    std::unique_ptr<Label> wetDryLabel;
    std::unique_ptr<Label> filterCutOffLabel;
    std::unique_ptr<Label> filterCutOffLabelHi;
    std::unique_ptr<Label> flutterFreqLabel;
    std::unique_ptr<Label> flutterDepthLabel;
    std::unique_ptr<Label> wowFreqLabel;
    std::unique_ptr<Label> wowDepthLabel;

    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> delayTimeAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> gainAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> feedbackAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> wetDryAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> filterCutOffAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> filterCutOffHiAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> flutterFreqAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> flutterDepthAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> wowFreqAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> wowDepthAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HayesTapeDelayAudioProcessorEditor)
};
