/*
  DATO DUO firmware
  (c) 2016, David Menting <david@dato.mu>
*/
#include "Arduino.h"
#include <Keypad.h>
#include "TouchSlider.h"

#define VERSION "0.4.7"

const int MIDI_CHANNEL = 1;
const int SYNC_LENGTH_MSEC = 12;

// Musical settings
const uint8_t SCALE[] = { 49,51,54,56,58,61,63,66,68,70 }; // Low with 2 note split
const float   SAMPLERATE_STEPS[] = { 44100,4435,2489,1109 }; 

#define INITIAL_VELOCITY 100

// Globals that should not be globals
int gate_length_msec = 40;

// Sequencer settings
uint8_t current_step = 8; // TODO: should be sequencer_num_steps
uint8_t target_step = 0;
int tempo = 0;
uint8_t set_key = 9;
int detune_amount = 0;
float osc_saw_frequency = 0.;
float osc_pulse_frequency = 0.;
float osc_saw_target_frequency = 0.;
uint8_t osc_saw_midi_note = 0;
bool note_is_playing = 0;
bool note_is_triggered = false;
bool note_is_played = false;
bool double_speed = false;
int transpose = 0;
bool next_step_is_random = false;
int num_notes_held = 0;
int tempo_interval;
bool random_flag = 0;
bool power_flag = 1;

uint16_t keyboard_map = 0;
uint16_t old_keyboard_map = 0;
const uint16_t KEYBOARD_MASK = 0b11111111111;

void keys_scan();
void pots_read();
void drum_init();
void drum_read();

void note_on(uint8_t midi_note, uint8_t velocity, bool enabled);
void note_off();

void keyboard_to_note();
float detune(int note, int amount);

int tempo_interval_msec();

void power_off();
void power_on();
void amp_enable();
void amp_disable();
void headphone_disable();
void headphone_enable();

#include "pinmap.h"
#include "MidiFunctions.h"
#include "Buttons.h"
#include "Synth.h"
#include "Sequencer.h"
#include "Leds.h"
#include "DrumSynth.h"
#include "Pitch.h"

void setup() {
  pins_init();
  amp_disable();
  headphone_disable();
  sequencer_init();
  audio_init();
  led_init();
  Serial.begin(57600);

  midi_init();

  keys_init();

  drum_init();

  touch_init();
  
  previous_note_on_time = millis();
  
  #ifdef NO_AUDIO
  amp_disable();
  #else
  amp_enable();
  headphone_enable();
  #endif

  Serial.print("Dato DUO firmware ");
  Serial.println(VERSION);
}

void loop() {

  if(power_flag != 0) {

    keys_scan();

    if(!sequencer_is_running) {
      keyboard_to_note();          
    }
    sequencer_update();
    midi_handle();
    pots_read();
    update_leds();
    drum_read();
  } else {
    //TODO: Why do I need to force these LEDs low?
    analogWrite(ENV_LED, 0);
    analogWrite(FILTER_LED, 0);
    analogWrite(OSC_LED, 0);

    if(keys_scan_powerbutton()) {
      power_on();
    } else {
      //TODO: Low power delay
      delay(100);
    }
  }
}

void keyboard_to_note() {
  // If the old map was zero and now it's not, turn on the right note
  if((old_keyboard_map & KEYBOARD_MASK) != (keyboard_map & KEYBOARD_MASK)) {
    // We're starting from the top. High notes have priority
    for(int i = 10; i >= 0; i--) {
      if(bitRead(keyboard_map,i)){
        note_on(SCALE[i]+transpose, INITIAL_VELOCITY, true);
        break;
      }
    }
  }

  if((old_keyboard_map & KEYBOARD_MASK) && (keyboard_map & KEYBOARD_MASK)==0) {
    // If the old map was not zero and now it is, turn off the note
    note_off();
  }
  old_keyboard_map = keyboard_map;
}

