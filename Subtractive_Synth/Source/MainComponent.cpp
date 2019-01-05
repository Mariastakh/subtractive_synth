#include "maximilian.h"
#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
	cutoff.setSliderStyle(Slider::Rotary);
	cutoff.setRange(20, 20000);
	cutoff.setTextBoxStyle(Slider::TextBoxBelow, false, 100, 20);
	cutoff.setColour(Slider::textBoxTextColourId, Colours::black);
	cutoff.onValueChange =  [this] { cutoffHz = cutoff.getValue(); };
	cutoff.setValue(cutoffHz);
	addAndMakeVisible(cutoff);

	cutoffLabel.setText("Cutoff", dontSendNotification);
	cutoffLabel.setColour(Label::textColourId, Colours::black);
	addAndMakeVisible(cutoffLabel);

	res.setSliderStyle(Slider::Rotary);
	res.setRange(1, 10);
	res.setTextBoxStyle(Slider::TextBoxBelow, false, 100, 20);
	res.setColour(Slider::textBoxTextColourId, Colours::black);
	res.onValueChange = [this] { resonance = res.getValue(); };
	res.setValue(resonance);
	addAndMakeVisible(res);

	resLabel.setText("Resonance", dontSendNotification);
	resLabel.setColour(Label::textColourId, Colours::black);
	addAndMakeVisible(resLabel);

	lfoF1.setSliderStyle(Slider::Rotary);
	lfoF1.setRange(0, 4);
	lfoF1.setTextBoxStyle(Slider::TextBoxBelow, false, 100, 20);
	lfoF1.setColour(Slider::textBoxTextColourId, Colours::black);
	lfoF1.onValueChange = [this] { freq1 = lfoF1.getValue(); };
	lfoF1.setValue(freq1);
	addAndMakeVisible(lfoF1);

	freq1Label.setText("Cutoff F Modulation", dontSendNotification);
	freq1Label.setColour(Label::textColourId, Colours::black);
	addAndMakeVisible(freq1Label);

	lfoF2.setSliderStyle(Slider::Rotary);
	lfoF2.setRange(0, 4);
	lfoF2.setTextBoxStyle(Slider::TextBoxBelow, false, 100, 20);
	lfoF2.setColour(Slider::textBoxTextColourId, Colours::black);
	lfoF2.onValueChange = [this] { freq2 = lfoF2.getValue(); };
	lfoF2.setValue(freq2);
	addAndMakeVisible(lfoF2);

	freq2Label.setText("Resonance F Modulation", dontSendNotification);
	freq2Label.setColour(Label::textColourId, Colours::black);
	addAndMakeVisible(freq2Label);

	lfoAmp1.setSliderStyle(Slider::Rotary);
	lfoAmp1.setRange(0, 19980);
	lfoAmp1.setTextBoxStyle(Slider::TextBoxBelow, false, 100, 20);
	lfoAmp1.setColour(Slider::textBoxTextColourId, Colours::black);
	lfoAmp1.onValueChange = [this] { amp1 = lfoAmp1.getValue(); };
	lfoAmp1.setValue(amp1);
	addAndMakeVisible(lfoAmp1);

	amp1Label.setText("Cutoff Mod Index", dontSendNotification);
	amp1Label.setColour(Label::textColourId, Colours::black);
	addAndMakeVisible(amp1Label);

	lfoAmp2.setSliderStyle(Slider::Rotary);
	lfoAmp2.setRange(0, 9);
	lfoAmp2.setTextBoxStyle(Slider::TextBoxBelow, false, 100, 20);
	lfoAmp2.setColour(Slider::textBoxTextColourId, Colours::black);
	lfoAmp2.onValueChange = [this] { amp2 = lfoAmp2.getValue(); };
	lfoAmp2.setValue(amp2);
	addAndMakeVisible(lfoAmp2);

	amp2Label.setText("Res Mod Index", dontSendNotification);
	amp2Label.setColour(Label::textColourId, Colours::black);
	addAndMakeVisible(amp2Label);

  
    ADSR.setAttack(20);
    ADSR.setDecay(15);
    ADSR.setSustain(0.75);
    ADSR.setRelease(40);

	// Make sure you set the size of the component after
	// you add any child components.
	setSize(800, 600);

	// specify the number of input and output channels that we want to open
	setAudioChannels(2, 2);
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()
}

void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
	auto* leftBuffer = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
	auto* rightBuffer = bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample);

	for (int sample = 0; sample < bufferToFill.numSamples; sample++)
	{
		// Counter, increments from 0 - 7, so it can iterate through all 8 pitchvalues in the pitchSequence array:
		currentCount = phasor1.phasor(1, 0, 7);
	
		// Store the current square wave value, with the pitchSequence-defined frequency:
		double sine1 = osc1.square(pitchSequence[currentCount]);

		// --- Filter parameters ---

		// Two lfos, one to control cutoff, and one to control the resonance. 
		// Offset the amplitude so that the modulation never enters negative
		// ranges:
		double lfo_1 = ((lfo1.triangle(freq1) + 1.0) / 2.0) * amp1;
		double lfo_2 = ((lfo2.triangle(freq2) + 1.0) / 2.0) * amp2;

		// Restrict the maximum resonance and cutoff values.
		// Probably not the best, but straightforward and foolproof
		// way of avoiding ranges that are too high, or going
		// over the Nyquist frequency:
		double modulatedCutoff = cutoffHz + lfo_1;
		double modulatedRes = resonance + lfo_2;
		if (modulatedCutoff > 20000) modulatedCutoff = 20000;
		if (modulatedRes > 10.0) modulatedRes = 10.0;

		// Update the filter with the new settings:
		filter.setCutoff(modulatedCutoff);
		filter.setResonance(modulatedRes);

		// Set the filter to low-pass and pass the signal in
		double fOut = filter.play(sine1, 1.0, 0.0, 0.0, 0.0); 
		// Apply the atan function to the output for a gentle distortion
		double out = limiter.atanDist(fOut, 1.0);
		// output
		leftBuffer[sample] = out;
		rightBuffer[sample] = rightBuffer[sample];
	}
}

void MainComponent::releaseResources()
{
  
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
	g.fillAll(Colours::whitesmoke);
	g.setColour(Colours::darkseagreen);
	g.fillRect(20, 15, 130, 120);
	g.fillRect(160, 15, 130, 120);

	g.fillRect(20, 200, 130, 120);
	g.fillRect(160, 200, 130, 120);

	g.fillRect(20, 395, 130, 120);
	g.fillRect(160, 395, 130, 120);

}

void MainComponent::resized()
{
	// Cutoff:
	cutoff.setBounds(10, 10, 150, 150);
	lfoF1.setBounds(10, 200, 150, 150);
	lfoAmp1.setBounds(10, 390, 150, 150);

	// Labels:
	cutoffLabel.setBounds(50, 160, 50, 50);
	freq1Label.setBounds(40, 350, 100, 50);
	amp1Label.setBounds(40, 540, 100, 50);

	// Resonance:
	res.setBounds(150, 10, 150, 150);
	lfoF2.setBounds(150, 200, 150, 150);
	lfoAmp2.setBounds(150, 390, 150, 150);
	// Labels:
	resLabel.setBounds(190, 160, 100, 50);
	freq2Label.setBounds(190, 350, 100, 50);
	amp2Label.setBounds(170, 540, 100, 50);
}
