#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Parameters.h"
#include "RotaryKnob.h"

//==============================================================================================================
TheDelayAudioProcessorEditor::TheDelayAudioProcessorEditor (TheDelayAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    delayGroup.setText("Delay");
    delayGroup.setTextLabelPosition(juce::Justification::horizontallyCentred);
    delayGroup.addAndMakeVisible(delayTimeKnob);
    addAndMakeVisible(delayGroup);
    
    feedbackGroup.setText("Feedback");
    feedbackGroup.setTextLabelPosition(juce::Justification::horizontallyCentred);
    feedbackGroup.addAndMakeVisible(feedbackKnob);
    addAndMakeVisible(feedbackGroup);
    
    outputGroup.setText("Output");
    outputGroup.setTextLabelPosition(juce::Justification::horizontallyCentred);
    outputGroup.addAndMakeVisible(mixKnob);
    outputGroup.addAndMakeVisible(gainKnob);
    addAndMakeVisible(outputGroup);
    
    setLookAndFeel(&mainLF);
    
    setSize (500, 330);
    //^^ this must always be the last part of the constructor
}

//==============================================================================================================

TheDelayAudioProcessorEditor::~TheDelayAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}
// deconstructor for TheDelayAudioProcessorEditor

//==============================================================================================================

void TheDelayAudioProcessorEditor::paint (juce::Graphics& g)
{
    auto noise = juce::ImageCache::getFromMemory(BinaryData::Noise_png, BinaryData::Noise_pngSize);
    auto fillType = juce::FillType(noise, juce::AffineTransform::scale(0.5f));
    g.setFillType(fillType);
    g.fillRect(getLocalBounds());
//{
    auto rect = getLocalBounds().withHeight(40);
    g.setColour(juce::Colours::pink);
    g.fillRect(rect);
    // creates a rectangle at the top of the image and paints it pink
    
    auto image = juce::ImageCache::getFromMemory(BinaryData::Logo_png, BinaryData::Logo_pngSize);
    // uses the juce::ImageCache to load the image into a juce::Image object
    
    int destWidth = image.getWidth() / 2;
    int destHeight = image.getHeight() / 2;
    
    g.drawImage(image, getWidth() / 2 - destWidth / 2, 0, destWidth, destHeight, 0, 0, image.getWidth(), image.getHeight());
 //} These lines are adding our Logo_png to the top of our plugin
}

//==============================================================================================================

void TheDelayAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    
    int y = 50; // creates a gap for 10 pixels from bottom of the header
    int height = bounds.getHeight() - 60;
    
    delayGroup.setBounds(10, y, 110, height);
    outputGroup.setBounds(bounds.getWidth() - 160, y, 150, height);
    feedbackGroup.setBounds(delayGroup.getRight() + 10, y, outputGroup.getX() - delayGroup.getRight() - 20, height);
    
    delayTimeKnob.setTopLeftPosition(20, 20);
    mixKnob.setTopLeftPosition(20, 20);
    gainKnob.setTopLeftPosition(mixKnob.getX(), mixKnob.getBottom()+10);
    feedbackKnob.setTopLeftPosition(20, 20);
    
    gainKnob.slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::lightgreen);
    
}


