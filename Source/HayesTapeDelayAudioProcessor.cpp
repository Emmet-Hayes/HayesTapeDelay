#include <iostream>
#include <sstream>
#include <cmath>

#include "HayesTapeDelayAudioProcessor.h"
#include "HayesTapeDelayAudioProcessorEditor.h"


HayesTapeDelayAudioProcessor::HayesTapeDelayAudioProcessor()
    : BaseAudioProcessor{ createParameterLayout() }
{
    addParameterListeners();
}

AudioProcessorValueTreeState::ParameterLayout HayesTapeDelayAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;
    {
        using FloatParamPair = std::pair<Identifier, AudioParameterFloat*&>;

        for (auto p : { FloatParamPair(Parameters::gain, gain),
                        FloatParamPair(Parameters::delaytime, delaytime),
                        FloatParamPair(Parameters::feedback, feedback),
                        FloatParamPair(Parameters::mix, mix),
                        FloatParamPair(Parameters::lowpass, lowpass),
                        FloatParamPair(Parameters::highpass, highpass),
                        FloatParamPair(Parameters::flutterfreq, flutterfreq),
                        FloatParamPair(Parameters::flutterdepth, flutterdepth),
                        FloatParamPair(Parameters::wowfreq, wowfreq),
                        FloatParamPair(Parameters::wowdepth, wowdepth),

            })
        {
            auto& info = Parameters::parameterInfoMap[p.first];
            auto param = std::make_unique<AudioParameterFloat>(p.first.toString(), info.labelName, NormalisableRange<float>(info.lowerLimit, info.upperLimit, info.interval), info.defaultValue);

            p.second = param.get();
            params.push_back(std::move(param));
        }
    }
    return { params.begin(), params.end() };
}

void HayesTapeDelayAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    const int numInputChannels = getTotalNumInputChannels();
    const int delayBufferSize = static_cast<int>(sampleRate * 10);
    mSampleRate = static_cast<int>(sampleRate);

    delayBuffer.setSize(numInputChannels, delayBufferSize, false, true);
    wetBuffer.setSize(numInputChannels, samplesPerBlock, false, true);

    dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    lowPassFilter0.setCoefficients(IIRCoefficients::makeLowPass(mSampleRate, 15000.0f));
    lowPassFilter1.setCoefficients(IIRCoefficients::makeLowPass(mSampleRate, 15000.0f));
    hiPassFilter0.setCoefficients(IIRCoefficients::makeHighPass(mSampleRate, 300.0f));
    hiPassFilter1.setCoefficients(IIRCoefficients::makeHighPass(mSampleRate, 300.0f));

    oscFlutterL.setFrequency(1.0);
    oscFlutterL.setSampleRate(mSampleRate);
    oscFlutterR.setFrequency(1.0);
    oscFlutterR.setSampleRate(mSampleRate);
    oscWowL.setFrequency(1.0);
    oscWowL.setSampleRate(mSampleRate);
    oscWowR.setFrequency(1.0);
    oscWowR.setSampleRate(mSampleRate);
    updateProcessing();
}

void HayesTapeDelayAudioProcessor::addParameterListeners()
{
    apvts.addParameterListener(Parameters::gain.toString(), this);
    apvts.addParameterListener(Parameters::delaytime.toString(), this);
    apvts.addParameterListener(Parameters::feedback.toString(), this);
    apvts.addParameterListener(Parameters::mix.toString(), this);
    apvts.addParameterListener(Parameters::lowpass.toString(), this);
    apvts.addParameterListener(Parameters::highpass.toString(), this);
    apvts.addParameterListener(Parameters::flutterfreq.toString(), this);
    apvts.addParameterListener(Parameters::flutterdepth.toString(), this);
    apvts.addParameterListener(Parameters::wowfreq.toString(), this);
    apvts.addParameterListener(Parameters::wowdepth.toString(), this);
}

