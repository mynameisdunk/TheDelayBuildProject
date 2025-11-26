/*
  ==============================================================================

    Parameters.cpp
    Created: 10 Nov 2025 11:50:04am
    Author:  Harry

  ==============================================================================
*/

// 1
#include "Parameters.h"
// 2


template<typename T>
// replace T with whatever type I give you when calling the function
static void castParameter(juce::AudioProcessorValueTreeState& apvts,
const juce::ParameterID& id, T& destination)
{
destination = dynamic_cast<T>(apvts.getParameter(id.getParamID())); jassert(destination);
}
        // ^^ castParameter function is used to more easily 'dynamicCast' variables

static juce::String stringFromMilliseconds(float value, int)    //format frequency display decimels
{
    if (value < 10.0f){
        return juce::String(value, 2) + "ms";
    }
    else if (value < 100.0f){
        return juce::String(value, 1) + "ms";
    }
    else if (value < 1000.0f){
        return juce::String(int(value)) + "ms";
    }
    else {
        return juce::String(value * 0.001f, 2) + "s";
    }
}

static juce::String stringFromDecibels(float value, int)
{
    return juce::String(value, 1) + "dB";
}

static:: juce::String stringFromPercent(float value, int)
{
    return juce::String(int(value)) + "%";
}

Parameters::Parameters(juce::AudioProcessorValueTreeState& apvts)
{
    castParameter(apvts, gainParamID, gainParam);
    castParameter(apvts, delayTimeParamID, delayTimeParam);
    castParameter(apvts, mixParamID, mixParam);
    //          look here, find this, put it here
}

juce::AudioProcessorValueTreeState::ParameterLayout

Parameters::createParameterLayout()
{
juce::AudioProcessorValueTreeState::ParameterLayout layout;
layout.add
(std::make_unique<juce::AudioParameterFloat>
 (gainParamID, "Output Gain", juce::NormalisableRange<float> {-12.0f, 12.0f, 0.001f}, -12.0f, juce::AudioParameterFloatAttributes().withStringFromValueFunction(stringFromDecibels)));
    
layout.add
    (std::make_unique<juce::AudioParameterFloat>
     (delayTimeParamID, "Delay Time", juce::NormalisableRange<float> {minDelayTime, maxDelayTime, 0.001f, 0.25f}, 100.0f, juce::AudioParameterFloatAttributes().withStringFromValueFunction(stringFromMilliseconds)));
 
 layout.add
    (std::make_unique<juce::AudioParameterFloat>
     (mixParamID, "Mix", juce::NormalisableRange<float>{0.0f, 100.0f, 0.01f}, 100.0f, juce::AudioParameterFloatAttributes().withStringFromValueFunction(stringFromPercent)));
 
return layout;
// return is a keyword that tells the compiler that at the end of this code that the variable first declared called 'layout' now has a new value defined by the code above

// Incredibly simply put
// This function builds a small “list” (layout) of plugin parameters by creating a gain parameter with a specific ID ("Output Gain"), range {-12 - 12}, and display format (1 decimal place, dB suffix). It returns that list so JUCE knows what parameters your plugin has.
}

void Parameters::update() noexcept {
    gainSmoother.setTargetValue(juce::Decibels::gainToDecibels(gainParam->get()));
    // You read a line like this from the inside-out: First it does gainParam->get(), then it does decibelsToGain, and finally it does setTargetValue.
    targetDelayTime = delayTimeParam->get();
    if (delayTime == 0.0f){
        delayTime = targetDelayTime;
    }
    
    mixSmoother.setTargetValue(mixParam->get() * 0.01f);

}

void Parameters::prepareToPlay(double sampleRate) noexcept
{
    double duration = 0.02;
    gainSmoother.reset(sampleRate,duration);
    
    coeff = 1.0f - std::exp(-1.0f/(0.3f * float(sampleRate)));
                                // ^^^ this value determines how quickly the delay value is smoothed. 0.2f is similar to an analogue capacitor. Larger values are slower smoothing times.
    mixSmoother.reset(sampleRate, duration);
}

void Parameters::reset() noexcept
{
    gain = 0.0f;
    gainSmoother.setCurrentAndTargetValue(juce::Decibels::decibelsToGain(gainParam->get()));
    
    delayTime = 0.0f;
    
    mix = 1.0f;
    mixSmoother.setCurrentAndTargetValue(mixParam->get()*0.01f);
}

void Parameters::smoothen() noexcept
{
    gain = gainSmoother.getNextValue();
    
    delayTime += (targetDelayTime -delayTime) * coeff;
    
    mix = mixSmoother.getNextValue();
}
