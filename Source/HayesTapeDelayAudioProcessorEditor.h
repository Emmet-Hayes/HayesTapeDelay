#pragma once

#include <JuceHeader.h>
#include "HayesTapeDelayAudioProcessor.h"
#include "CustomLookAndFeel.h"

class HayesTapeDelayAudioProcessorEditor : public AudioProcessorEditor
									
{
public:
    HayesTapeDelayAudioProcessorEditor (HayesTapeDelayAudioProcessor&);

    void paint (Graphics&) override;
    void resized() override;

private:
    HayesTapeDelayAudioProcessor& processor;

	std::unique_ptr<Slider> delayTimeSlider;
	std::unique_ptr<Slider> gainSlider;
	std::unique_ptr<Slider> feedbackSlider;
	std::unique_ptr<Slider> wetDrySlider;
	std::unique_ptr<Slider> filterCutOffSlider;
	std::unique_ptr<Slider> filterCutOffSliderHi;
	std::unique_ptr<Slider> flutterFreqSlider;
	std::unique_ptr<Slider> flutterDepthSlider;
	std::unique_ptr<Slider> wowFreqSlider;
	std::unique_ptr<Slider> wowDepthSlider;

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

	std::unique_ptr<CustomLookAndFeel> customLookAndFeel;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HayesTapeDelayAudioProcessorEditor)
};
