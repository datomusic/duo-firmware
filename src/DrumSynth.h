/*
  DATO DUO firmware - DrumSynth
  (c) 2017, Benjamin Brandenburg <bnjmn7@gmail.com>
*/

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthWaveformSine   hat_sine1;          //xy=71,248
AudioSynthNoiseWhite     hat_noise1;         //xy=88,171
AudioSynthSimpleDrum     kick_drum1;          //xy=134,415
AudioEffectEnvelope2      hat_envelope3;      //xy=246,235
AudioEffectEnvelope2      hat_envelope1;      //xy=256,138
AudioEffectEnvelope2     kick_envelope2;      //xy=308,411
AudioFilterStateVariable hat_filter1;        //xy=432,130
AudioFilterStateVariable kick_filter2;        //xy=443,282
AudioFilterBiquad        hat_biquad1;        //xy=575,92
AudioConnection          hatkick_patchCord1(hat_sine1, hat_envelope3);
AudioConnection          hatkick_patchCord2(hat_noise1, hat_envelope1);
AudioConnection          hatkick_patchCord3(kick_drum1, kick_envelope2);
AudioConnection          hatkick_patchCord4(hat_envelope3, 0, hat_filter1, 1);
AudioConnection          hatkick_patchCord5(hat_envelope1, 0, hat_filter1, 0);
AudioConnection          hatkick_patchCord6(kick_envelope2, 0, kick_filter2, 0);
AudioConnection          hatkick_patchCord7(hat_filter1, 0, hat_biquad1, 0);
AudioConnection          hatkick_patchCord8(kick_filter2, 0, mixer2, 2);
AudioConnection          hatkick_patchCord9(hat_biquad1, 0, mixer2, 3);

// GUItool: end automatically generated code

int kick_duration = 100;
unsigned long kick_on_time, hat_on_time;
bool kick_playing = 0;
int hat_duration = 200;
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
  Serial.print("\nevent callback ");
  Serial.print(event);
  Serial.print(" a: ");
  Serial.print(slider1.a.getSignal());
  Serial.print(" b: ");
  Serial.print(slider1.b.getSignal());

  int val = constrain(map(value,-50,100,0,100),0,127);
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
  Serial.print("\nevent callback ");
  Serial.print(event);
  Serial.print(" a: ");
  Serial.print(slider2.a.getSignal());
  Serial.print(" b: ");
  Serial.print(slider2.a.getSignal());

  int val = constrain(map(value,-50,100,0,100),0,127);
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
  hat_envelope1.attack(2);
  hat_envelope1.release(1);
 // hat_envelope1.decay(1);
  hat_envelope1.sustain(0.02);
  hat_biquad1.setHighShelf(1, 2000, 0.7, 1); // before freq = 2000

  //SINE WAVE ADDED TO NOISE
  hat_envelope3.attack(0.5);
  hat_envelope3.release(1);
  hat_envelope3.sustain(0.04);

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
  //.noteOn();
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
 int val_hattouch = velocity*8;
 int freq = 4500 - val_hattouch;
 int freq_2 = velocity/2; // mapping 0 - 63
 float q_respot = val_hattouch / 500.0; // Q FROM 0.0 to 2.0
 AudioNoInterrupts();
 //change Hat parameters here
 hat_noise1.amplitude(1.0);
 hat_envelope1.decay(40+3*freq_2);
 hat_filter1.frequency(freq); // FILTER_RES_POT
 hat_filter1.resonance(q_respot); // OSC_DETUNE POT
 hat_biquad1.setBandpass(0, 8000-6*val_hattouch, 1); // Before 10000
 hat_biquad1.setHighpass(0, 9000-4*val_hattouch, 1);
 // SINE WAVE ADDED TO NOISE
 hat_sine1.frequency(200+2*val_hattouch);
 hat_sine1.amplitude(1.0);
 hat_envelope3.decay(40+freq_2);
 AudioInterrupts();
 //.noteOn();
 hat_envelope1.noteOn();
 hat_envelope3.noteOn();
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
