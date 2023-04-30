#ifndef Synth_h
#define Synth_h

#include <analyze_peak.h>
#include <effect_bitcrusher.h>
#include <effect_delay.h>
#include <effect_fade.h>
#include "firmware/effect_custom_envelope.h"
#include <effect_multiply.h>
#include <filter_variable.h>
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
AudioFilterStateVariable delay_filter;         //xy=712.0999755859375,174.10000610351562
AudioEffectFade          delay_fader;
AudioEffectBitcrusher    bitcrusher1;    //xy=718.1000061035156,81
AudioMixer4              mixer_delay;     //xy=728.0999755859375,279.1000061035156
AudioMixer4              mixer_output;         //xy=861.1000061035156,100
AudioEffectFade          pop_suppressor;          //xy=1062,292
AudioAnalyzePeak         peak2;          //xy=987.1000061035156,151
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

// Feedback path
AudioConnection          patchCord14(mixer_delay, delay1);
AudioConnection          patchCord9(delay1, 0, delay_filter, 0);
AudioConnection          patchCord15(delay_filter, 2, delay_fader, 0);
AudioConnection          patchCord10(delay_fader, 0, mixer_delay, 1);

AudioConnection          patchCord11(delay1, 0, mixer_output, 1); 


AudioConnection          patchCord12(bitcrusher1, 0, mixer_output, 0);
AudioConnection          patchCord13(bitcrusher1, 0, mixer_delay, 0);
// AudioConnection          patchCord13(mixer_delay, delay1);

AudioConnection          patchCord21(hat_noise1, hat_envelope1);
AudioConnection          patchCord22(hat_envelope1, 0, hat_filter_hp, 0);
AudioConnection          patchCord23(hat_filter_hp, 2, hat_filter_bp, 0);
AudioConnection          patchCord24(hat_filter_bp, 1, hat_mixer, 0);
AudioConnection          patchCord25(hat_snappy, 0, hat_mixer, 1);
AudioConnection          patchCord26(hat_mixer, 0, mixer_delay, 3);
AudioConnection          patchCord27(kick_drum1, 0, mixer_output, 2);
AudioConnection          patchCord28(hat_mixer, 0, mixer_output, 3);
AudioConnection          patchCord29(mixer_output, pop_suppressor);
AudioConnection          patchCord30(mixer_output, peak2);
// GUItool: end automatically generated code

void audio_init();
void audio_volume(int volume);
void synth_update();

void audio_init() {
  // assume AudioNoInterrupts
  AudioMemory(192); // 260 bytes per block, 2.9ms per block
  
  // Mixer mixes the oscillators - don't add up to more than 0.8 or the output will clip
  #if defined(__MIMXRT1011__)
  mixer1.gain(0, 0.2f); // OSC1
  osc_saw.begin(0.4f, 110, WAVEFORM_BANDLIMIT_SAWTOOTH);
  osc_pulse.pulseWidth(0.5f);
  osc_pulse.begin(.5f, 220, WAVEFORM_BANDLIMIT_PULSE);
  #else
  osc_saw.begin(0.4f, 110, WAVEFORM_SAWTOOTH);
  osc_pulse.pulseWidth(0.5f);
  osc_pulse.begin(.4f, 220, WAVEFORM_PULSE);
  mixer1.gain(0, 0.4f);
  #endif
  mixer1.gain(1, 0.4f); // OSC2

  // Filter
  filter1.resonance(0.7f); // range 0.7-5.0
  filter1.frequency(400);
  filter1.octaveControl(4);

  // Amp envelope
  envelope1.attack(2);
  envelope1.decay(0);
  envelope1.sustain(1.0f);
  envelope1.release(400);

  // Filter envelope
  dc1.amplitude(1.0); // Filter env needs an input signal
  envelope2.attack(15);
  envelope2.decay(0);
  envelope2.sustain(1.0f);
  envelope2.release(300);

  // Bitcrusher - set to audio samplerate and max bitrate
  bitcrusher1.bits(16);
  bitcrusher1.sampleRate(44100);

  delay1.delay(0, 350); // Delay time
  delay_filter.frequency(400); // High pass filter in feedback
  mixer_delay.gain(0, 0.0f); // Delay input
  mixer_delay.gain(1, 0.4f); // Delay feedback

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
    //dac1.analogReference(EXTERNAL);
  #else
    //dac1.analogReference(INTERNAL);
  #endif
}

inline void audio_volume(int volume) {
  static const int LOW_VOLUME_THRESHOLD = 4;

  if(volume < LOW_VOLUME_THRESHOLD) {
    mixer_output.gain(0,0);
    mixer_output.gain(1,0);
    mixer_output.gain(2,0);
    mixer_output.gain(3,0);
  } else {
    mixer_output.gain(0, (volume/(1023.f/MAIN_GAIN)));
    mixer_output.gain(1, (volume/(1023.f/DELAY_GAIN)));
    mixer_output.gain(2, ((volume+512)/(2048.f/KICK_GAIN)));
    mixer_output.gain(3, ((volume+512)/(2048.f/HAT_GAIN)));
  }
}

void synth_update() {

  float osc_saw_amplitude = 0.4f;

  if(synth.detune > 850) {
    osc_saw_amplitude = map(synth.detune,850,1023,400,0) / 1000.0f;
  } else if(synth.detune < 200) {
    osc_saw_amplitude = map(synth.detune,0,200,200,400) / 1000.0f;
  }

  float bitcrusher_samplerate = HIGH_SAMPLE_RATE;
  if(synth.crush) {
    bitcrusher_samplerate = LOW_SAMPLE_RATE;
  }

  float osc_pulse_pulseWidth = map(synth.pulseWidth,0,1023,500,950)/1000.0f;
  float filter_resonance = map(synth.resonance,0,1023,70,320)/100.0f;
  
  // Audio interrupts have to be off to apply settings
  AudioNoInterrupts();

  osc_saw.frequency(osc_saw_frequency);
  osc_saw.amplitude(osc_saw_amplitude);
  
  osc_pulse.frequency(osc_pulse_frequency/2);
  osc_pulse.pulseWidth(osc_pulse_pulseWidth);

  filter1.frequency((synth.filter/2)+30);
  filter1.resonance(filter_resonance); // 0.7-3.2 range

  envelope1.release(((synth.release*synth.release) >> 11)+30);

  bitcrusher1.sampleRate(bitcrusher_samplerate);

  audio_volume(synth.amplitude);
  
  AudioInterrupts(); 
}
#endif
