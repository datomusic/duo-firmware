#ifndef Synth_h
#define Synth_h

// #include <Wire.h>

/*
  Begin <Audio.h>
  We're replicating stuff from PJRC's <Audio.h> here, excluding the Audio libraries we don't need
*/
#if TEENSYDUINO < 120
#error "Teensyduino version 1.20 or later is required to compile the Audio library."
#endif
#ifdef __AVR__
#error "The Audio Library only works with Teensy 3.X.  Teensy 2.0 is unsupported."
#endif

#include <DMAChannel.h>
#if !defined(DMACHANNEL_HAS_BEGIN) || !defined(DMACHANNEL_HAS_BOOLEAN_CTOR)
#error "You need to update DMAChannel.h & DMAChannel.cpp"
#error "https://github.com/PaulStoffregen/cores/blob/master/teensy3/DMAChannel.h"
#error "https://github.com/PaulStoffregen/cores/blob/master/teensy3/DMAChannel.cpp"
#endif

// When changing multiple audio object settings that must update at
// the same time, these functions allow the audio library interrupt
// to be disabled.  For example, you may wish to begin playing a note
// in response to reading an analog sensor.  If you have "velocity"
// information, you might start the sample playing and also adjust
// the gain of a mixer channel.  Use AudioNoInterrupts() first, then
// make both changes to the 2 separate objects.  Then allow the audio
// library to update with AudioInterrupts().  Both changes will happen
// at the same time, because AudioNoInterrupts() prevents any updates
// while you make changes.
//
#define AudioNoInterrupts() (NVIC_DISABLE_IRQ(IRQ_SOFTWARE))
#define AudioInterrupts()   (NVIC_ENABLE_IRQ(IRQ_SOFTWARE))

// include all the library headers, so a sketch can use a single
// #include <Audio.h> to get the whole library
//
#include <effect_bitcrusher.h>
#include <effect_fade.h>
#include <effect_envelope.h>
#include <effect_multiply.h>
#include <filter_variable.h>
#include <input_adc.h>
#include <output_dac.h>
#include <mixer.h>
#include <synth_waveform.h>
#include <synth_dc.h>
#include <synth_whitenoise.h>
#include <synth_pinknoise.h>

// End <Audio.h>

// GUItool: begin automatically generated code
AudioSynthWaveform       waveform2;      //xy=82,289
AudioSynthWaveform       waveform1;      //xy=83,236
AudioSynthWaveformDc     dc1;            //xy=92,341
AudioEffectEnvelope      envelope2;      //xy=231,341
AudioMixer4              mixer1;         //xy=259,275
AudioFilterStateVariable filter1;        //xy=407,283
AudioEffectEnvelope      envelope1;      //xy=562,270
AudioSynthNoiseWhite     noise1;         //xy=707,313
AudioEffectBitcrusher    bitcrusher1;    //xy=721,269
AudioMixer4              mixer2;         //xy=868,289
AudioOutputAnalog        dac1;           //xy=996,289
AudioConnection          patchCord1(waveform2, 0, mixer1, 1);
AudioConnection          patchCord2(waveform1, 0, mixer1, 0);
AudioConnection          patchCord3(dc1, envelope2);
AudioConnection          patchCord4(envelope2, 0, filter1, 1);
AudioConnection          patchCord5(mixer1, 0, filter1, 0);
AudioConnection          patchCord6(filter1, 0, envelope1, 0);
AudioConnection          patchCord7(envelope1, bitcrusher1);
AudioConnection          patchCord8(noise1, 0, mixer2, 1);
AudioConnection          patchCord9(bitcrusher1, 0, mixer2, 0);
AudioConnection          patchCord10(mixer2, dac1);
// GUItool: end automatically generated code


void audio_init();

void audio_init() {

  // Oscillators
  waveform1.begin(0.3, 220, WAVEFORM_SAWTOOTH);
  waveform2.pulseWidth(0.5);
  waveform2.begin(0.2, 110, WAVEFORM_PULSE);
  
  // Mixer mixes the oscillators
  mixer1.gain(0, 0.5); // OSC1
  mixer1.gain(1, 0.5); // OSC2

  // Filter
  filter1.resonance(0.7); // range 0.7-5.0
  filter1.frequency(400);
  filter1.octaveControl(4);

  // Amp envelope
  envelope1.delay(0);
  envelope1.attack(2);
  envelope1.hold(0);
  envelope1.decay(0);
  envelope1.sustain(1.0);
  envelope1.release(400);

  // Filter envelope
  dc1.amplitude(1.0); // Filter env needs an input signal
  envelope2.delay(0);
  envelope2.attack(15);
  envelope2.hold(0);
  envelope2.decay(0);
  envelope2.sustain(1.0);
  envelope2.release(300);

  // Bitcrusher - set to audio samplerate and max bitrate
  bitcrusher1.bits(16);
  bitcrusher1.sampleRate(44100);

  noise1.amplitude(0.0);
  mixer2.gain(0, 1.0);
  mixer2.gain(1, 0.7);
}

#endif