void HayesTapeDelayAudioProcessor::parameterChanged(const String& parameterID, float newValue)
{
    if (parameterID == Parameters::gain.toString())
        *gain = newValue;
    if (parameterID == Parameters::delaytime.toString())
        *delaytime = newValue;
    if (parameterID == Parameters::feedback.toString())
        *feedback = newValue;
    if (parameterID == Parameters::mix.toString())
        *mix = newValue;
    if (parameterID == Parameters::lowpass.toString())
        *lowpass = newValue;
    if (parameterID == Parameters::highpass.toString())
        *highpass = newValue;
    if (parameterID == Parameters::flutterfreq.toString())
        *flutterfreq = newValue;
    if (parameterID == Parameters::flutterdepth.toString())
        *flutterdepth = newValue;
    if (parameterID == Parameters::wowfreq.toString())
        *wowfreq = newValue;
    if (parameterID == Parameters::wowdepth.toString())
        *wowdepth = newValue;
    updateProcessing();
}

void HayesTapeDelayAudioProcessor::updateProcessing()
{
    theDelayEngine.gainInput = (float)(Decibels::decibelsToGain((float)*gain));
    theDelayEngine.delayTimeInput = *delaytime;
    theDelayEngine.feedbackInput = (float)(Decibels::decibelsToGain((float)*feedback));
    theDelayEngine.mixInput = *mix;

    updateOscillator(0);
    updateOscillator(1);
    updateFilter();
}

void HayesTapeDelayAudioProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& /*midiMessages*/)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    const int delayBufferLength = delayBuffer.getNumSamples();
    const int bufferLength = buffer.getNumSamples();

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        const float* bufferReadPtr = buffer.getReadPointer(channel);
        float* bufferWritePtr = buffer.getWritePointer(channel);
        float* wetBufferWritePtr = wetBuffer.getWritePointer(channel);

        // fill delayBuffer
        fillBuffer(channel, bufferLength, delayBufferLength, bufferReadPtr, dBWritePositionR, theDelayEngine.gainInput, lastInputGain);
        lastInputGain = theDelayEngine.gainInput;

        float writeValue = {};
        updateFilter(); // update values for high-pass & low-pass filters
        /* sample processing loop */
        for (int i = 0; i < bufferLength; i++)
        {
            int k;
            float delayTimeInSamples;
            double bpm = 120.0; // reasonable default

            AudioPlayHead* const ph = getPlayHead();
            if (ph != nullptr && ph->getPosition())
                bpm = *ph->getPosition()->getBpm();

            /* send 1 unit of delay from delay buffer to output buffer*/
            if (channel == 0)
                k = dBWritePositionL;
            else
                k = dBWritePositionR;

            /* get delay time from user*/
            float delayTimeInput = (120000.0f / theDelayEngine.delayTimeInput) / static_cast<float>(bpm); //half, quarter, dotted, eighth note

                                                                                     /* calculate modulation length in samples*/
            delayTimeInSamples = mSampleRate * delayTimeInput / 1000.0f;
            float delaySampleFloor = (floor)(delayTimeInSamples);

            /* create clean signal */
            float cleansig = *bufferWritePtr;

            if (delayTimeInSamples == 0.f)
                *wetBufferWritePtr = cleansig;
            else
            {
                updateOscillator(channel); //returns mod value
                if (delayTimeInSamples - delaySampleFloor != 0)
                    delayTimeInSamples -= (delayTimeInSamples - delaySampleFloor);

                /*get values for interpolation*/
                writeValue = calcWriteValue(channel, delayBuffer, k, i, delayBufferLength, delayTimeInSamples, static_cast<float>(mod));
                *wetBufferWritePtr = writeValue * sqrt(theDelayEngine.mixInput);

                /* low & hi pass filter */
                if (channel == 0)
                {
                    *wetBufferWritePtr = lowPassFilter0.processSingleSampleRaw(*wetBufferWritePtr);
                    *wetBufferWritePtr = hiPassFilter0.processSingleSampleRaw(*wetBufferWritePtr);
                }
                else if (channel == 1) {
                    *wetBufferWritePtr = lowPassFilter1.processSingleSampleRaw(*wetBufferWritePtr);
                    *wetBufferWritePtr = hiPassFilter1.processSingleSampleRaw(*wetBufferWritePtr);
                }
                /* wave shaping k = 2*/
                *wetBufferWritePtr = static_cast<float>((1 / atan(2)) * atan(2 * *wetBufferWritePtr));

                /* mix wet and dry signals */
                *bufferWritePtr = *wetBufferWritePtr + (sqrt(1 - theDelayEngine.mixInput) * cleansig);

                /* send feedback from output buffer to delay buffer*/
                delayBuffer.addFromWithRamp(channel, (k + i) % delayBufferLength, bufferWritePtr, 1, theDelayEngine.feedbackInput, lastFeedbackGain);
                lastFeedbackGain = theDelayEngine.feedbackInput;
            }
            bufferWritePtr++;
            wetBufferWritePtr++;
        }

        if (channel == 0)
        {
            dBWritePositionL += bufferLength;
            dBWritePositionL %= delayBufferLength;
        }
        else if (channel == 1) {
            dBWritePositionR += bufferLength;
            dBWritePositionR %= delayBufferLength;
        }
    }
}

