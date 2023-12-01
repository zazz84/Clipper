/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ClipperAudioProcessorEditor::ClipperAudioProcessorEditor (ClipperAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), audioProcessor (p), valueTreeState(vts)
{
	getLookAndFeel().setColour(juce::Slider::thumbColourId, juce::Colour::fromHSV(0.0f, 0.5f, 0.4f, 1.0f));
	getLookAndFeel().setColour(juce::Slider::rotarySliderFillColourId, juce::Colour::fromHSV(0.0f, 0.5f, 0.5f, 1.0f));
	getLookAndFeel().setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour::fromHSV(0.0f, 0.5f, 0.6f, 1.0f));

	for (int i = 0; i < N_SLIDERS_COUNT; i++)
	{
		auto& label = m_labels[i];
		auto& slider = m_sliders[i];

		//Lable
		label.setText(ClipperAudioProcessor::paramsNames[i], juce::dontSendNotification);
		label.setFont(juce::Font(24.0f * 0.01f * SCALE, juce::Font::bold));
		label.setJustificationType(juce::Justification::centred);
		addAndMakeVisible(label);

		//Slider
		slider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
		slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
		addAndMakeVisible(slider);
		m_sliderAttachment[i].reset(new SliderAttachment(valueTreeState, ClipperAudioProcessor::paramsNames[i], slider));
	}

	setSize((int)(SLIDER_WIDTH * 0.01f * SCALE * N_SLIDERS_COUNT), (int)(SLIDER_WIDTH * 0.01f * SCALE));
}

ClipperAudioProcessorEditor::~ClipperAudioProcessorEditor()
{
}

void ClipperAudioProcessorEditor::paint (juce::Graphics& g)
{
	g.fillAll(juce::Colour::fromHSV(0.0f, 0.5f, 0.7f, 1.0f));
}

void ClipperAudioProcessorEditor::resized()
{
	// Sliders + Menus
	int width = getWidth() / N_SLIDERS_COUNT;
	int height = getHeight();
	juce::Rectangle<int> rectangles[N_SLIDERS_COUNT];

	for (int i = 0; i < N_SLIDERS_COUNT; ++i)
	{
		rectangles[i].setSize(width, height);
		rectangles[i].setPosition(i * width, 0);
		m_sliders[i].setBounds(rectangles[i]);

		rectangles[i].removeFromBottom((int)(LABEL_OFFSET * 0.01f * SCALE));
		m_labels[i].setBounds(rectangles[i]);
	}
}