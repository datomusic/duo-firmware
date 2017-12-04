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
#include "effect_custom_envelope.h"
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
AudioSynthWaveform       osc_pulse;      //xy=78.10000610351562,97
AudioSynthWaveform       osc_saw;        //xy=79.10000610351562,44
AudioSynthWaveformDc     dc1;            //xy=88.10000610351562,149
AudioEffectCustomEnvelope envelope2;      //xy=227.10000610351562,149
AudioMixer4              mixer1;         //xy=255.10000610351562,83
AudioFilterStateVariable filter1;        //xy=403.1000061035156,91
AudioEffectCustomEnvelope envelope1;      //xy=560.1000061035156,81
AudioAnalyzePeak         peak1;          //xy=705.1000061035156,37
AudioEffectDelay         delay1;         //xy=712.0999755859375,174.10000610351562
AudioEffectBitcrusher    bitcrusher1;    //xy=718.1000061035156,81
AudioMixer4              mixer_delay;     //xy=728.0999755859375,279.1000061035156
AudioMixer4              mixer_output;         //xy=861.1000061035156,100
AudioEffectFade          pop_suppressor;          //xy=1062,292
AudioAnalyzePeak         peak2;          //xy=987.1000061035156,151
AudioOutputAnalog        dac1;           //xy=988.1000061035156,100
AudioSynthNoiseWhite     hat_noise1;
AudioSynthSimpleDrum     kick_drum1;
AudioEffectCustomEnvelope hat_envelope1;
AudioSynthSimpleDrum     hat_snappy;
AudioFilterStateVariable hat_filter_bp;
AudioFilterStateVariable hat_filter_hp;
AudioMixer4              hat_mixer;
AudioConnection          patchCord1(osc_pulse, 0, mixer1, 1);
AudioConnection          patchCord2(osc_saw, 0, mixer1, 0);
AudioConnection          patchCord3(dc1, envelope2);
AudioConnection          patchCord4(envelope2, 0, filter1, 1);
AudioConnection          patchCord5(mixer1, 0, filter1, 0);
AudioConnection          patchCord6(filter1, 0, envelope1, 0);
AudioConnection          patchCord7(envelope1, peak1);
AudioConnection          patchCord8(envelope1, bitcrusher1);
AudioConnection          patchCord9(delay1, 0, mixer_output, 1);
AudioConnection          patchCord10(delay1, 0, mixer_delay, 1);
AudioConnection          patchCord11(bitcrusher1, 0, mixer_output, 0);
AudioConnection          patchCord12(bitcrusher1, 0, mixer_delay, 0);
AudioConnection          patchCord13(mixer_delay, delay1);
AudioConnection          patchCord20(hat_noise1, hat_envelope1);
AudioConnection          patchCord21(hat_envelope1, 0, hat_filter_hp, 0);
AudioConnection          patchCord22(hat_filter_hp, 2, hat_filter_bp, 0);
AudioConnection          patchCord23(hat_filter_bp, 1, hat_mixer, 0);
AudioConnection          patchCord24(hat_snappy, 0, hat_mixer, 1);
AudioConnection          patchCord25(hat_mixer, 0, mixer_delay, 3);
AudioConnection          patchCord26(kick_drum1, 0, mixer_output, 2);
AudioConnection          patchCord27(hat_mixer, 0, mixer_output, 3);
AudioConnection          patchCord14(mixer_output, pop_suppressor);
AudioConnection          patchCord15(mixer_output, peak2);
AudioConnection          patchCord16(pop_suppressor, dac1);
// GUItool: end automatically generated code

#define MAIN_GAIN 0.8
#define DELAY_GAIN 0.6
#define KICK_GAIN 0.8
#define HAT_GAIN 0.9

void audio_init();
void audio_volume(uint8_t volume);
void synth_update();

void audio_init() {
  AudioMemory(128); // 260 bytes per block, 2.9ms per block

  // Oscillators
  osc_saw.begin(0.4, 110, WAVEFORM_SAWTOOTH);
  osc_pulse.pulseWidth(0.5);
  osc_pulse.begin(0.4, 220, WAVEFORM_PULSE);
  
  // Mixer mixes the oscillators - don't add up to more than 0.8 or the output will clip
  mixer1.gain(0, 0.4); // OSC1
  mixer1.gain(1, 0.4); // OSC2

  // Filter
  filter1.resonance(0.7); // range 0.7-5.0
  filter1.frequency(400);
  filter1.octaveControl(4);

  // Amp envelope
  envelope1.attack(2);
  envelope1.decay(0);
  envelope1.sustain(1.0);
  envelope1.release(400);

  // Filter envelope
  dc1.amplitude(1.0); // Filter env needs an input signal
  envelope2.attack(15);
  envelope2.decay(0);
  envelope2.sustain(1.0);
  envelope2.release(300);

  // Bitcrusher - set to audio samplerate and max bitrate
  bitcrusher1.bits(16);
  bitcrusher1.sampleRate(44100);

  delay1.delay(0, 440); // Delay time
  mixer_delay.gain(0, 0.0); // Delay input
  mixer_delay.gain(1, 0.4); // Delay feedback

  mixer_output.gain(0, MAIN_GAIN);
  mixer_output.gain(1, DELAY_GAIN); // Delay output
  mixer_output.gain(2, KICK_GAIN); // Kick output
  mixer_output.gain(3, HAT_GAIN); // Hat output
  
  /* 
    Sets the reference voltage for the internal DAC, which
    in turn determines the peak to peak voltage of the audio
    signal.

    Clipping and other distortion may occur in SUPER_LOUD_MODE
    so use with caution.

    INTERNAL = 1.1V internal reference
    EXTERNAL = 3.3V supply voltage
    */
  #ifdef SUPER_LOUD_MODE
    dac1.analogReference(EXTERNAL);
  #else
    dac1.analogReference(INTERNAL);
  #endif
}

void audio_volume(int volume) {
  mixer_output.gain(0, (volume/1023.)*MAIN_GAIN);
  mixer_output.gain(1, (volume/1023.)*DELAY_GAIN);

  mixer_output.gain(2, (volume/2048.)*KICK_GAIN+0.5);
  mixer_output.gain(3, (volume/2048.)*HAT_GAIN+0.5);
}

void synth_update() {
  // Audio interrupts have to be off to apply settings
  AudioNoInterrupts();

  osc_saw.frequency(osc_saw_frequency);

  if(synth.detune > 850) {
    osc_saw.amplitude(map(synth.detune,850,1023,400,0)/1000.);
  } else if(synth.detune < 200) {
    osc_saw.amplitude(map(synth.detune,0,200,200,400)/1000.);
  } else {
    osc_saw.amplitude(0.4);
  }
  osc_pulse.frequency(osc_pulse_frequency);
  osc_pulse.pulseWidth(map(synth.pulseWidth,0,1023,1000,100)/1000.0);

  filter1.frequency((synth.filter/2)+30);
  filter1.resonance(map(synth.resonance,0,1023,70,320)/100.0); // 0.7-3.2 range

  envelope1.release(((synth.release*synth.release) >> 11)+30);

  if(!synth.crush) {
    bitcrusher1.sampleRate(HIGH_SAMPLE_RATE);
  } else {
    bitcrusher1.sampleRate(LOW_SAMPLE_RATE);
  }

  audio_volume(synth.amplitude);

  AudioInterrupts(); 
}
#endif
