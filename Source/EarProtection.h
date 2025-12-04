/*
  ==============================================================================

    EarProtection.h
    Created: 3 Dec 2025 2:05:50pm
    Author:  Harry

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

// Silences the buffer if bad or loud values are detected in the output buffer.
// Use this during debugging to avoid blowing out your eardrums on headphones.
inline void earProtection(juce::AudioBuffer<float>& buffer)
{
    bool firstWarning = true;
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
        float* channelData = buffer.getWritePointer(channel);
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
            float x = channelData[sample];
            bool silence = false;
            if (std::isnan(x)) {
                DBG("!!! WARNING: nan detected in audio buffer, silencing !!!");
                silence = true;
            } else if (std::isinf(x)) {
                DBG("!!! WARNING: inf detected in audio buffer, silencing !!!");
                silence = true;
            } else if (x < -2.0f || x > 2.0f) {  // screaming feedback
                DBG("!!! WARNING: sample out of range, silencing !!!");
                silence = true;
            } else if (x < -1.0f || x > 1.0f) {
                if (firstWarning) {
                    DBG("!!! WARNING: sample out of range: " << x << " !!!");
                    firstWarning = false;
                }
            }
            if (silence) {
                buffer.clear();
                return;
            }
        }
    }
}
