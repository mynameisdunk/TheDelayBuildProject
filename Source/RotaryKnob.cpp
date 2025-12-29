#include <JuceHeader.h>
#include "RotaryKnob.h"
#include "PluginEditor.h"
#include "Parameters.h"
#include "PluginProcessor.h"
#include "LookAndFeel.h"

//==============================================================================
RotaryKnob::RotaryKnob(const juce::String& text,
                       juce::AudioProcessorValueTreeState& apvts,
                       const juce::ParameterID& parameterID,
                       bool drawFromMiddle,
                       bool showTextBox)
: attachment(apvts, parameterID.getParamID(), slider)

{
    slider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(showTextBox ? juce::Slider::TextBoxBelow : juce::Slider::NoTextBox, false, 70, 16);
    slider.setBounds(0, 0, 70, 86);
    addAndMakeVisible(slider);
    
    label.setText(text, juce::NotificationType::dontSendNotification);
    label.setJustificationType(juce::Justification::horizontallyCentred);
    label.setBorderSize(juce::BorderSize<int>{0, 0, 2, 0});
    label.attachToComponent(&slider, false);
    addAndMakeVisible(label);
    
    setLookAndFeel(RotaryKnobLookAndFeel::get());
    
    float pi = juce::MathConstants<float>::pi;
    slider.setRotaryParameters(1.3f * pi, 2.7f * pi, true);
    
    slider.getProperties().set("drawFromMiddle", drawFromMiddle);
    
    setSize( 70, 110);
}

RotaryKnob::~RotaryKnob()
{
    
}



void RotaryKnob::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    slider.setTopLeftPosition(0, 24);
}
