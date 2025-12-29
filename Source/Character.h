#pragma once
#include <JuceHeader.h>

class feedbackCharacter
{
public:
    
    // prepareing a new filter
    void prepare(double sampleRate);
    float process(float x);
    void setCutoffFrequency(float frequency);
    
    
    float saturationCharacter(float x);
    float addTapeHiss(float x, float preampSim);
    float noiseHighPass(float x, float& z, float cutoff, float sampleRate);
    float noiseLowPass(float x, float& z, float cutoff, float sampleRate);
    float airBoost(float x);
    float makeRE201Hiss(float& hpState, float& lpState, float sampleRate, float hissAmount);
    float makeGlassyHiss(float& state, float sampleRate);
    float softLimit(float x, float threshold);
    // MASTER FUNCTION
    float processFeedbackCharacter(float wetSignal, float& hissHPState, float& hissLPState);
    
    void setDirtAmount(float amount) // adding a 'setter'
    {
        float norm = juce::jlimit(0.0f, 10.0f, amount)/ 10.0f;
        norm *= norm;
        saturationCharacterDrive = 0.7f * norm;
    //  pins the value of saturationCharacterDrive to MAX 0.7 (used here to smooth the response based on the tanh curve)
    }
    
    float preampSim(float x, float boost);
    float tapeSim(float x);
    void setTapeDrive(float amount);
    float tapeLimit(float x);
    
private:

    float copyOfSampleRate = 44100.0;
    
    float saturationCharacterDrive = 0.0f;
    float baselineHiss = 0.001f;
    float hissHPStateL = 0.0f;
    float hissHPStateR = 0.0f;
    float hissLPStateL = 0.0f;
    float hissLPStateR = 0.0f;
    float softThreshold = 0.98f;
    float glassyHiss = 0.0f;
    float tapeDrive = 1.0f;
    float env = 1.0f;
    
    
    juce::dsp::IIR::Filter<float> preEmphasis;
    juce::dsp::IIR::Filter<float> tapeLowPass;
    
    
};

