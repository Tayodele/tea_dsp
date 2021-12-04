#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "..\..\TeaCommon\conversions.h"
#include <string>

using namespace TTModules;

//==============================================================================
/**
*/
class TeaPannerAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                       private juce::Slider::Listener,
                                       private juce::Button::Listener,
                                       private juce::ComboBox::Listener,
                                       private juce::ChangeListener
{
public:
    TeaPannerAudioProcessorEditor (TeaPannerAudioProcessor&);
    ~TeaPannerAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void sliderValueChanged (juce::Slider* slider) override;
    void buttonClicked (juce::Button* button) override;
    void comboBoxChanged (juce::ComboBox* box) override;
    void changeListenerCallback (juce::ChangeBroadcaster *source) override;
    void setFrequency(bool noBPM = false);
    
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    TeaPannerAudioProcessor& audioProcessor;

    juce::Slider panFreq;
    juce::ComboBox steps;
    juce::TextButton freq_bpm;
    juce::Label the_bpm;

    juce::Slider inVol;
    juce::Slider outVol;

    //GUI flags
    bool fOrb;
    bpmstep stepVal;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TeaPannerAudioProcessorEditor)
};
