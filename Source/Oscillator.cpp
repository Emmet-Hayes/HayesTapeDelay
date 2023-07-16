#include "Oscillator.h"

void Oscillator::setFrequency(double frequency) {
	mFrequency = frequency;
	updateIncrement();
}

void Oscillator::setSampleRate(double sampleRate) {
	mSampleRate = sampleRate;
	updateIncrement();
}

void Oscillator::updateIncrement() {
	mPhaseIncrement = mFrequency / mSampleRate;
}

double Oscillator::nextSample(double frequency) {

	mPhaseIncrement = frequency / 44100.0;

	mPhase += mPhaseIncrement;
	while (mPhase > 1.0) {
		mPhase -= 1.0;
	}
	return 0.5 + 0.5 * sin(mPhase * 4.0 * acos(0.0));
}

/* Adapted from 'Flutterbird' by Tesselode, which helps to facilitate the wow and flutter functionality 
for the delay.
https://github.com/tesselode
*/