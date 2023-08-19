#include "HayesTapeDelayAudioProcessor.h"
#include "HayesTapeDelayAudioProcessorEditor.h"

HayesTapeDelayAudioProcessorEditor::HayesTapeDelayAudioProcessorEditor (HayesTapeDelayAudioProcessor& p)
:   BaseAudioProcessorEditor{ p }
,   processor           { p }
,   presetBar           { p }
{
    setLookAndFeel(&customLookAndFeel);
  
    delayTimeSlider = std::make_unique<TimeSlider>();
    delayTimeSlider->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    delayTimeSlider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    gainSlider = std::make_unique<DbSlider>();
    gainSlider->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    gainSlider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    feedbackSlider = std::make_unique<DbSlider>();
    feedbackSlider->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    feedbackSlider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    wetDrySlider = std::make_unique<PercentSlider>();
    wetDrySlider->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    wetDrySlider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    filterCutOffSlider = std::make_unique<FreqSlider>();
    filterCutOffSlider->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    filterCutOffSlider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    filterCutOffSliderHi = std::make_unique<FreqSlider>();
    filterCutOffSliderHi->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    filterCutOffSliderHi->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    flutterFreqSlider = std::make_unique<FreqSlider>();
    flutterFreqSlider->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    flutterFreqSlider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    flutterDepthSlider = std::make_unique<DepthSlider>();
    flutterDepthSlider->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    flutterDepthSlider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    wowFreqSlider = std::make_unique<FreqSlider>();
    wowFreqSlider->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    wowFreqSlider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    wowDepthSlider = std::make_unique<DepthSlider>();
    wowDepthSlider->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    wowDepthSlider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);

    delayTimeLabel = std::make_unique<Label>("", "Delay Time");
    gainLabel = std::make_unique<Label>("", "Gain (dB)");
    feedbackLabel = std::make_unique<Label>("", "Feedback (dB)");
    wetDryLabel = std::make_unique<Label>("", "Mix");
    filterCutOffLabel = std::make_unique<Label>("", "Low Pass");
    filterCutOffLabelHi = std::make_unique<Label>("", "High Pass");
    flutterFreqLabel = std::make_unique<Label>("", "Flutter Freq");
    flutterDepthLabel = std::make_unique<Label>("", "Flutter Depth");
    wowFreqLabel = std::make_unique<Label>("", "Wow Freq");
    wowDepthLabel = std::make_unique<Label>("", "Wow Depth");

    delayTimeSlider->setRange(1, 8);
    addAndMakeVisible(delayTimeSlider.get());

    delayTimeLabel->setJustificationType(Justification::centred);
    delayTimeLabel->attachToComponent(delayTimeSlider.get(), false);
    addAndMakeVisible(delayTimeLabel.get());

    gainSlider->setRange(0.0f, 1.2f);
    gainLabel->setJustificationType(Justification::centred);
    gainLabel->attachToComponent(gainSlider.get(), false);
    addAndMakeVisible(gainSlider.get());

    feedbackSlider->setRange(-45.0, -1.0);
    feedbackLabel->setJustificationType(Justification::centred);
    feedbackLabel->attachToComponent(feedbackSlider.get(), false);
    addAndMakeVisible(feedbackSlider.get());


    wetDrySlider->setRange(0.0f, 1.0f);
    wetDryLabel->setJustificationType(Justification::centred);
    wetDryLabel->attachToComponent(wetDrySlider.get(), false);
    addAndMakeVisible(wetDrySlider.get());

    filterCutOffSlider->setRange(400.0f, 21000.0f);
    filterCutOffSlider->setSkewFactorFromMidPoint(600);
    filterCutOffLabel->setJustificationType(Justification::centred);
    filterCutOffLabel->attachToComponent(filterCutOffSlider.get(), false);
    addAndMakeVisible(filterCutOffSlider.get());

    filterCutOffSliderHi->setRange(400.0f, 21000.0f);
    filterCutOffSliderHi->setSkewFactorFromMidPoint(600);
    filterCutOffLabelHi->setJustificationType(Justification::centred);
    filterCutOffLabelHi->attachToComponent(filterCutOffSliderHi.get(), false);
    addAndMakeVisible(filterCutOffSliderHi.get());

    flutterFreqSlider->setRange(2.5f, 5.0f);
    flutterFreqLabel->setJustificationType(Justification::centred);
    flutterFreqLabel->attachToComponent(flutterFreqSlider.get(), false);
    addAndMakeVisible(flutterFreqSlider.get());

    flutterDepthSlider->setRange(-0.2f, 0.2f);
    flutterDepthLabel->setJustificationType(Justification::centred);
    flutterDepthLabel->attachToComponent(flutterDepthSlider.get(), false);
    addAndMakeVisible(flutterDepthSlider.get());

    wowFreqSlider->setRange(0.0f, 2.5f);
    wowFreqLabel->setJustificationType(Justification::centred);
    wowFreqLabel->attachToComponent(wowFreqSlider.get(), false);
    addAndMakeVisible(wowFreqSlider.get());

    wowDepthSlider->setRange(-0.2f, 0.2f);
    wowDepthLabel->setJustificationType(Justification::centred);
    wowDepthLabel->attachToComponent(wowDepthSlider.get(), false);
    addAndMakeVisible(wowDepthSlider.get());

    addAndMakeVisible(presetBar);

    delayTimeAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.getValueTreeState(), Parameters::delaytime.toString(), *delayTimeSlider);
    gainAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.getValueTreeState(), Parameters::gain.toString() , *gainSlider);
    feedbackAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.getValueTreeState(), Parameters::feedback.toString(), *feedbackSlider);
    wetDryAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.getValueTreeState(), Parameters::mix.toString(), *wetDrySlider);
    filterCutOffAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.getValueTreeState(), Parameters::lowpass.toString(), *filterCutOffSlider);
    filterCutOffHiAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.getValueTreeState(), Parameters::highpass.toString(), *filterCutOffSliderHi);
    flutterFreqAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.getValueTreeState(), Parameters::flutterfreq.toString(), *flutterFreqSlider);
    flutterDepthAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.getValueTreeState(), Parameters::flutterdepth.toString(), *flutterDepthSlider);
    wowFreqAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.getValueTreeState(), Parameters::wowfreq.toString(), *wowFreqSlider);
    wowDepthAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.getValueTreeState(), Parameters::wowdepth.toString(), *wowDepthSlider);
    
    image = image = juce::ImageCache::getFromMemory(BinaryData::bg_file_jpg, BinaryData::bg_file_jpgSize);
    setSize(700, 300);
}

HayesTapeDelayAudioProcessorEditor::~HayesTapeDelayAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

void HayesTapeDelayAudioProcessorEditor::paint (Graphics& g)
{
    g.drawImage (image, 0, 0, getWidth(), getHeight(), 0, 400, 1000, 428);
}

void HayesTapeDelayAudioProcessorEditor::resized()
{
    presetBar.setBounds(0, 0, 700, 20);
    delayTimeSlider->setBounds(15, 40, 100, 100);
    feedbackSlider->setBounds(135, 40, 100, 100);
    wetDrySlider->setBounds(15, 185, 100, 100);
    gainSlider->setBounds(135, 185, 100, 100);
    filterCutOffSlider->setBounds(290, 40, 100, 100);
    filterCutOffSliderHi->setBounds(290, 185, 100, 100);
    flutterFreqSlider->setBounds(465, 40, 100, 100);
    flutterDepthSlider->setBounds(585, 40, 100, 100);
    wowFreqSlider->setBounds(465, 185, 100, 100);
    wowDepthSlider->setBounds(585, 185, 100, 100);
}


