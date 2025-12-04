/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Parameters.h"


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
private:
    
    Parameters params {apvts};
 
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delayLine;
    
    float feedbackL = 0.0f;
    float feedbackR = 0.0f;
    
    float saturationCharacterDrive = 1.5f;
    float saturationCharacter(float x);
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TheDelayAudioProcessor)
};
