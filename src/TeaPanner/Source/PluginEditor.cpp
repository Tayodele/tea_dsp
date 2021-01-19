#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <math.h>

using namespace TTModules;

//==============================================================================
TeaPannerAudioProcessorEditor::TeaPannerAudioProcessorEditor (TeaPannerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);

    //set listener for BPM cast
    audioProcessor.bpmcast.addChangeListener (this);
    
    // these define the parameters of our Freq slider
    panFreq.setSliderStyle (juce::Slider::Rotary);
    panFreq.setRange (0.1, 10.0, 0.1);
    panFreq.setTextBoxStyle (juce::Slider::NoTextBox, false, 90, 0);
    panFreq.setPopupDisplayEnabled (true, false, this);
    panFreq.setTextValueSuffix (" Pan Frequency");
    panFreq.setValue(1.0);
    
    // these define the parameters of our Freq Button
    freq_bpm.setButtonText ("Freq/BPM");
    freq_bpm.setToggleState(false,0);
    freq_bpm.changeWidthToFitText(30);
    fOrb = freq_bpm.getToggleState();
    
    // these define the parameters of our Steps Box
    steps.addItem("1/64",8);
    steps.addItem("1/32",7);
    steps.addItem("1/16",6);
    steps.addItem("1/8",5);
    steps.addItem("1/4",4);
    steps.addItem("2/4",3);
    steps.addItem("4/4",2);
    steps.addItem("8/4",1);
    steps.addSeparator();
    steps.setSelectedId(3,0);
    stepVal = (TTModules::bpmstep) 2;

    //Add label so we know if the BPM was actually set.
    the_bpm.setText("BPM: " + std::to_string(audioProcessor.panbpm),(juce::NotificationType)0);
    the_bpm.setFont (15.0f);
    the_bpm.setColour (0,juce::Colours::white);
    the_bpm.setEditable(false);

    // these define the parameters of our In slider
    inVol.setSliderStyle (juce::Slider::Rotary);
    inVol.setRange (-30., 0., 0.1);
    inVol.setTextBoxStyle (juce::Slider::NoTextBox, false, 90, 0);
    inVol.setPopupDisplayEnabled (true, false, this);
    inVol.setTextValueSuffix (" Input Volume");
    inVol.setValue(-3.0);
    
    // these define the parameters of our Out slider
    outVol.setSliderStyle (juce::Slider::Rotary);
    outVol.setRange (-30., 0., 0.1);
    outVol.setTextBoxStyle (juce::Slider::NoTextBox, false, 90, 0);
    outVol.setPopupDisplayEnabled (true, false, this);
    outVol.setTextValueSuffix (" Output Volume");
    outVol.setValue(0.0);
    
    
    addAndMakeVisible(&panFreq);
    addAndMakeVisible(&inVol);
    addAndMakeVisible(&outVol);
    addAndMakeVisible(&freq_bpm);
    addAndMakeVisible(&steps);
    addAndMakeVisible(&the_bpm);
    steps.setVisible(false);
    panFreq.addListener(this);
    inVol.addListener(this);
    outVol.addListener(this);
    freq_bpm.addListener(this);
    steps.addListener(this);

}

TeaPannerAudioProcessorEditor::~TeaPannerAudioProcessorEditor()
{
}

//==============================================================================
void TeaPannerAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("TeaPanner", 0, 0, getWidth(), 30, juce::Justification::centred, 1);

}

void TeaPannerAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto bigKnobW = 100;
    auto bigKnobH = 100;
    panFreq.setBounds(getWidth()/2 - bigKnobW/2,getHeight()/2,bigKnobW,bigKnobH);
    steps.setBounds(getWidth()/2 - (bigKnobW*0.7)/2,getHeight()/2,bigKnobW*0.7,bigKnobH/2.);
    the_bpm.setBounds(getWidth()/2 - (bigKnobW*0.7)/2,(getHeight()/2)-60,bigKnobW*0.7,bigKnobH/2.);
    freq_bpm.setBounds(getWidth()/2 - bigKnobW/2,getHeight()/6,bigKnobW,bigKnobH);
    inVol.setBounds(getWidth()/5 - bigKnobW/2,getHeight()/2,bigKnobW,bigKnobH);
    outVol.setBounds(4*getWidth()/5 - bigKnobW/2,getHeight()/2,bigKnobW,bigKnobH);
    
}

void TeaPannerAudioProcessorEditor::sliderValueChanged (juce::Slider* slider)
{
    audioProcessor.inVol = inVol.getValue();
    audioProcessor.outVol = outVol.getValue();
    setFrequency(true);
}

void TeaPannerAudioProcessorEditor::buttonClicked (juce::Button* button) 
{
    freq_bpm.setToggleState(!fOrb,0);
    fOrb = freq_bpm.getToggleState();
    if(fOrb) {
        panFreq.setVisible (false);
        steps.setVisible (true);
    } else {
        panFreq.setVisible (true);
        steps.setVisible (false);
    }
    setFrequency();
}

void TeaPannerAudioProcessorEditor::comboBoxChanged (juce::ComboBox* box) 
{
    double stepbpm = audioProcessor.panbpm;
    the_bpm.setText("BPM: " + std::to_string(audioProcessor.panbpm),(juce::NotificationType)0);
    stepVal = (TTModules::bpmstep) (steps.getSelectedId()-1);
    setFrequency();
}

void TeaPannerAudioProcessorEditor::changeListenerCallback (juce::ChangeBroadcaster *source) {
    the_bpm.setText("BPM: " + std::to_string(audioProcessor.panbpm),(juce::NotificationType)0);
    setFrequency();
}

void TeaPannerAudioProcessorEditor::setFrequency(bool noBPM) {
    if(fOrb) {
        if(!noBPM) {
            audioProcessor.freq = TTModules::bpm2Freq((float) audioProcessor.panbpm,(TTModules::bpmstep)stepVal);
            audioProcessor.tLFO0->setFreq(audioProcessor.freq);
            audioProcessor.tLFO1->setFreq(audioProcessor.freq);
        }
    } else if(!fOrb) {
        if(abs(panFreq.getValue()) != audioProcessor.freq) {
            audioProcessor.freq = abs(panFreq.getValue());
            audioProcessor.tLFO0->setFreq(audioProcessor.freq);
            audioProcessor.tLFO1->setFreq(audioProcessor.freq);
        }
    }
}