// Scans the keypad and handles step enable and keys
void keys_scan() {
  if(muxDigitalRead(DELAY_PIN)) {
    delayMixer.gain(0, 0.0); // Delay input
    delayMixer.gain(3, 0.0);
  } else {
    delayMixer.gain(0, 0.5); // Delay input
    delayMixer.gain(3, 0.4); // Hat delay input
  }

  if(digitalRead(JACK_DETECT)) {
    amp_disable();
  } else {
    amp_enable();
  }

  if (keypad.getKeys())  {
    for (int i=0; i<LIST_MAX; i++) {
      if ( keypad.key[i].stateChanged ) {
        char k = keypad.key[i].kchar;
        switch (keypad.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
            case PRESSED:   
                if (k <= KEYB_9 && k >= KEYB_0) {
                  bitSet(keyboard_map,(k - KEYB_0));
                  if(sequencer_is_running) {
                    step_note[target_step] = k - KEYB_0;
                    step_enable[target_step] = 1;
                    step_velocity[target_step] = INITIAL_VELOCITY; 
                  } else {
                    current_step++;
                    if (current_step >= SEQUENCER_NUM_STEPS) current_step = 0;
                    target_step=current_step;
                    step_note[target_step] = k - KEYB_0;
                    step_enable[target_step] = 1;
                    step_velocity[target_step] = INITIAL_VELOCITY; 
                  }
                } else if (k <= STEP_8 && k >= STEP_1) {
                  step_enable[k-STEP_1] = 1-step_enable[k-STEP_1];
                  if(!step_enable[k-STEP_1]) { leds(k-STEP_1) = CRGB::Black; }
                  step_velocity[k-STEP_1] = INITIAL_VELOCITY;
                } else if (k == BTN_SEQ2) {
                  if(!sequencer_is_running) {
                    sequencer_advance();
                  }
                  double_speed = true;
                } else if (k == BTN_DOWN) {
                  transpose--;
                  if(transpose<-12){transpose = -24;}
                } else if (k == BTN_UP) {
                  transpose++;
                  if(transpose>12){transpose = 24;}
                } else if (k == BTN_SEQ1) {
                  next_step_is_random = true;
                  if(!sequencer_is_running) {
                    sequencer_advance();
                  }
                  random_flag = true;
                } else if (k == SEQ_START) {
                  sequencer_toggle_start();
                }
                break;
            case HOLD:
                if (k == SEQ_START) {
                  power_off();
                }
                break;
            case RELEASED:
                if (k <= KEYB_9 && k >= KEYB_0) {
                  MIDI.sendNoteOff(SCALE[k-KEYB_0]+transpose,64,MIDI_CHANNEL);
                  bitClear(keyboard_map,(k - KEYB_0));
                } else if (k == BTN_SEQ2) {
                  double_speed = false;
                } else if (k == BTN_DOWN) {
                  if(transpose<-12){transpose = -12;}
                  if(transpose>12){transpose = 12;}
                } else if (k == BTN_UP) {
                  if(transpose<-12){transpose = -12;}
                  if(transpose>12){transpose = 12;}
                } else if (k == BTN_SEQ1) {
                  next_step_is_random = false;
                } 
                break;
            case IDLE:
                break;
        }
      }
    }
  } 
}

void pots_read() {
  // Read out the pots/switches
  gate_length_msec = map(analogRead(GATE_POT),1023,0,10,200);
  detune_amount = 1023-muxAnalogRead(OSC_DETUNE_POT);

  int volume_pot_value = muxAnalogRead(FADE_POT);
  int resonance = muxAnalogRead(FILTER_RES_POT);
  int amp_env_release = map(muxAnalogRead(AMP_ENV_POT),0,1023,30,500);
  int filter_pot_value = muxAnalogRead(FILTER_FREQ_POT);
  int pulse_pot_value = muxAnalogRead(OSC_PW_POT);

  float osc_pulse_target_frequency = detune(osc_saw_midi_note,detune_amount);

  analogWrite(FILTER_LED, filter_pot_value>>2);
  analogWrite(OSC_LED, 255-(pulse_pot_value>>2));

  // Constant rate glide
  const float GLIDE_COEFFICIENT = 0.04f;
  if(!muxDigitalRead(SLIDE_PIN)) {
    osc_saw_frequency = osc_saw_frequency + (osc_saw_target_frequency - osc_saw_frequency)*GLIDE_COEFFICIENT;
    osc_pulse_frequency = osc_pulse_frequency + (osc_pulse_target_frequency - osc_pulse_frequency)*GLIDE_COEFFICIENT;
  } else {
    osc_saw_frequency = osc_saw_target_frequency;
    osc_pulse_frequency = osc_pulse_target_frequency;
  }

  // Audio interrupts have to be off to apply settings
  AudioNoInterrupts();

  osc_saw.frequency(osc_saw_frequency);

  // TODO: mix away the oscillator at the end of the range
  osc_pulse.frequency(osc_pulse_frequency);
  osc_pulse.pulseWidth(map(pulse_pot_value,0,1023,1000,50)/1000.0);

  filter1.frequency(map(filter_pot_value,0,1023,60,400));
  //filter1.frequency(fscale(0.,1023.,60.,300.,filter_pot_value,0));

  // TODO: do exponential filter pot behaviour
  filter1.resonance(map(resonance,0,1023,500,70)/100.0); // 0.7-5.0 range

  envelope1.release(amp_env_release);

  if(digitalRead(BITC_PIN)) {
    bitcrusher1.sampleRate(SAMPLERATE_STEPS[0]);
  } else {
    bitcrusher1.sampleRate(SAMPLERATE_STEPS[2]);
  }

  audio_volume(1023-volume_pot_value);

  AudioInterrupts(); 
}

