/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Parameters.h"
#include "RotaryKnob.h"
#include "LookAndFeel.h"

//==============================================================================
/**
*/
class TheDelayAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    TheDelayAudioProcessorEditor (TheDelayAudioProcessor&);
    ~TheDelayAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    TheDelayAudioProcessor& audioProcessor;
    
    RotaryKnob gainKnob { "Gain", audioProcessor.apvts, gainParamID, true };
    RotaryKnob mixKnob {"Mix", audioProcessor.apvts, mixParamID};
    RotaryKnob delayTimeKnob {"Time", audioProcessor.apvts, delayTimeParamID};
    RotaryKnob feedbackKnob {"Feedback", audioProcessor.apvts, feedbackParamID, true};
    
    juce::GroupComponent delayGroup, feedbackGroup, outputGroup;
    
    MainLookAndFeel mainLF;

    /*
     juce::AudioProcessorValueTreeState::SliderAttachment attachment
    {
        audioProcessor.apvts, gainParamID.getParamID(), gainKnob.slider
    };
    */
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TheDelayAudioProcessorEditor)
};
