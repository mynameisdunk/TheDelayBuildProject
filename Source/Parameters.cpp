#include "Parameters.h"
#include "DSP.h"

template<typename T>
// replace T with whatever type I give you when calling the function
static void castParameter(juce::AudioProcessorValueTreeState& apvts,
const juce::ParameterID& id, T& destination)
{
destination = dynamic_cast<T>(apvts.getParameter(id.getParamID())); jassert(destination);
}
        // ^^ castParameter function is used to more easily 'dynamicCast' variables

//===============================================================================================================

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

//===============================================================================================================

static float millisecondsFromString(const juce::String& text)
{
    float value = text.getFloatValue();
    if (!text.endsWithIgnoreCase("ms"))
        {
            if (text.endsWithIgnoreCase("s") || value < Parameters::minDelayTime)
            {
                return value * 1000.0f;
            }
        }
        return value;
}

//^^ this function is used to format the users input into a useable output value
// eg if 1.4s is entered into the delayTime this will ensure it becomes 1.4s and is not read as 1.4ms which would then be snapped to the minDelayTime
// '|| value < Parameters::minDelayTime' is saying OR if the value is less than the smallest allowed value, assume the user inteded for the time to be in seconds

//===============================================================================================================

static juce::String stringFromDecibels(float value, int)
{
    return juce::String(value, 1) + "dB";
}

//===============================================================================================================

static:: juce::String stringFromPercent(float value, int)
{
    return juce::String(int(value)) + "%";
}

//===============================================================================================================

static juce::String stringFromHz(float value, int)
{
    if (value < 1000.0f)
    {
        return juce::String(int(value)) + " Hz";
    }
    else if (value < 10000.0f)
    {
        return juce::String(value / 1000.0f, 2) + " k";
    }
    else
    {
        return juce::String(value / 1000.0f, 1) + " kHz";
    }
}

//===============================================================================================================

static float hzFromString(const juce::String& str)
{
    float value = str.getFloatValue(); if (value < 20.0f)
    {
        return value * 1000.0f;
    }
        return value;
}

//===============================================================================================================

Parameters::Parameters(juce::AudioProcessorValueTreeState& apvts)
{
    castParameter(apvts, gainParamID, gainParam);
    castParameter(apvts, delayTimeParamID, delayTimeParam);
    castParameter(apvts, mixParamID, mixParam);
    castParameter(apvts, feedbackParamID, feedbackParam);
    castParameter(apvts, stereoParamID, stereoParam);
    castParameter(apvts, resonanceParamID, resonanceParam);
    castParameter(apvts, dirtParamID, dirtParam);
    castParameter(apvts, inputLevelParamID, inputLevelParam);
    castParameter(apvts, lowCutParamID, lowCutParam);
    castParameter(apvts, highCutParamID, highCutParam);
    castParameter(apvts, lowShelfParamID, lowShelfParam);
    castParameter(apvts, highShelfParamID, highShelfParam);
    castParameter(apvts, wowFlutterParamID, wowFlutterParam);
    
    //          look here, find this, put it here
}

//===============================================================================================================

juce::AudioProcessorValueTreeState::ParameterLayout

