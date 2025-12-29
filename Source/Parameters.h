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
const juce::ParameterID feedbackParamID {"feedback", 1};
const juce::ParameterID stereoParamID {"stereo", 1};
const juce::ParameterID lowCutParamID {"lowCut", 1};
const juce::ParameterID highCutParamID {"highCut", 1};
const juce::ParameterID resonanceParamID {"resonance", 1};
const juce::ParameterID dirtParamID {"dirt", 1};
const juce::ParameterID inputLevelParamID {"inputDrive", 1};
const juce::ParameterID lowShelfParamID {"lowShelf", 1};
const juce::ParameterID highShelfParamID {"highShelf", 1};
const juce::ParameterID wowFlutterParamID {"wowFlutter", 1};


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
    float feedback = 1.0f;
    float panL = 0.0f;
    float panR = 1.0f;
    float stereo = 0.0f;
    float lowCut = 20.0f;
    float highCut = 20000.0f;
    float resonance = 0.5f;
    float dirt = 0.0f;
    float inputLevel = 0.0f;
    float lowShelf = 1.0f;
    float highShelf = 1.0f;
    float wowFlutter = 0.0f;
    
    
    
private:
    juce::AudioParameterFloat* gainParam;
    juce::LinearSmoothedValue<float> gainSmoother;
    // this creates a new object for a float value which smooths the gain when the parameter is automated or changed quickly.. Prevents jumps from 0 to -6db gain, instead smoothly moving between values inbetween
    
    juce::AudioParameterFloat* delayTimeParam;
    float targetDelayTime = 0.0f;
    float coeff = 0.0f; // one pole smoothing
    
    juce::AudioParameterFloat* mixParam;
    juce::LinearSmoothedValue<float> mixSmoother;
    
    juce::AudioParameterFloat* feedbackParam;
    juce::LinearSmoothedValue<float> feedbackSmoother;
    
    juce::AudioParameterFloat* stereoParam;
    juce::LinearSmoothedValue<float> stereoSmoother;
    
    juce::AudioParameterFloat* lowCutParam;
    juce::LinearSmoothedValue<float> lowCutSmoother;
    
    juce::AudioParameterFloat* highCutParam;
    juce::LinearSmoothedValue<float> highCutSmoother;
    
    juce::AudioParameterFloat* resonanceParam;
    juce::LinearSmoothedValue<float> resonanceSmoother;
    
    juce::AudioParameterFloat* dirtParam;
    juce::LinearSmoothedValue<float> dirtSmoother;
    
    juce::AudioParameterFloat* inputLevelParam;
    juce::LinearSmoothedValue<float> inputLevelSmoother;
    
    juce::AudioParameterFloat* lowShelfParam;
    juce::LinearSmoothedValue<float> lowShelfSmoother;
    
    juce::AudioParameterFloat* highShelfParam;
    juce::LinearSmoothedValue<float> highShelfSmoother;
    
    juce::AudioParameterFloat* wowFlutterParam;
    juce::LinearSmoothedValue<float> wowFlutterSmoother;
    
  
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Parameters)
};

