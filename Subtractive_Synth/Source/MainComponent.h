/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once
#include "Maximilian.h"
#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent   : public AudioAppComponent
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;

private:
    //==============================================================================
    // Your private member variables go here...
    maxiOsc osc1, osc2, osc3, lfo1, lfo2, phasor1, phasor2, phasor3;
    maxiFilter f1, f2;
    
    Slider cutoff, res, lfoF1, lfoF2, lfoAmp1, lfoAmp2, speed;
    Label cutoffLabel, resLabel, freq1Label, freq2Label, amp1Label, amp2Label, speedLabel;
    
    int cutoffHz = 100;
    float resonance = 1.0;
    int freq1 = 0, freq2 = 0, amp1 = 1, amp2 = 1;
    
    int pitchSequence[8] = {100, 200, 250, 350, 400, 250, 100, 250};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
