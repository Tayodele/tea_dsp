#pragma once

#include <JuceHeader.h>
#include "..\..\TeaLFO\TeaLFO.h"

//==============================================================================
/**
*/
using namespace  TTModules;

class TeaPannerAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    TeaPannerAudioProcessor();
    ~TeaPannerAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    // Editor Params ===============================================================
    juce::ChangeBroadcaster bpmcast;
    float freq;

    // DSP =========================================================================
   TeaLFO* tLFO0;
   TeaLFO* tLFO1;
   // In decibels
   float inVol;
   float outVol;
   // updated BPM
   double panbpm;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TeaPannerAudioProcessor)
};
