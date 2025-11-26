/*
  ==============================================================================

    Parameters.h
    Created: 10 Nov 2025 11:50:04am
    Author:  Harry

  ==============================================================================
*/


#pragma once
#include <JuceHeader.h>

const juce::ParameterID gainParamID {"gain", 1};
const juce::ParameterID delayTimeParamID {"delayTime", 1};
const juce::ParameterID mixParamID {"mix", 1};

class Parameters
{
public:
    Parameters(juce::AudioProcessorValueTreeState& apvts);
    
    static constexpr float minDelayTime = 25.0f;
    static constexpr float maxDelayTime = 1400.0f;
                                        // 1.4 seconds
                                        // this allows us to reference parameters::maxDelayTime instead of hardcoding a value into a process block
    
    
static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    void prepareToPlay(double sampleRate) noexcept;
    void reset() noexcept;
    void smoothen() noexcept;
    void update() noexcept;
    
    float gain = 0.0f;
    
    float delayTime = 0.0f;
    
    float mix = 1.0f;
    
private:
    juce::AudioParameterFloat* gainParam;
    juce::LinearSmoothedValue<float> gainSmoother;
    // this creates a new object for a float value which smooths the gain when the parameter is automated or changed quickly.. Prevents jumps from 0 to -6db gain, instead smoothly moving between values inbetween
    
    juce::AudioParameterFloat* delayTimeParam;
    
    float targetDelayTime = 0.0f;
    float coeff = 0.0f; // one pole smoothing
    
    juce::AudioParameterFloat* mixParam;
    juce::LinearSmoothedValue<float> mixSmoother;
    
  
};

