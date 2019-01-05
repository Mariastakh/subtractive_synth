#pragma once
#include "maximilian.h"
#include "../JuceLibraryCode/JuceHeader.h"


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
	maxiOsc osc1, lfo1, lfo2, phasor1;
	maxiSVF filter;
	maxiEnv ADSR;
	maxiDistortion limiter;

	Slider cutoff, res, lfoF1, lfoF2, lfoAmp1, lfoAmp2, speed;
	Label cutoffLabel, resLabel, freq1Label, freq2Label, amp1Label, amp2Label, speedLabel;

	double cutoffHz = 100;
	double resonance = 1.0;
	double freq1 = 0, freq2 = 0, amp1 = 1, amp2 = 1;
	double envelope;
	int currentCount;
	int pitchSequence[8] = { 100, 200, 250, 350, 400, 250, 100, 250 };
	
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