float HayesTapeDelayAudioProcessor::calcWriteValue(int channel, AudioBuffer<float>& buffer, int k, int i, int delayBufferLength, float delayTimeInSamples, float modifier)
{
    float kk = (float)k;
    float ii = (float)i;
    float delayBufferLengthF = (float)delayBufferLength;

    float floorValue0 = buffer.getSample(channel, ((int)(floor)(kk + ii - delayTimeInSamples - modifier + delayBufferLengthF) - 1) % delayBufferLength);
    float floorValue = buffer.getSample(channel, ((int)(floor)(kk + ii - delayTimeInSamples - modifier + delayBufferLengthF) - 0) % delayBufferLength);
    float floorValue1 = buffer.getSample(channel, ((int)(ceil)(kk + ii - delayTimeInSamples - modifier + delayBufferLengthF) + 0) % delayBufferLength);
    float floorValue2 = buffer.getSample(channel, ((int)(ceil)(kk + ii - delayTimeInSamples - modifier + delayBufferLengthF) + 1) % delayBufferLength);

    //  return cubicInterp(channel, delayTimeInSamples, floorValue0, floorValue, floorValue1, floorValue2, mod);
    return interpolate(floorValue0, floorValue, floorValue1, floorValue2, delayTimeInSamples, modifier);
}

/* delay buffer is filled with contents from the output buffer*/
void HayesTapeDelayAudioProcessor::fillBuffer(int channel, const int bufferLength, const int delayBufferLength, const float* bufferReadPtr,
    int dBWritePosition, float startGain, float endGain)
{
    if (delayBufferLength > bufferLength + dBWritePosition)
    {
        delayBuffer.copyFromWithRamp(channel, dBWritePosition, bufferReadPtr, bufferLength, startGain, endGain);
    }
    else {
        const int bufferRemaining = delayBufferLength - dBWritePosition;
        const float midGain = lastInputGain + ((endGain - startGain) / bufferLength) * (bufferRemaining / bufferLength);
        delayBuffer.copyFromWithRamp(channel, dBWritePosition, bufferReadPtr, bufferRemaining, startGain, midGain);
        delayBuffer.copyFromWithRamp(channel, 0, bufferReadPtr + bufferRemaining, bufferLength - bufferRemaining, midGain, endGain);
    }
}

/* time shift is done here from delay buffer to output buffer*/
void HayesTapeDelayAudioProcessor::fetchDelay(AudioBuffer<float>& buffer, int channel, const int feedbackBufferLength,
    const int delayBufferLength, const float* /*feedbackBufferPtr*/, const float* delayBufferPtr, float /*startGain*/, float /*endGain*/)
{
    int delayTimeInput = static_cast<int>(*delaytime);
    int delayTimeInSamples = static_cast<int>(mSampleRate * delayTimeInput / 1000.0f);
    const int readPosition = (int)(delayBufferLength + dBWritePositionL - delayTimeInSamples) % delayBufferLength;

    if (delayBufferLength > feedbackBufferLength + readPosition)
        buffer.copyFrom(channel, 0, delayBufferPtr + readPosition, feedbackBufferLength);
    else
    {
        const int bufferRemaining = delayBufferLength - readPosition;
        buffer.copyFrom(channel, 0, delayBufferPtr + readPosition, bufferRemaining);
        buffer.copyFrom(channel, bufferRemaining, delayBufferPtr, feedbackBufferLength - bufferRemaining);
    }
}

