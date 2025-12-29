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
    RotaryKnob stereoKnob {"Stereo", audioProcessor.apvts, stereoParamID, true};
    RotaryKnob highCutKnob {"High Cut", audioProcessor.apvts, highCutParamID};
    RotaryKnob lowCutKnob {"Low Cut", audioProcessor.apvts, lowCutParamID};
    // RotaryKnob resonanceKnob {"Resonance", audioProcessor.apvts, resonanceParamID};
    RotaryKnob dirtKnob {"Dirt", audioProcessor.apvts, dirtParamID};
    RotaryKnob inputLevelKnob {"Input Level", audioProcessor.apvts, inputLevelParamID, false, false};
    RotaryKnob lowShelfGainKnob {"Lows", audioProcessor.apvts, lowShelfParamID, true};
    RotaryKnob highShelfGainKnob {"Highs", audioProcessor.apvts, highShelfParamID, true};
    RotaryKnob wowFlutterKnob {"Machine Age", audioProcessor.apvts, wowFlutterParamID, false, false};
    
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
