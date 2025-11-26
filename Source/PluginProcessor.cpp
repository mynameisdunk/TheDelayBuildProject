/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Parameters.h"

//==============================================================================

TheDelayAudioProcessor::TheDelayAudioProcessor() :
    AudioProcessor(
BusesProperties()
.withInput("Input", juce::AudioChannelSet::stereo(), true) .withOutput("Output", juce::AudioChannelSet::stereo(), true)
), // watch the comma!
params(apvts) // this is new
{// do nothing
 //   castParameter(apvts, delayTimeParamID, delayTimeParam);
}

TheDelayAudioProcessor::~TheDelayAudioProcessor()
{
}

//==============================================================================
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
    
    DBG(maxDelayInSamples);
}

void TheDelayAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TheDelayAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}
#endif

void TheDelayAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, [[maybe_unused]] juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    params.update();
    
    // delayLine.setDelay(48000.0f);
    float sampleRate = float(getSampleRate());
    
    float* channelDataL = buffer.getWritePointer(0);
    float* channelDataR = buffer.getWritePointer(1);
    
    for (int sample = 0; sample < buffer.getNumSamples(); sample ++)
    {
        params.smoothen();
        
        float delayInSamples = params.delayTime / 1000.0f * sampleRate;
        delayLine.setDelay(delayInSamples);
        
        float dryL = channelDataL[sample];
        float dryR = channelDataR[sample];
        
        delayLine.pushSample(0,dryL);
        delayLine.pushSample(1,dryR);
        
        float wetL = delayLine.popSample(0);
        float wetR = delayLine.popSample(1);
        
        float mixL = dryL * (1.0f - params.mix) + wetL * params.mix;
        float mixR = dryR * (1.0f - params.mix) + wetR * params.mix;
        
        channelDataL[sample] = mixL * params.gain;
        channelDataR[sample] = mixR * params.gain;
        
    }
}

//==============================================================================
bool TheDelayAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* TheDelayAudioProcessor::createEditor()
{
    return new TheDelayAudioProcessorEditor (*this);
}

//==============================================================================
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

 

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
    {
    return new TheDelayAudioProcessor();
}
