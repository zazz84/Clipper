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
	juce::Colour dark = juce::Colour::fromHSV(0.0f, 0.5f, 0.4f, 1.0f);
	juce::Colour light = juce::Colour::fromHSV(0.0f, 0.5f, 0.6f, 1.0f);

	getLookAndFeel().setColour(juce::Slider::thumbColourId, dark);
	getLookAndFeel().setColour(juce::Slider::rotarySliderFillColourId, juce::Colour::fromHSV(0.0f, 0.5f, 0.5f, 1.0f));
	getLookAndFeel().setColour(juce::Slider::rotarySliderOutlineColourId, light);

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

	// Buttons
	addAndMakeVisible(typaAButton);
	addAndMakeVisible(typaBButton);
	
	typaAButton.onClick = [this] { m_type = 0; };
	typaAButton.onClick = [this] { m_type = 1; };
	
	typaAButton.setRadioGroupId(TYPE_BUTTON_GROUP);
	typaBButton.setRadioGroupId(TYPE_BUTTON_GROUP);
	
	typaAButton.setClickingTogglesState(true);
	typaBButton.setClickingTogglesState(true);

	buttonAAttachment.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(valueTreeState, "ButtonA", typaAButton));
	buttonBAttachment.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(valueTreeState, "ButtonB", typaBButton));

	typaAButton.setColour(juce::TextButton::buttonColourId, light);
	typaBButton.setColour(juce::TextButton::buttonColourId, light);

	typaAButton.setColour(juce::TextButton::buttonOnColourId, dark);
	typaBButton.setColour(juce::TextButton::buttonOnColourId, dark);

	setSize((int)(SLIDER_WIDTH * 0.01f * SCALE * N_SLIDERS_COUNT), (int)((SLIDER_WIDTH + BOTTOM_MENU_HEIGHT) * 0.01f * SCALE));
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
	int height = SLIDER_WIDTH * 0.01f * SCALE;
	juce::Rectangle<int> rectangles[N_SLIDERS_COUNT];

	for (int i = 0; i < N_SLIDERS_COUNT; ++i)
	{
		rectangles[i].setSize(width, height);
		rectangles[i].setPosition(i * width, 0);
		m_sliders[i].setBounds(rectangles[i]);

		rectangles[i].removeFromBottom((int)(LABEL_OFFSET * 0.01f * SCALE));
		m_labels[i].setBounds(rectangles[i]);
	}

	// Buttons
	const int posY = height + (int)(BOTTOM_MENU_HEIGHT * 0.01f * SCALE * 0.25f);
	const int buttonHeight = (int)(BOTTOM_MENU_HEIGHT * 0.01f * SCALE * 0.5f);

	typaAButton.setBounds((int)(getWidth() * 0.5f - buttonHeight * 0.6f), posY, buttonHeight, buttonHeight);
	typaBButton.setBounds((int)(getWidth() * 0.5f + buttonHeight * 0.6f), posY, buttonHeight, buttonHeight);
}