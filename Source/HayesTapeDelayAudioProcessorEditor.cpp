#include "HayesTapeDelayAudioProcessor.h"
#include "HayesTapeDelayAudioProcessorEditor.h"

HayesTapeDelayAudioProcessorEditor::HayesTapeDelayAudioProcessorEditor (HayesTapeDelayAudioProcessor& p)
:   BaseAudioProcessorEditor{ p }
,   processor           { p }
,   presetBar           { p }
{
    setLookAndFeel(&customLookAndFeel);

    auto initialise_slider = [&](juce::Slider* slider, float low, float high, bool skew)
    {
        slider->setSliderStyle(juce::Slider::Rotary);
        slider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
        slider->setRange(low, high);
        if (skew)
            slider->setSkewFactorFromMidPoint(600);
        addAndMakeVisible(slider);
    };

    sliders[0] = std::make_unique<TimeSlider>();
    initialise_slider(sliders[0].get(), 1.0f, 8.0f, false);
    sliders[1] = std::make_unique<DbSlider>();
    initialise_slider(sliders[1].get(), 0.0f, 1.2f, false);
    sliders[2] = std::make_unique<DbSlider>();
    initialise_slider(sliders[2].get(), -45.0f, -1.0f, false);
    sliders[3] = std::make_unique<PercentSlider>();
    initialise_slider(sliders[3].get(), 0.0f, 1.0f, false);
    sliders[4] = std::make_unique<FreqSlider>();
    initialise_slider(sliders[4].get(), 400.0f, 21000.0f, true);
    sliders[5] = std::make_unique<FreqSlider>();
    initialise_slider(sliders[5].get(), 400.0f, 21000.0f, true);
    sliders[6] = std::make_unique<FreqSlider>();
    initialise_slider(sliders[6].get(), 2.5f, 5.0f, false);
    sliders[7] = std::make_unique<DepthSlider>();
    initialise_slider(sliders[7].get(), -0.2f, 0.2f, false);
    sliders[8] = std::make_unique<FreqSlider>();
    initialise_slider(sliders[8].get(), 0.0f, 2.5f, false);
    sliders[9] = std::make_unique<DepthSlider>();
    initialise_slider(sliders[9].get(), -0.2f, 0.2f, false);

    auto initialise_label = [&](const char* name, std::unique_ptr<Label> label, juce::Slider* slider)
    {
        label = std::make_unique<juce::Label>("", name);
        label->setJustificationType(Justification::centred);
        label->attachToComponent(slider, false);
    };

    initialise_label("Delay Time",    std::move(labels[0]), sliders[0].get());
    initialise_label("Gain",          std::move(labels[1]), sliders[1].get());
    initialise_label("Feedback",      std::move(labels[2]), sliders[2].get());
    initialise_label("Mix",           std::move(labels[3]), sliders[3].get());
    initialise_label("Low Pass",      std::move(labels[4]), sliders[4].get());
    initialise_label("High Pass",     std::move(labels[5]), sliders[5].get());
    initialise_label("Flutter Freq",  std::move(labels[6]), sliders[6].get());
    initialise_label("Flutter Depth", std::move(labels[7]), sliders[7].get());
    initialise_label("Wow Freq",      std::move(labels[8]), sliders[8].get());
    initialise_label("Wow Depth",     std::move(labels[9]), sliders[9].get());

    addAndMakeVisible(presetBar);

    attachments[0] = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.getValueTreeState(), "delay time", *sliders[0]);
    attachments[1] = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.getValueTreeState(), "gain", *sliders[1]);
    attachments[2] = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.getValueTreeState(), "feedback", *sliders[2]);
    attachments[3] = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.getValueTreeState(), "mix", *sliders[3]);
    attachments[4] = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.getValueTreeState(), "lowpass", *sliders[4]);
    attachments[5] = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.getValueTreeState(), "highpass", *sliders[5]);
    attachments[6] = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.getValueTreeState(), "flutter frequency", *sliders[6]);
    attachments[7] = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.getValueTreeState(), "flutter depth", *sliders[7]);
    attachments[8] = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.getValueTreeState(), "wow frequency", *sliders[8]);
    attachments[9] = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.getValueTreeState(), "wow depth", *sliders[9]);
    
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
    sliders[0]->setBounds(15, 40, 100, 100);
    sliders[1]->setBounds(135, 40, 100, 100);
    sliders[2]->setBounds(15, 185, 100, 100);
    sliders[3]->setBounds(135, 185, 100, 100);
    sliders[4]->setBounds(290, 40, 100, 100);
    sliders[5]->setBounds(290, 185, 100, 100);
    sliders[6]->setBounds(465, 40, 100, 100);
    sliders[7]->setBounds(585, 40, 100, 100);
    sliders[8]->setBounds(465, 185, 100, 100);
    sliders[9]->setBounds(585, 185, 100, 100);
}
