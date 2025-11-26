/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TheDelayAudioProcessorEditor::TheDelayAudioProcessorEditor (TheDelayAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
}

TheDelayAudioProcessorEditor::~TheDelayAudioProcessorEditor()
{
}

//==============================================================================
void TheDelayAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::cornsilk);

    g.setColour (juce::Colours::cadetblue);
    g.setFont (juce::FontOptions(90.0f));
    g.drawFittedText ("First Edits", getLocalBounds(), juce::Justification::centred, 1);
}

void TheDelayAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
