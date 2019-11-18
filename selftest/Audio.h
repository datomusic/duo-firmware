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
#include <output_dac.h>
#include <synth_sine.h>

// GUItool: begin automatically generated code
AudioSynthWaveformSine   sine1;          //xy=226,269
AudioOutputAnalog        dac1;           //xy=353,269
AudioConnection          patchCord1(sine1, dac1);
// GUItool: end automatically generated code
bool amp_enabled = 0;
void headphone_enable();
void headphone_disable();
void audio_init();
void audio_stop();
void amp_enable();
void amp_disable();

void audio_init() {
  AudioMemory(32);
  headphone_enable();
  sine1.frequency(233);
  sine1.amplitude(1.0);
}

void audio_stop() {
  sine1.amplitude(0);
}


void amp_enable() {
  if(amp_enabled == 0) {
    pinMode(AMP_ENABLE, OUTPUT);
    digitalWrite(AMP_ENABLE, LOW);
    amp_enabled = 1;
  }
}

void amp_disable() {
  if(amp_enabled == 1) {
    pinMode(AMP_ENABLE, INPUT);
    amp_enabled = 0;
  }
}

void headphone_disable() {
  digitalWrite(HP_ENABLE, LOW);
}

void headphone_enable() {
  digitalWrite(HP_ENABLE, HIGH);
}