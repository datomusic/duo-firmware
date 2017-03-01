/*
  DATO DUO firmware - DrumSynth
  (c) 2017, Benjamin Brandenburg <bnjmn7@gmail.com>
*/
#include <filter_biquad.h>
#include <synth_sine.h>

  // GUItool: begin automatically generated code
AudioSynthNoiseWhite     hat_noise1;     //xy=378,191
AudioSynthSimpleDrum     kick_drum1;     //xy=437,383
AudioEffectEnvelope2     hat_envelope1;  //xy=546,191
AudioSynthSimpleDrum     hat_snappy;          //xy=590,250
AudioEffectEnvelope2     kick_envelope2; //xy=602,383
AudioFilterStateVariable hat_filter1;    //xy=713,197
AudioFilterStateVariable kick_filter2;   //xy=775,386
AudioMixer4              hat_mixer;         //xy=872,257
AudioConnection          hatkick_patchCord1(hat_noise1, hat_envelope1);
AudioConnection          hatkick_patchCord2(kick_drum1, kick_envelope2);
AudioConnection          hatkick_patchCord3(hat_envelope1, 0, hat_filter1, 0);
AudioConnection          hatkick_patchCord4(hat_snappy, 0, hat_mixer, 1);
AudioConnection          hatkick_patchCord5(kick_envelope2, 0, kick_filter2, 0);
AudioConnection          hatkick_patchCord6(hat_filter1, 2, hat_mixer, 0);
AudioConnection          hatkick_patchCord7(hat_mixer, 0, delayMixer, 3);
AudioConnection          hatkick_patchCord8(kick_filter2, 0, mixer2, 2);
AudioConnection          hatkick_patchCord9(hat_mixer, 0, mixer2, 3);
// GUItool: end automatically generated code

int kick_duration = 100;
unsigned long kick_on_time, hat_on_time;
bool kick_playing = 0;
int hat_duration = 100;
bool hat_playing = 0;

void drum_init();
void drum_read();
void kick_noteon(uint8_t velocity);
void hat_noteon(uint8_t velocity);
void kick_noteoff();
void hat_noteoff();
void kickEvent(uint8_t event, int value);
void hatEvent(uint8_t event, int value);

#define TOUCHEVENT_TOUCH 1
#define TOUCHEVENT_RELEASE 2

TouchSlider slider1(TOUCH4, TOUCH3);
TouchSlider slider2(TOUCH1, TOUCH2);

void kickEvent(uint8_t event, int value) {

  int val = constrain(value+63,0,127);

  switch(event) {
    case TOUCHEVENT_TOUCH:
      kick_noteon(val);
      break;
    case TOUCHEVENT_RELEASE:
      kick_noteoff();
      break;
  }
}

void hatEvent(uint8_t event, int value) {
  int val = constrain(value+63,0,127);

  switch(event) {
    case TOUCHEVENT_TOUCH:
      hat_noteon(val);
      break;
    case TOUCHEVENT_RELEASE:
      hat_noteoff();
      break;
  }
}


void touch_init() {
  slider1.a.setThreshold(30);
  slider1.b.setThreshold(30);
  slider1.setHandleTouchEvent(kickEvent);
  slider2.a.setThreshold(30);
  slider2.b.setThreshold(30);
  slider2.setHandleTouchEvent(hatEvent);
}


void drum_init() {
  // HI-HAT ->
  hat_envelope1.attack(2.0);
  hat_envelope1.release(0.0);
  // hat_envelope1.decay(1);
  hat_envelope1.sustain(0.0);
  // hat_biquad1.setHighShelf(1, 2000, 0.7, 1); // before freq = 2000
  hat_snappy.length(30);
  hat_snappy.pitchMod(4.0);
  hat_snappy.frequency(126);
  // KICK DRUM ->
  kick_drum1.length(kick_duration);
  kick_drum1.frequency(60);
  kick_drum1.pitchMod(4.0);
  kick_envelope2.attack(0.0);
  kick_envelope2.decay(25);
  kick_envelope2.sustain(20.0);
  kick_filter2.frequency(100);
  kick_filter2.resonance(1.5);

}

void drum_read() {
  slider1.update(millis());
  slider2.update(millis());
  
  if(millis() > kick_on_time + kick_duration) {
    kick_noteoff();
  }
  if(millis() > hat_on_time + hat_duration) {
    hat_noteoff();
  }
}

void kick_noteon(uint8_t velocity) {
  AudioNoInterrupts();
  //change Kick parameters here
  AudioInterrupts();
  kick_drum1.noteOn();
  kick_envelope2.noteOn();
  kick_playing = 1;
  kick_on_time = millis();
}

void kick_noteoff() {
  kick_envelope2.noteOff();
  kick_playing = 0;
}

void hat_noteon(uint8_t velocity) {
 hat_duration = velocity + 10;

 // int val_hattouch = velocity*8;
 // int freq = 4500 - velocity*8;
 // int freq_2 = velocity/2; // mapping 0 - 63
 // float q_respot = velocity / 127.0; // Q FROM 0.0 to 2.0
 hat_snappy.noteOn();
 AudioNoInterrupts();
 //change Hat parameters here
 hat_noise1.amplitude(1.0);
 hat_envelope1.decay(map(velocity,0,127,20,30));

 hat_filter1.frequency(2000);
 hat_filter1.resonance(map(velocity,0,127,300,70)/100.);

 hat_mixer.gain(0, map(velocity,0,127,70,20)/100.); // noise gain
 hat_mixer.gain(1, map(velocity,0,127,0,100)/100.); // snappy gain
 AudioInterrupts();
 hat_envelope1.noteOn();
 hat_playing = 1;
 hat_on_time = millis();
}

void hat_noteoff() {
  hat_envelope1.noteOff();
  AudioNoInterrupts();
  hat_noise1.amplitude(0.0);
  AudioInterrupts();
  hat_playing = 0;
}