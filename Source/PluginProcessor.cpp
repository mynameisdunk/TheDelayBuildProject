#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Parameters.h"
#include "Character.h"
#include "EarProtection.h"


//============================================================================================================

TheDelayAudioProcessor::TheDelayAudioProcessor() :
    AudioProcessor(
        BusesProperties()
            .withInput("Input", juce::AudioChannelSet::stereo(), true)
            .withOutput("Output", juce::AudioChannelSet::stereo(), true)
), // watch the comma!
params(apvts) // this is new
{
 //   lowCutFilter.setType(juce::dsp::StateVariableTPTFilterType::highpass);
 //   highCutFilter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    hissHighPass.setType(juce::dsp::StateVariableTPTFilterType::highpass);
    hissLowPass.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    outputHighCut.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    outputLowCut.setType(juce::dsp::StateVariableTPTFilterType::highpass);
    
}

TheDelayAudioProcessor::~TheDelayAudioProcessor()
{
}

//============================================================================================================
const juce::String TheDelayAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool TheDelayAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool TheDelayAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool TheDelayAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double TheDelayAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int TheDelayAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int TheDelayAudioProcessor::getCurrentProgram()
{
    return 0;
}

void TheDelayAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String TheDelayAudioProcessor::getProgramName (int index)
{
    return {};
}

void TheDelayAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void TheDelayAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    
    params.prepareToPlay(sampleRate);
    params.reset();
    
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = juce::uint32(samplesPerBlock);
    spec.numChannels = 2;
    
    delayLine.prepare(spec);
    
    double numSamples = Parameters::maxDelayTime / 1000.0 * sampleRate;
    int maxDelayInSamples = int(std::ceil(numSamples));
                                //casts result to an integer value
    delayLine.setMaximumDelayInSamples(maxDelayInSamples);
    delayLine.reset();
    
    feedbackL = 0.0f;
    feedbackR = 0.0f;
    
    copyOfSampleRate = float(sampleRate);
    //                 ^^Explicitly casts sampleRate to float
    
    DBG(maxDelayInSamples);
    
    /*
    lowCutFilter.prepare(spec);
    lowCutFilter.reset();
    highCutFilter.prepare(spec);
    highCutFilter.reset();
    */
    
    hissHighPass.prepare(spec);
    hissHighPass.reset();
    hissLowPass.prepare(spec);
    hissLowPass.reset();
    outputHighCut.prepare(spec);
    outputHighCut.reset();
    outputLowCut.prepare(spec);
    outputLowCut.reset();
    // the juce::dsp objects must always be prepared before they can be used
    
    highShelf.prepare(spec);
    highShelf.reset();
    lowShelf.prepare(spec);
    lowShelf.reset();
    
// LAST STATE VARIABLES FOR LESS PROCESSING  =============================================================
    //lastLowCut = -1.0f;
    //lastHighCut = -1.0f;
    lastLowShelf = 1.0f;
    lastHighShelf = 1.0f;
    
    fc.prepare(sampleRate);
    
    wowPhase = 0.0f;
    flutterPhase = 0.0f;
    
}

//============================================================================================================

void TheDelayAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TheDelayAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto mono = juce::AudioChannelSet::mono();
    const auto stereo = juce::AudioChannelSet::stereo();
    const auto mainIn = layouts.getMainInputChannelSet();
    const auto mainOut = layouts.getMainOutputChannelSet();
    
    if (mainIn == mono && mainOut == mono) { return true; }
    if (mainIn == mono && mainOut == stereo) { return true; }
    if (mainIn == stereo && mainOut == stereo) { return true; }
    
    return false;
    
}

#endif

void TheDelayAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, [[maybe_unused]] juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
// UPDATES
    params.update();
    if (params.lowShelf != lastLowShelf || params.highShelf != lastHighShelf) {
        updateShelfFilters();
        lastLowShelf = params.lowShelf;
        lastHighShelf = params.highShelf;
    }
    
    if(outputHighCut.getCutoffFrequency() != lastOutputHighCut){
        outputHighCut.setCutoffFrequency(lastOutputHighCut);
    }
    if(outputLowCut.getCutoffFrequency() != lastOutputLowCut){
        outputLowCut.setCutoffFrequency(lastOutputLowCut);
    }
    
    // delayLine.setDelay(48000.0f);
    float sampleRate = float(getSampleRate());
    
    auto mainInput = getBusBuffer(buffer, true, 0);
    auto mainInputChannels = mainInput.getNumChannels();
    auto isMainInputStereo = mainInputChannels > 1;
    const float* inputDataL = mainInput.getReadPointer(0);
    const float* inputDataR = mainInput.getReadPointer(isMainInputStereo ? 1 : 0);
    auto mainOutput = getBusBuffer(buffer, false, 0);
    auto mainOutputChannels = mainOutput.getNumChannels();
    auto isMainOutputStereo = mainOutputChannels > 1;
    float* outputDataL = mainOutput.getWritePointer(0);
    float* outputDataR = mainOutput.getWritePointer(isMainOutputStereo ? 1 : 0);
    
    if (isMainOutputStereo) // if statement to alter delay line push/ pop setting if used in full mono mode
    {
        for (int sample = 0; sample < buffer.getNumSamples(); sample ++)
        {
            params.smoothen();
            // updates parameter values
            
            // advance phases
            wowPhase += wowRate / sampleRate;
            flutterPhase += flutterRate / sampleRate;

            if (wowPhase >= 1.0f) wowPhase -= 1.0f;
            if (flutterPhase >= 1.0f) flutterPhase -= 1.0f;

            // generate modulation
            float wow = std::sin(juce::MathConstants<float>::twoPi * wowPhase); // CONTROL SLOW WARBLE
            float flutter = (std::sin(juce::MathConstants<float>::twoPi * flutterPhase) * params.wowFlutter); // CONTROL FAST WARBLE

            // scale by parameter
            float modulation = ((wow * wowDepth * params.wowFlutter) + (flutter * flutterDepth * params.wowFlutter));

            // apply to delay
            float delayInSamples = params.delayTime / 1000.0f * sampleRate;
            // delayLine.setDelay(delayInSamples); WITHOUT MODULATION
            baseDelayInSamples = delayInSamples;
            delayLine.setDelay(baseDelayInSamples + modulation);
            
            // ========================================================================================================================
// IMPROVING PERFORMANCE
            if (hissHighPass.getCutoffFrequency() != lastHissHighPass) {
                hissHighPass.setCutoffFrequency(lastHissHighPass);
            }
            if (hissLowPass.getCutoffFrequency() != lastHissLowPass){
                hissLowPass.setCutoffFrequency(lastHissLowPass);
            }
            
            
// SETTERS AND SCALING
            fc.setDirtAmount(params.dirt);
            
// INPUT SIGNAL
            float dryL = inputDataL[sample];
            float dryR = inputDataR[sample];
            dryL = fc.preampSim(dryL, params.inputLevel);
            dryR = fc.preampSim(dryR, params.inputLevel);
            
            float mono = (dryL + dryR) * 0.5f;
           
//DELAY LINE
            delayLine.pushSample(0, mono*params.panL + feedbackL);
            delayLine.pushSample(1, mono*params.panR + feedbackR);
            
            float wetR = delayLine.popSample(0);
            float wetL = delayLine.popSample(1);
            // ^ we have swapped the LR here as it creates a true ping pong delay
            
//LEFT CHANNEL
            //          TPT Filter (REMOVED)
            // float filteredL = lowCutFilter.processSample(0, wetL);
            // filteredL = highCutFilter.processSample(0, filteredL);
            
            //          IIR Filter
            // wetL = fc.tapeSim(wetL);
            feedbackL = wetL * params.feedback;
            
            feedbackL = lowShelf.processSample(feedbackL);
            feedbackL = highShelf.processSample(feedbackL);
            feedbackL = fc.tapeSim(feedbackL);
            
            
            
//added character
            float tapeHissL = fc.addTapeHiss(feedbackL, params.inputLevel);
            tapeHissL = hissHighPass.processSample(0, tapeHissL);
            tapeHissL = hissLowPass.processSample(0, tapeHissL);
            
            //float hiss201 = fc.makeRE201Hiss(fc.hissHPStateL, fc.hissLPStateL, sampleRate, feedbackL);
            
//RIGHT CHANNEL
            //          TPT Filter (REMOVED)
            // float filteredR = lowCutFilter.processSample(1, wetR);
            // filteredR = highCutFilter.processSample(1, filteredR);
            
            //          IIR Filter
            //wetR = fc.tapeSim(wetR);
            feedbackR = wetR * params.feedback;
            
            feedbackR = lowShelf.processSample(feedbackR);
            feedbackR = highShelf.processSample(feedbackR);
            feedbackR = fc.tapeSim(feedbackR);
            
            
            
//added character
            float tapeHissR = fc.addTapeHiss(feedbackR, params.inputLevel);
            tapeHissR = hissHighPass.processSample(1, tapeHissR);
            tapeHissR = hissLowPass.processSample(1, tapeHissR);
            
// SUMMING & OUTPUT
            feedbackL = feedbackL + tapeHissL;
            feedbackR = feedbackR + tapeHissR;
            feedbackL = outputHighCut.processSample(0, feedbackL);
            feedbackL = outputLowCut.processSample(0, feedbackL);
            feedbackR = outputHighCut.processSample(1, feedbackR);
            feedbackR = outputLowCut.processSample(1, feedbackR);
            feedbackL = fc.tapeLimit(feedbackL);
            feedbackR = fc.tapeLimit(feedbackR);
            
            float outputWetL = feedbackL;
            float outputWetR = feedbackR;
            // MUST ALWAYS BE SET TO THE LAST EDITED VERSION OF THE OUTPUT WET SIGNAL
            float mixL = dryL * (1.0f - params.mix) + outputWetL * params.mix;
            float mixR = dryR * (1.0f - params.mix) + outputWetR * params.mix;
                
            outputDataL[sample] = mixL * params.gain;
            outputDataR[sample] = mixR * params.gain;
        
        }
    }
    
    else // small block for mono I + O processing   MUST BE UPDATED ONCE STEREO PROCESSING IS FINALISED !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    {
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
        params.smoothen();
        float delayInSamples = params.delayTime / 1000.0f * sampleRate;
        delayLine.setDelay(delayInSamples);
        float dry = inputDataL[sample];
        delayLine.pushSample(0, dry + feedbackL);
        float wet = delayLine.popSample(0);
        feedbackL = wet * params.feedback;
        float mix = dry + wet * params.mix;
        outputDataL[sample] = mix * params.gain;
        }
    }

