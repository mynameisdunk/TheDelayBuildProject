#include "Character.h"
#include "Parameters.h"

void feedbackCharacter::prepare(double sampleRate)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = 512;
    spec.numChannels = 1;

    preEmphasis.prepare(spec);
    tapeLowPass.prepare(spec);

    // High-shelf pre-emphasis (~3 dB @ 4 kHz)
    preEmphasis.coefficients =
        juce::dsp::IIR::Coefficients<float>::makeHighShelf(
            sampleRate, 4000.0f, 0.707f,
            juce::Decibels::decibelsToGain(3.0f));

    // Tape bandwidth roll-off (~14 kHz)
    tapeLowPass.coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            sampleRate, 14000.0f);

    preEmphasis.reset();
    tapeLowPass.reset();
}


//============================================================================================================
                                    // ADDING CHARACTER TO THE DELAY FEEDBACK
//============================================================================================================
float feedbackCharacter::saturationCharacter(float x)
    {
        float y = std::tanh(x * 2.0f);
        
        return y;
        
    }

//-----------------------------------------------------------------------------------------------------------

float feedbackCharacter::addTapeHiss(float x, float preampSim)
    {
    // float y = ((preampSim / 4.0f) * (preampSim / 2.0f));
    
        float dynamicHiss = std::sqrt(baselineHiss * (preampSim + 1.0f)) * 0.01f; // this is now a much flatter response curve
        //    y             this flattens the curve                      ^ this drastically reduces the value of y
        float noise = juce::Random::getSystemRandom().nextFloat() * 2.0f - 1.0f;

        return x + noise * dynamicHiss;
    }

//-----------------------------------------------------------------------------------------------------------

float feedbackCharacter::noiseHighPass(float x, float& z, float cutoff, float sampleRate)
    {
        float RC = 1.0f / (cutoff * 2.0f * juce::MathConstants<float>::pi);
        float alpha = 1.0f / (1.0f + (sampleRate * RC));

        float y = alpha * (z + x - z);  // simple differentiator style HPF
        z = y;                          // store state
        return y;
    }

//-----------------------------------------------------------------------------------------------------------
// Second Optional Noise High Pass using TPT Filter



//-----------------------------------------------------------------------------------------------------------

float feedbackCharacter::noiseLowPass(float x, float& z, float cutoff, float sampleRate)
    {
        float RC = 1.0f / (cutoff * 2.0f * juce::MathConstants<float>::pi);
        float alpha = RC / (RC + 1.0f / sampleRate);

        z = alpha * (x - z);
        
        return z;
    }

//-----------------------------------------------------------------------------------------------------------

float feedbackCharacter::airBoost(float x)
    {
        return x * 1.2f + (x * x * x) * 0.8f;
    }

//-----------------------------------------------------------------------------------------------------------

float feedbackCharacter::makeRE201Hiss(float& hpState, float& lpState, float sampleRate, float hissAmount)
    {
        
        float n = juce::Random::getSystemRandom().nextFloat() * 2.0f - 1.0f;
        n = noiseHighPass(n, hpState, 5000.0f, sampleRate);
        n = airBoost(n);
        n = noiseLowPass(n, lpState, 13000.0f, sampleRate);
        n = airBoost(n);
        return n * hissAmount;
    }


//-----------------------------------------------------------------------------------------------------------

float feedbackCharacter::makeGlassyHiss(float& state, float sampleRate)
    {
        float noise = juce::Random::getSystemRandom().nextFloat() * 2.0f - 1.0f;
        float hpNoise = noiseHighPass(noise, state, 5000.0f, sampleRate);
        hpNoise *= 1.5f;
        glassyHiss = hpNoise * 1.2f + (hpNoise * hpNoise * hpNoise) * 0.5f;
        
        return glassyHiss * baselineHiss;
    }

//-----------------------------------------------------------------------------------------------------------

float feedbackCharacter::softLimit(float x, float threshold)
    {
        float s = x / threshold;
        float y = std::tanh(s);
        
        return y * threshold;
    }

//-----------------------------------------------------------------------------------------------------------

float feedbackCharacter::processFeedbackCharacter(float wetSignal,float& hissHPState,float& hissLPState)
    {
        
        float saturated = saturationCharacter(wetSignal);
        float spaceEchoHiss = makeRE201Hiss(hissHPState, hissLPState, copyOfSampleRate, baselineHiss);
        float combined = saturated + spaceEchoHiss;
        float limited = softLimit(combined, softThreshold);
        
        return limited;
    }

//-----------------------------------------------------------------------------------------------------------

float feedbackCharacter::preampSim(float x, float boost)
    {
        x *= boost;
        x = std::tanh(x * 1.2f);        // preamp drive
    
        static float lp = 0.0f;         // high shelf roll off
        lp += 0.1f* (x - lp);
        x = lp;
        
        return x;
    }
//-----------------------------------------------------------------------------------------------------------

void feedbackCharacter::setTapeDrive(float amount)
{
    // amount expected 0–1
    amount = juce::jlimit(0.0f, 1.0f, amount);

    // Tape drive rarely exceeds ~2.5 internally
    tapeDrive = 1.0f + amount * 1.5f;
}

//-----------------------------------------------------------------------------------------------------------

float feedbackCharacter::tapeSim(float x)
{
    // Envelope follower
        float level = std::abs(x);
        env = 0.995f * env + 0.005f * level;
    
    // 1. Pre-emphasis (real filter, actually exists)
    x = preEmphasis.processSample(x);
    
    // Flux limiting (tape ceiling)
        x = x / (1.0f + std::abs(x) * 1.2f);

    // 2. Soft compression (level-dependent)
    constexpr float compAmount = 0.8f;
    x *= 1.0f / (1.0f + std::abs(x) * compAmount);

    // 3. Asymmetrical tape saturation
    constexpr float bias = 0.12f;
    x = std::tanh((x + bias) * tapeDrive)
      - std::tanh(bias * tapeDrive);
    
    // Flux limiting (tape ceiling)
        x = x / (1.0f + std::abs(x) * 1.2f);

    // 4. Tape bandwidth roll-off
    x = tapeLowPass.processSample(x);

    return x;
}

//-----------------------------------------------------------------------------------------------------------

float feedbackCharacter::tapeLimit(float x)
{
    constexpr float maxFlux = 0.8f;
    return x / (1.0f + std::abs(x) / maxFlux);
}
// magnetic flux type limiting
