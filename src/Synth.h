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
#include <analyze_peak.h>
#include <effect_bitcrusher.h>
#include <effect_delay.h>
#include <effect_fade.h>
#include "effect_envelope2.h"
#include <effect_multiply.h>
#include <filter_variable.h>
#include <input_adc.h>
#include <output_dac.h>
#include <mixer.h>
#include <synth_waveform.h>
#include <synth_dc.h>
#include <synth_whitenoise.h>
#include <synth_pinknoise.h>
#include <synth_simple_drum.h>

// GUItool: begin automatically generated code
AudioSynthWaveform       waveform2;      //xy=78.10000610351562,97
AudioSynthWaveform       waveform1;      //xy=79.10000610351562,44
AudioSynthWaveformDc     dc1;            //xy=88.10000610351562,149
AudioEffectEnvelope2      envelope2;      //xy=227.10000610351562,149
AudioMixer4              mixer1;         //xy=255.10000610351562,83
AudioFilterStateVariable filter1;        //xy=403.1000061035156,91
AudioSynthNoiseWhite     noise1;         //xy=595.833251953125,244.40475463867188
AudioEffectEnvelope2      envelope1;      //xy=560.1000061035156,81
AudioEffectEnvelope2      envelope3;      //xy=731.547611781529,244.40475463867185
AudioAnalyzePeak         peak1;          //xy=705.1000061035156,37
AudioEffectDelay         delay1;         //xy=712.0999755859375,174.10000610351562
AudioSynthSimpleDrum     drum1;          //xy=800.1190403529575,180.11904035295757
AudioEffectBitcrusher    bitcrusher1;    //xy=718.1000061035156,81
AudioMixer4              delayMixer;         //xy=728.0999755859375,279.1000061035156
AudioMixer4              mixer2;         //xy=861.1000061035156,100
AudioAnalyzePeak         peak2;          //xy=987.1000061035156,151
AudioOutputAnalog        dac1;           //xy=988.1000061035156,100
AudioConnection          patchCord1(waveform2, 0, mixer1, 1);
AudioConnection          patchCord2(waveform1, 0, mixer1, 0);
AudioConnection          patchCord3(dc1, envelope2);
AudioConnection          patchCord4(envelope2, 0, filter1, 1);
AudioConnection          patchCord5(mixer1, 0, filter1, 0);
AudioConnection          patchCord6(filter1, 0, envelope1, 0);
AudioConnection          patchCord7(envelope1, peak1);
AudioConnection          patchCord8(envelope1, bitcrusher1);
AudioConnection          patchCord9(delay1, 0, mixer2, 1);
AudioConnection          patchCord10(delay1, 1, delayMixer, 1);
AudioConnection          patchCord11(bitcrusher1, 0, mixer2, 0);
AudioConnection          patchCord12(bitcrusher1, 0, delayMixer, 0);
AudioConnection          patchCord13(delayMixer, delay1);
AudioConnection          patchCord14(mixer2, dac1);
AudioConnection          patchCord15(mixer2, peak2);
AudioConnection          patchCord16(drum1, 0, mixer2, 2);
AudioConnection          patchCord17(noise1, envelope3);
AudioConnection          patchCord18(envelope3, 0, mixer2, 3);
// GUItool: end automatically generated code

#define MAIN_GAIN 0.9
#define DELAY_GAIN 0.6
#define KICK_GAIN 0.8
#define HAT_GAIN 0.3

void audio_init();
void audio_volume(uint8_t volume);

void audio_init() {
  AudioMemory(200); // 260 bytes per block, 2.9ms per block

  // Oscillators
  waveform1.begin(0.4, 220, WAVEFORM_SAWTOOTH);
  waveform2.pulseWidth(0.5);
  waveform2.begin(0.4, 110, WAVEFORM_PULSE);
  
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

  delay1.delay(0, 100); // Delay time
  delay1.delay(1, 440);
  delayMixer.gain(0, 0.0); // Delay input
  delayMixer.gain(1, 0.4); // Delay feedback

  mixer2.gain(0, MAIN_GAIN);
  mixer2.gain(1, DELAY_GAIN); // Delay output
  mixer2.gain(2, KICK_GAIN); // Kick output
  mixer2.gain(3, HAT_GAIN); // Hat output
  
  #ifdef SUPER_LOUD_MODE
    dac1.analogReference(EXTERNAL);
  #else
    dac1.analogReference(INTERNAL);
  #endif
}

void audio_volume(int volume) {
  mixer2.gain(0, (volume/1023.)*MAIN_GAIN);
  mixer2.gain(1, (volume/1023.)*DELAY_GAIN);
  mixer2.gain(2, (volume/1023.)*KICK_GAIN);
  mixer2.gain(3, (volume/1023.)*HAT_GAIN);
}

#endif
