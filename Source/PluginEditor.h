/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
class ClipperAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    ClipperAudioProcessorEditor (ClipperAudioProcessor&, juce::AudioProcessorValueTreeState&);
    ~ClipperAudioProcessorEditor() override;

	// GUI setup
	static const int N_SLIDERS_COUNT = 4;
	static const int SCALE = 70;
	static const int LABEL_OFFSET = 25;
	static const int SLIDER_WIDTH = 200;

    //==============================================================================
	void paint (juce::Graphics&) override;
    void resized() override;

	typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
	typedef juce::AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ClipperAudioProcessor& audioProcessor;

	juce::AudioProcessorValueTreeState& valueTreeState;

	juce::Label m_labels[N_SLIDERS_COUNT] = {};
	juce::Slider m_sliders[N_SLIDERS_COUNT] = {};
	std::unique_ptr<SliderAttachment> m_sliderAttachment[N_SLIDERS_COUNT] = {};

	juce::Label automationTLabel;
	juce::Label smoothingTypeLabel;
	juce::Label detectionTypeLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ClipperAudioProcessorEditor)
};