Parameters::createParameterLayout()
{
juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add
    (std::make_unique<juce::AudioParameterFloat>
     (gainParamID, "Output Gain", juce::NormalisableRange<float> {-12.0f, 12.0f, 0.001f}, -6.0f, juce::AudioParameterFloatAttributes().withStringFromValueFunction(stringFromDecibels)));
    layout.add
    (std::make_unique<juce::AudioParameterFloat>
     (delayTimeParamID, "Delay Time", juce::NormalisableRange<float> {minDelayTime, maxDelayTime, 0.001f, 0.25f}, 100.0f,
      juce::AudioParameterFloatAttributes()
      .withStringFromValueFunction(stringFromMilliseconds)
      .withValueFromStringFunction(millisecondsFromString)));
    layout.add
    (std::make_unique<juce::AudioParameterFloat>
     (mixParamID, "Mix", juce::NormalisableRange<float>{0.0f, 100.0f, 0.01f}, 50.0f, juce::AudioParameterFloatAttributes().withStringFromValueFunction(stringFromPercent)));
    layout.add
    (std::make_unique<juce::AudioParameterFloat>
    (feedbackParamID, "Feedback", juce::NormalisableRange<float> {-100.0f, 100.0f, 1.0f}, 0.0f, juce::AudioParameterFloatAttributes().withStringFromValueFunction(stringFromPercent)));
    layout.add
    (std::make_unique<juce::AudioParameterFloat>
     (stereoParamID, "Stereo", juce::NormalisableRange<float>{-100.0f, 100.0f, 0.1f}, 0.0f));
    
    layout.add
    (std::make_unique<juce::AudioParameterFloat>(lowCutParamID, "Low Cut", juce::NormalisableRange<float>{20.0f, 600.0f, 1.0f, 0.3f}, 20.0f, juce::AudioParameterFloatAttributes()
                        .withStringFromValueFunction(stringFromHz)
                        .withValueFromStringFunction(hzFromString)));
    layout.add
    (std::make_unique<juce::AudioParameterFloat>(highCutParamID, "High Cut", juce::NormalisableRange<float>{600.0f, 20000.0f, 1.0f, 0.3f}, 20000.0f,
    juce::AudioParameterFloatAttributes()
                        .withStringFromValueFunction(stringFromHz)
                        .withValueFromStringFunction(hzFromString)));
    layout.add
    (std::make_unique<juce::AudioParameterFloat>(resonanceParamID, "Resonance", juce::NormalisableRange<float> {0.1f, 10.0f, 0.1f}, 1.0));
    layout.add
    (std::make_unique<juce::AudioParameterFloat>(dirtParamID, "Dirt", juce::NormalisableRange<float> {0.0f, 10.0f, 0.1f}, 0.0f));
    layout.add
    (std::make_unique<juce::AudioParameterFloat>(inputLevelParamID, "Input Level", juce::NormalisableRange<float> {1.0f, 15.0f, 0.1f}, 1.5f));
    layout.add
    (std::make_unique<juce::AudioParameterFloat>(lowShelfParamID, "Lows", juce::NormalisableRange<float> {-10.0f, 10.0f, 1.0f}, 0.1f));
    layout.add
    (std::make_unique<juce::AudioParameterFloat>(highShelfParamID, "Highs", juce::NormalisableRange<float> {-10.0f, 10.0f, 1.0f}, 0.1f));
    layout.add
    (std::make_unique<juce::AudioParameterFloat>(wowFlutterParamID, "Machine Age", juce::NormalisableRange<float> {0.0f, 10.0f, 0.1f}, 3.5f));
 
return layout;
// return is a keyword that tells the compiler that at the end of this code that the variable first declared called 'layout' now has a new value defined by the code above

// Incredibly simply put
// This function builds a small “list” (layout) of plugin parameters by creating a gain parameter with a specific ID ("Output Gain"), range {-12 - 12}, and display format (1 decimal place, dB suffix). It returns that list so JUCE knows what parameters your plugin has.
}

//===============================================================================================================

void Parameters::update() noexcept {
    gainSmoother.setTargetValue(juce::Decibels::decibelsToGain(gainParam->get()));
    // You read a line like this from the inside-out: First it does gainParam->get(), then it does decibelsToGain, and finally it does setTargetValue.
    targetDelayTime = delayTimeParam->get();
    if (delayTime == 0.0f){
        delayTime = targetDelayTime;
    }
    
    mixSmoother.setTargetValue(mixParam->get() * 0.01f);
    feedbackSmoother.setTargetValue(feedbackParam->get() * 0.01f);
    stereoSmoother.setTargetValue(stereoParam->get() * 0.01f);
    // reads the stereo knobs current position and tells the smoother its new target value, also scales to -1 to 1
    
    highCutSmoother.setTargetValue(highCutParam->get());
    lowCutSmoother.setTargetValue(lowCutParam->get());
    
    dirtSmoother.setTargetValue(dirtParam->get());
    
    resonanceSmoother.setTargetValue(resonanceParam->get());
    
    inputLevelSmoother.setTargetValue(inputLevelParam->get());
    
    // lowShelfGain = lowShelfGainParam->get() * 0.1f;
    // highShelfGain = highShelfGainParam->get() * 0.1f;
    
    lowShelfSmoother.setTargetValue(lowShelfParam->get());
    highShelfSmoother.setTargetValue(highShelfParam->get());
    
    wowFlutterSmoother.setTargetValue(wowFlutterParam->get());
    
   // highCutFilter.setResonance(resonanceParam->get());

}

