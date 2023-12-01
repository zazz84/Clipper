/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

const std::string ClipperAudioProcessor::paramsNames[] = { "Asymetry", "Threshold", "Mix", "Volume" };

//==============================================================================
ClipperAudioProcessor::ClipperAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
	asymetryParameter  = apvts.getRawParameterValue(paramsNames[0]);
	thresholdParameter = apvts.getRawParameterValue(paramsNames[1]);
	mixParameter       = apvts.getRawParameterValue(paramsNames[2]);
	volumeParameter    = apvts.getRawParameterValue(paramsNames[3]);
}

ClipperAudioProcessor::~ClipperAudioProcessor()
{
}

//==============================================================================
const juce::String ClipperAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ClipperAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ClipperAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ClipperAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ClipperAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ClipperAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ClipperAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ClipperAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ClipperAudioProcessor::getProgramName (int index)
{
    return {};
}

void ClipperAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ClipperAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{	
}

void ClipperAudioProcessor::releaseResources()
{
	
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ClipperAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void ClipperAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	// Get params
	const auto asymetry = asymetryParameter->load();
	const auto threshold = thresholdParameter->load();
	const auto mix = mixParameter->load();
	const auto volume = juce::Decibels::decibelsToGain(volumeParameter->load());

	// Mics constants
	const float thresholdGain = juce::Decibels::decibelsToGain(threshold);
	const float offset = asymetry * thresholdGain;
	const float thresholdGainPositive = thresholdGain + offset;
	const float thresholdGainNegative = -1.0f * thresholdGain + offset;
	const float mixInverse = 1.0f - mix;
	const int channels = getTotalNumOutputChannels();
	const int samples = buffer.getNumSamples();		

	for (int channel = 0; channel < channels; ++channel)
	{
		// Channel pointer
		auto* channelBuffer = buffer.getWritePointer(channel);

		for (int sample = 0; sample < samples; ++sample)
		{
			// Get input
			const float in = channelBuffer[sample];

			// Apply clipping
			const float inClipped = (in > 0) ? fminf(in, thresholdGainPositive) : fmaxf(in, thresholdGainNegative);

			// Apply volume, mix and send to output
			channelBuffer[sample] = volume * (mix * inClipped + mixInverse * in);
		}
	}
}

//==============================================================================
bool ClipperAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ClipperAudioProcessor::createEditor()
{
    return new ClipperAudioProcessorEditor (*this, apvts);
}

//==============================================================================
void ClipperAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{	
	auto state = apvts.copyState();
	std::unique_ptr<juce::XmlElement> xml(state.createXml());
	copyXmlToBinary(*xml, destData);
}

void ClipperAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
	std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

	if (xmlState.get() != nullptr)
		if (xmlState->hasTagName(apvts.state.getType()))
			apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessorValueTreeState::ParameterLayout ClipperAudioProcessor::createParameterLayout()
{
	APVTS::ParameterLayout layout;

	using namespace juce;

	layout.add(std::make_unique<juce::AudioParameterFloat>(paramsNames[0], paramsNames[0], NormalisableRange<float>( -1.0f,  1.0f, 0.05f, 1.0f),   0.0f));
	layout.add(std::make_unique<juce::AudioParameterFloat>(paramsNames[1], paramsNames[1], NormalisableRange<float>(-60.0f,  0.0f,  1.0f, 1.0f), -12.0f));
	layout.add(std::make_unique<juce::AudioParameterFloat>(paramsNames[2], paramsNames[2], NormalisableRange<float>(  0.0f,  1.0f, 0.05f, 1.0f),   1.0f));
	layout.add(std::make_unique<juce::AudioParameterFloat>(paramsNames[3], paramsNames[3], NormalisableRange<float>(-24.0f, 24.0f,  0.1f, 1.0f),   0.0f));

	return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ClipperAudioProcessor();
}