#if JUCE_DEBUG
    earProtection(buffer);
#endif

    
}

// delayLine.popSample(0, delayInSamples * 2.0f, false)

//===============================================================================================================
                                                // HELPER FUNCTIONS
//===============================================================================================================

void TheDelayAudioProcessor::updateShelfFilters()
{
    const float clampedLow  = juce::jlimit(-10.0f, 10.0f, params.lowShelf);
    const float clampedHigh = juce::jlimit(-10.0f, 10.0f, params.highShelf);
    
    float sampleRate = float(getSampleRate());

    auto lowCoef = juce::dsp::IIR::Coefficients<float>::makeLowShelf(
        sampleRate, 200.0f, 0.707f, juce::Decibels::decibelsToGain(clampedLow));

    auto highCoef = juce::dsp::IIR::Coefficients<float>::makeHighShelf(
        sampleRate, 6000.0f, 0.707f, juce::Decibels::decibelsToGain(clampedHigh));

    // Swap coefficients safely
    *lowShelf.coefficients = *lowCoef;
    *highShelf.coefficients = *highCoef;
}



bool TheDelayAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* TheDelayAudioProcessor::createEditor()
{
    return new TheDelayAudioProcessorEditor (*this);
}

//==========================================================================================================
void TheDelayAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    copyXmlToBinary(*apvts.copyState().createXml(),destData);
}
void TheDelayAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data,sizeInBytes));
    if (xml.get() != nullptr && xml->hasTagName(apvts.state.getType()))
    {
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
        
    }
}

 

//==========================================================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
    {
    return new TheDelayAudioProcessor();
}
