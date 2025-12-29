/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Parameters.h"
#include "Character.h"

// This declares a constant named gainParamID that is a juce::ParameterID object. As before this uses the identifier "gain" and the version hint 1. The idea is that this is the only place you’ll ever type the string "gain". Everywhere else you’ll use gainParamID instead.
//==============================================================================
/**
*/
class TheDelayAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    TheDelayAudioProcessor();           // constructor
    ~TheDelayAudioProcessor() override; // destructor

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    // This is where the audio processing takes place
    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    juce::AudioProcessorValueTreeState apvts
    {
        *this, nullptr, "Parameters", Parameters::createParameterLayout()
       
    };
    
    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    // used to recal plugin preset information
    
    float processFeedbackCharacter(float wetSignal, float& hissHPState, float& hissLPState);
    
private:
    
    Parameters params {apvts};
 
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delayLine;
    
    float copyOfSampleRate;
    
    float feedbackL = 0.0f;
    float feedbackR = 0.0f;

// TPT Filters
    juce::dsp::StateVariableTPTFilter<float> lowCutFilter;
    juce::dsp::StateVariableTPTFilter<float> highCutFilter;
    juce::dsp::StateVariableTPTFilter<float> hissHighPass;
    juce::dsp::StateVariableTPTFilter<float> hissLowPass;
    juce::dsp::StateVariableTPTFilter<float> outputHighCut;
    juce::dsp::StateVariableTPTFilter<float> outputLowCut;
    
// IIR Filters
    juce::dsp::IIR::Filter<float> lowShelf;
    juce::dsp::IIR::Filter<float> highShelf;
    
    void updateShelfFilters();
    
    float lastLowCut = -1.0f;
    float lastHighCut = -1.0f;
    float lastHissHighPass = 3000.0f; // This is the hardcoded value for the hissHighPass
    float lastHissLowPass = 14000.0f; // This is the hardcoded value for the hissLowPass
    float lastLowShelf = 1.0f;
    float lastHighShelf = 1.0f;
    float lastOutputHighCut = 14000.0f;
    float lastOutputLowCut = 50.0f;
    
    feedbackCharacter fc;
    // ^ this creates an instance of your class feedbackCharacter. It allows all member functions to be called using fc.'functionname' BALLER

    float wowPhase = 0.0f;
    float flutterPhase = 0.0f;
    
    float wowRate = 0.3f; // Hz
    float flutterRate = 6.0f; // Hz
    
    float wowDepth = 3.0f; // samples
    float flutterDepth = 0.5f; // samples
    
    float wowFlutterAmount = 0.0f;
    
    float baseDelayInSamples = 0.0f;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TheDelayAudioProcessor)
};