void note_on(uint8_t midi_note, uint8_t velocity, bool enabled) {

  // Override velocity if button on the synth is pressed
  if(!digitalRead(ACCENT_PIN)) {
    velocity = 127;
  }

  note_is_playing = midi_note;

  if(enabled) {
    AudioNoInterrupts();

    dc1.amplitude(velocity / 127.); // DC amplitude controls filter env amount.
    osc_saw_midi_note = midi_note;
    osc_saw_target_frequency = (int)midi_note_to_frequency(midi_note);
    osc_saw.frequency(osc_saw_frequency);
    // Detune OSC2
    osc_pulse.frequency(detune(osc_saw_midi_note,detune_amount));

    AudioInterrupts(); 

    MIDI.sendNoteOn(midi_note, velocity, MIDI_CHANNEL);
    envelope1.noteOn();
    envelope2.noteOn();
  } else {
    // Set LED to white but don't play a note
    leds(current_step) = LED_WHITE;
  }
}

void note_off() {
  if (note_is_playing) {
    MIDI.sendNoteOff(note_is_playing, 64, MIDI_CHANNEL);
    if(!step_enable[current_step]) {
      leds(current_step) = CRGB::Black;
    } else {
      envelope1.noteOff();
      envelope2.noteOff();
    }
    note_is_playing = 0;
  } 
}

float detune(int note, int amount) { // amount goes from 0-1023
  if (amount > 800) {
    return midi_note_to_frequency(note) * (amount+9000)/10000.;
  } else if (amount < 100) {
    return midi_note_to_frequency(note - 12) * ( 20000 - amount )/20000.;
  } else {
    int offset = map(amount,200,900,4,0);
    return midi_note_to_frequency(note - DETUNE_OFFSET_SEMITONES[offset]);
  }
}

void power_off() { // TODO: this is super crude and doesn't work, but it shows the effect
  sequencer_stop();
  AudioNoInterrupts();
  amp_disable();

  for(int i = 32; i >= 0; i--) {
    FastLED.setBrightness(i);
    FastLED.show();
    analogWrite(ENV_LED,i);
    analogWrite(FILTER_LED,i);
    analogWrite(OSC_LED,i);
    delay(20);
  }
  FastLED.clear();
  FastLED.show();
  power_flag = 0;
  pinMode(row_pins[powerbutton_row],INPUT_PULLUP);
  pinMode(col_pins[powerbutton_col],OUTPUT);
  digitalWrite(col_pins[powerbutton_col],LOW);
  while(!digitalRead(row_pins[powerbutton_row])) {
    // wait for release of the power button
  }
  delay(100);
}

void power_on() {
  led_init();
  AudioInterrupts();
  digitalWrite(AMP_ENABLE, LOW);
  power_flag = 1;
}

void amp_enable() {
  pinMode(AMP_ENABLE, OUTPUT);
  digitalWrite(AMP_ENABLE, LOW);
}

void amp_disable() {
  pinMode(AMP_ENABLE, INPUT);
}

void headphone_disable() {
  digitalWrite(HP_ENABLE, LOW);
}

void headphone_enable() {
  digitalWrite(HP_ENABLE, HIGH);
}