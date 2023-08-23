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

    auto initialise_label = [&](juce::Label* label, juce::Slider* slider)
    {
        label->setJustificationType(Justification::centred);
        label->attachToComponent(slider, false);
        addAndMakeVisible(label);
    };

    labels[0] = std::make_unique<juce::Label>("", "Delay Time");
    initialise_label(labels[0].get(), sliders[0].get());
    labels[1] = std::make_unique<juce::Label>("", "Gain");
    initialise_label(labels[1].get(), sliders[1].get());
    labels[2] = std::make_unique<juce::Label>("", "Feedback");
    initialise_label(labels[2].get(), sliders[2].get());
    labels[3] = std::make_unique<juce::Label>("", "Mix");
    initialise_label(labels[3].get(), sliders[3].get());
    labels[4] = std::make_unique<juce::Label>("", "Low Pass");
    initialise_label(labels[4].get(), sliders[4].get());
    labels[5] = std::make_unique<juce::Label>("", "High Pass");
    initialise_label(labels[5].get(), sliders[5].get());
    labels[6] = std::make_unique<juce::Label>("", "Flutter Freq");
    initialise_label(labels[6].get(), sliders[6].get());
    labels[7] = std::make_unique<juce::Label>("", "Flutter Depth");
    initialise_label(labels[7].get(), sliders[7].get());
    labels[8] = std::make_unique<juce::Label>("", "Wow Freq");
    initialise_label(labels[8].get(), sliders[8].get());
    labels[9] = std::make_unique<juce::Label>("", "Wow depth");
    initialise_label(labels[9].get(), sliders[9].get());

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
    
    const auto ratio = static_cast<double> (defaultWidth) / defaultHeight;
    setResizable(false, true);
    getConstrainer()->setFixedAspectRatio(ratio);
    getConstrainer()->setSizeLimits(defaultWidth, defaultHeight, defaultWidth * 2, defaultHeight * 2);
    setSize(defaultWidth, defaultHeight);
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
    const auto scale = static_cast<float> (getWidth()) / defaultWidth;

    auto setBoundsAndApplyScaling = [&](juce::Component* component, int x, int y, int w, int h, bool isSlider = true)
    {
        component->setBounds(static_cast<int>(x * scale), static_cast<int>(y * scale),
            static_cast<int>(w * scale), static_cast<int>(h * scale));
        if (isSlider)
            dynamic_cast<juce::Slider*>(component)->setTextBoxStyle(juce::Slider::TextBoxBelow, false, static_cast<int>(70 * scale), static_cast<int>(20 * scale));
    };

    customLookAndFeel.setWindowScale(scale);
    setBoundsAndApplyScaling(&presetBar, 0, 0, 700, 20, false);
    setBoundsAndApplyScaling(sliders[0].get(), 15, 40, 100, 100);
    setBoundsAndApplyScaling(sliders[1].get(), 135, 40, 100, 100);
    setBoundsAndApplyScaling(sliders[2].get(), 15, 185, 100, 100);
    setBoundsAndApplyScaling(sliders[3].get(), 135, 185, 100, 100);
    setBoundsAndApplyScaling(sliders[4].get(), 290, 40, 100, 100);
    setBoundsAndApplyScaling(sliders[5].get(), 290, 185, 100, 100);
    setBoundsAndApplyScaling(sliders[6].get(), 465, 40, 100, 100);
    setBoundsAndApplyScaling(sliders[7].get(), 585, 40, 100, 100);
    setBoundsAndApplyScaling(sliders[8].get(), 465, 185, 100, 100);
    setBoundsAndApplyScaling(sliders[9].get(), 585, 185, 100, 100);
}