/* send feedback from output buffer to delay buffer*/
void HayesTapeDelayAudioProcessor::sendFeedback(AudioBuffer<float>& /*buffer*/, int channel, const int feedbackBufferLength, const int delayBufferLength, float* feedbackBufferWritePtr,
    float startGain, float endGain)
{
    if (delayBufferLength > feedbackBufferLength + dBWritePositionL)
        delayBuffer.addFromWithRamp(channel, dBWritePositionL, feedbackBufferWritePtr, feedbackBufferLength, startGain, endGain);
    else
    {
        const int bufferRemaining = delayBufferLength - dBWritePositionL;
        const float midGain = lastInputGain + ((endGain - startGain) / feedbackBufferLength) * (bufferRemaining / static_cast<float>(feedbackBufferLength));
        delayBuffer.addFromWithRamp(channel, dBWritePositionL, feedbackBufferWritePtr, bufferRemaining, startGain, midGain);
        delayBuffer.addFromWithRamp(channel, 0, feedbackBufferWritePtr, feedbackBufferLength - bufferRemaining, midGain, endGain);
    }
}


AudioProcessorValueTreeState& HayesTapeDelayAudioProcessor::getValueTreeState()
{
    return apvts;
}

AudioProcessorEditor* HayesTapeDelayAudioProcessor::createEditor()
{
    return new HayesTapeDelayAudioProcessorEditor(*this);
}

void HayesTapeDelayAudioProcessor::updateFilter()
{
    lowPassFilter0.setCoefficients(IIRCoefficients::makeLowPass(mSampleRate, *lowpass));
    lowPassFilter1.setCoefficients(IIRCoefficients::makeLowPass(mSampleRate, *lowpass));
    hiPassFilter0.setCoefficients(IIRCoefficients::makeHighPass(mSampleRate, *highpass));
    hiPassFilter1.setCoefficients(IIRCoefficients::makeHighPass(mSampleRate, *highpass));
}

void HayesTapeDelayAudioProcessor::getStateInformation(MemoryBlock& destData)
{
    MemoryOutputStream stream(destData, false);
    apvts.state.writeToStream(stream);
}

void HayesTapeDelayAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    ValueTree tree = ValueTree::readFromData(data, sizeInBytes);
    if (tree.isValid())
        apvts.state = tree;
}

double HayesTapeDelayAudioProcessor::updateOscillator(int channel)
{
    auto target = 0.0;
    auto flutterDepth = *flutterdepth * 0.5;
    auto wowDepth = *wowdepth * 0.5;
    //auto totalDepth = flutterDepth + wowDepth;
    //auto followSpeed = 1.0;

    if (channel == 0)
    {
        auto oscFlutterLValue = oscFlutterL.nextSample(*flutterfreq);
        if (flutterDepth < 0.0)
        {
            flutterDepth *= -1.0;
            oscFlutterLValue = 1.0 - oscFlutterLValue;
        }
        target += flutterDepth * oscFlutterLValue;

        auto oscWowLValue = oscWowL.nextSample(*wowfreq);
        if (wowDepth < 0.0)
        {
            wowDepth *= -1.0;
            oscWowLValue = 1.0 - oscWowLValue;
        }
        target += wowDepth * oscWowLValue;

        auto followSpeed = 1.0;
        if (flutterDepth && wowDepth == 0.0)
            followSpeed = 10.0;

        oscLPosition += (target - oscLPosition) * followSpeed * (double)(1.0 / mSampleRate);
        mod = oscLPosition * (double)mSampleRate;
    }
    else if (channel == 1)
    {
        auto oscFlutterRValue = oscFlutterR.nextSample(*flutterfreq);
        if (flutterDepth < 0.0)
        {
            flutterDepth *= -1.0;
            oscFlutterRValue = 1.0 - oscFlutterRValue;
        }
        target += flutterDepth * oscFlutterRValue;

        auto oscWowRValue = oscWowR.nextSample(*wowfreq);
        if (wowDepth < 0.0)
        {
            wowDepth *= -1.0;
            oscWowRValue = 1.0 - oscWowRValue;
        }
        target += wowDepth * oscWowRValue;

        auto followSpeed = 1.0;
        if (flutterDepth && wowDepth == 0.0)
            followSpeed = 10.0;

        oscRPosition += (target - oscRPosition) * followSpeed * (double)(1.0 / mSampleRate);
        mod = (oscRPosition * (double)mSampleRate);
    }
    return mod;


    /* Adapted from 'Flutterbird' by Tesselode, which helps to facilitate the wow and flutter functionality
    for the delay.
    https://github.com/tesselode
    */
}

AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new HayesTapeDelayAudioProcessor();
}