//===============================================================================================================

void Parameters::prepareToPlay(double sampleRate) noexcept
{
    double duration = 0.02;
    gainSmoother.reset(sampleRate,duration);
    
    coeff = 1.0f - std::exp(-1.0f/(0.3f * float(sampleRate)));
                                // ^^^ this value determines how quickly the delay value is smoothed. 0.2f is similar to an analogue capacitor. Larger values are slower smoothing times.
    mixSmoother.reset(sampleRate, duration);
    feedbackSmoother.reset(sampleRate, duration);
    stereoSmoother.reset(sampleRate, duration);
    lowCutSmoother.reset(sampleRate, duration);
    highCutSmoother.reset(sampleRate, duration);
    resonanceSmoother.reset(sampleRate, duration);
    dirtSmoother.reset(sampleRate, duration);
    lowShelfSmoother.reset(sampleRate, duration);
    highShelfSmoother.reset(sampleRate, duration);
    wowFlutterSmoother.reset(sampleRate, duration);
    
    
    
}

//===============================================================================================================

void Parameters::reset() noexcept
{
    gain = 0.0f;
    gainSmoother.setCurrentAndTargetValue(juce::Decibels::decibelsToGain(gainParam->get()));
    
    delayTime = 0.0f;
    
    mix = 1.0f;
    mixSmoother.setCurrentAndTargetValue(mixParam->get()*0.01f);
    
    feedback = 0.0f;
    feedbackSmoother.setCurrentAndTargetValue(feedbackParam->get()*0.01f);
    
    panL = 0.0f;
    panR = 1.0f;
    
    stereo = 0.0f;
    stereoSmoother.setCurrentAndTargetValue(stereoParam->get()*0.1f);
    // this line is getting the current value of the Stereo knob and scaling it to -1 to 1 from -100 to 100
    
    highCutSmoother.setCurrentAndTargetValue(highCutParam->get());
    lowCutSmoother.setCurrentAndTargetValue(lowCutParam->get());
    
    resonanceSmoother.setCurrentAndTargetValue(resonanceParam->get());
    
    dirtSmoother.setCurrentAndTargetValue(dirtParam->get());
    
    inputLevelSmoother.setCurrentAndTargetValue(inputLevelParam->get());
    
    lowShelfSmoother.setCurrentAndTargetValue(lowShelfParam->get());
    highShelfSmoother.setCurrentAndTargetValue(highShelfParam->get());
    
    wowFlutterSmoother.setCurrentAndTargetValue(wowFlutterParam->get());
}

//===============================================================================================================

void Parameters::smoothen() noexcept
{
    gain = gainSmoother.getNextValue();

    delayTime += (targetDelayTime -delayTime) * coeff;
    
    mix = mixSmoother.getNextValue();
    
    feedback = feedbackSmoother.getNextValue();
    
    panningEqualPower(stereoSmoother.getNextValue(), panL, panR);
    //                get the next step in the ramp, update this, and this
    
    lowCut = lowCutSmoother.getNextValue();
    highCut = highCutSmoother.getNextValue();
    
    resonance = resonanceSmoother.getNextValue();
    
    dirt = dirtSmoother.getNextValue();
    
    inputLevel = inputLevelSmoother.getNextValue();
    
    lowShelf = lowShelfSmoother.getNextValue();
    highShelf = highShelfSmoother.getNextValue();
    
    wowFlutter = wowFlutterSmoother.getNextValue();
}


