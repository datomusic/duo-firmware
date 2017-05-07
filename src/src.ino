/*
  DATO DUO firmware
  (c) 2016, David Menting <david@dato.mu>
*/
#include "Arduino.h"
#include <Keypad.h>
#include "TouchSlider.h"

#define VERSION "0.6.6"

/*
  DEV mode does the following:
  - Reset the DUO to DFU mode when long pressing the Play button
 */
#define DEV_MODE

const int MIDI_CHANNEL = 1;

// Musical settings
const uint8_t SCALE[] = { 49,51,54,56,58,61,63,66,68,70 };
const uint8_t SCALE_OFFSET_FROM_C3[] { 1,3,6,8,10,13,15,18,20,22 };
const float   SAMPLERATE_STEPS[] = { 44100,4435,2489,1109 }; 

#define INITIAL_VELOCITY 100

// Globals that should not be globals
int gate_length_msec = 40;

// Sequencer settings
uint8_t current_step; // TODO: should be sequencer_num_steps
int tempo = 0;
uint8_t set_key = 9;
float osc_saw_frequency = 0.;
float osc_pulse_frequency = 0.;
float osc_pulse_target_frequency = 0.;
float osc_saw_target_frequency = 0.;
uint8_t osc_pulse_midi_note = 0;
uint8_t note_is_playing = 0;
bool note_is_triggered = false;
int transpose = 0;
bool next_step_is_random = false;
int tempo_interval;
bool random_flag = 0;

uint32_t midi_clock = 0;
uint16_t audio_peak_values;
uint16_t peak_update_time;

void keys_scan();
void pots_read();
void drum_init();
void drum_read();
void print_log();

void note_on(uint8_t midi_note, uint8_t velocity, bool enabled);
void note_off();

void keyboard_to_note();
float detune(int note, int amount);

int tempo_interval_msec();

void enter_dfu();

// 10 bit representation of full scale parameters
typedef struct {
  int detune;
  int pulseWidth;
  int filter;
  int resonance;
  int release;
  int amplitude;
  bool glide;
  bool accent;
  bool delay;
  bool crush;
} synth_parameters;

synth_parameters synth;
#include "note_stack.h"
NoteStack note_stack;

#include "pinmap.h"
#include "MidiFunctions.h"
#include "MathFunctions.h"
#include "Buttons.h"
#include "Synth.h"
#include "TempoHandler.h"
TempoHandler tempo_handler;

#include "Sequencer.h"
#include "Leds.h"
#include "DrumSynth.h"
#include "Pitch.h"

#include "Power.h"

void setup() {

  pins_init();
  amp_disable();
  headphone_disable();
  sequencer_init();
  audio_init();
  led_init();

  midi_init();

  MIDI.setHandleStart(sequencer_restart);
  MIDI.setHandleContinue(sequencer_restart);
  MIDI.setHandleStop(sequencer_stop);
  MIDI.setHandleControlChange(midi_handle_cc);

  button_matrix_init();
  drum_init();
  touch_init();
  
  previous_note_on_time = millis();
  #ifdef DEV_MODE
    Serial.begin(57600);
    Serial.print("Dato DUO firmware ");
    Serial.println(VERSION);
  #endif
  headphone_enable();
}

void loop() {
  if(power_check()) {
    // Fast stuff
    keys_scan();
    keyboard_to_note();   
    midi_handle();
    pitch_update();   

    sequencer_update();

    pots_read();

    sequencer_update();

    drum_read();

    sequencer_update();

    led_update();

    sequencer_update();
  }
}

// Scans the button_matrix and handles step enable and keys
void keys_scan() {
  if(muxDigitalRead(DELAY_PIN)) {
    synth.delay = false;
    mixer_delay.gain(0, 0.0); // Delay input
    mixer_delay.gain(3, 0.0);
  } else {
    synth.delay = true;
    mixer_delay.gain(0, 0.5); // Delay input
    mixer_delay.gain(3, 0.4); // Hat delay input
  }

  if (button_matrix.getKeys())  {
    for (int i=0; i<LIST_MAX; i++) {
      if ( button_matrix.key[i].stateChanged ) {
        char k = button_matrix.key[i].kchar;
        switch (button_matrix.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
            case PRESSED:   
                if (k <= KEYB_9 && k >= KEYB_0) {
                  keyboard_set_note(SCALE[k - KEYB_0]);
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
                  #ifdef DEV_MODE
                    enter_dfu();
                  #else
                    power_off();
                  #endif
                } 
                #ifdef DEV_MODE
                if (k == BTN_UP) {
                  print_log();
                }
                #endif
                break;
            case RELEASED:
                if (k <= KEYB_9 && k >= KEYB_0) {
                  keyboard_unset_note(SCALE[k - KEYB_0]);
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
                  random_flag = false;
                } 
                break;
            case IDLE:
                if (k <= KEYB_9 && k >= KEYB_0) {
                  keyboard_unset_note(k - KEYB_0);
                }
                break;
        }
      }
    }
  } 
}

void pots_read() {
  gate_length_msec = map(analogRead(GATE_POT),1023,0,10,200);
  
  synth.detune = muxAnalogRead(OSC_DETUNE_POT);
  synth.release = muxAnalogRead(AMP_ENV_POT);
  synth.filter = muxAnalogRead(FILTER_FREQ_POT);
  synth.amplitude = muxAnalogRead(FADE_POT);
  synth.pulseWidth = muxAnalogRead(OSC_PW_POT);
  synth.resonance = muxAnalogRead(FILTER_RES_POT);

  analogWrite(FILTER_LED, synth.filter>>2);
  analogWrite(OSC_LED, 255-(synth.pulseWidth>>2));

  // Audio interrupts have to be off to apply settings
  AudioNoInterrupts();

  osc_saw.frequency(osc_saw_frequency);

  if(detune_amount > 800) {
    osc_saw.amplitude(map(detune_amount,800,1023,400,0)/1000.);
  } else {
    osc_saw.amplitude(0.4);
  }
  osc_pulse.frequency(osc_pulse_frequency);
  osc_pulse.pulseWidth(map(synth.pulseWidth,0,1023,1000,100)/1000.0);

  filter1.frequency(((synth.filter*synth.filter) >> 11)+40);
  filter1.resonance(map(synth.resonance,0,1023,70,400)/100.0); // 0.7-5.0 range

  envelope1.release(((synth.release*synth.release) >> 11)+30);

  if(digitalRead(BITC_PIN)) {
    bitcrusher1.sampleRate(SAMPLERATE_STEPS[0]);
  } else {
    bitcrusher1.sampleRate(SAMPLERATE_STEPS[2]);
  }

  audio_volume(synth.amplitude);

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
    osc_pulse_midi_note = midi_note;
    osc_pulse_target_frequency = (int)midi_note_to_frequency(midi_note);
    osc_pulse.frequency(osc_pulse_frequency);
    // Detune OSC2
    osc_saw.frequency(detune(osc_pulse_midi_note,detune_amount));

    AudioInterrupts(); 

    MIDI.sendNoteOn(midi_note, velocity, MIDI_CHANNEL);
    usbMIDI.sendNoteOn(midi_note, velocity, MIDI_CHANNEL);
    envelope1.noteOn();
    envelope2.noteOn();
  } else {
    leds(current_step) = LED_WHITE;
  }
}

void note_off() {
  if (note_is_playing) {
    MIDI.sendNoteOff(note_is_playing, 0, MIDI_CHANNEL);
    usbMIDI.sendNoteOff(note_is_playing, 0, MIDI_CHANNEL);
    if(!step_enable[current_step]) {
      leds(current_step) = CRGB::Black;
    } else {
      envelope1.noteOff();
      envelope2.noteOff();
    }
    note_is_playing = 0;
  } 
}

void print_log() {
  Serial.printf("Release: %d\n", synth.release);
}

/*
 * enter_dfu() writes a special string to the K20DX256's VBAT register and reboots the mcu
 * This signals the Kiibohd DFU bootloader that it should start in firmware update mode
 */
void enter_dfu() {
  #define VBAT *(volatile uint8_t *)0x4003E000 // Register available in all power states
  const uint8_t sys_reset_to_loader_magic[22] = "\xff\x00\x7fRESET TO LOADER\x7f\x00\xff";

  // Blank all leds and turn the power button blue before rebooting
  FastLED.clear();
  physical_leds[0] = CRGB::Blue;
  FastLED.show();
  // Reset
  for (unsigned int pos = 0; pos < sizeof(sys_reset_to_loader_magic); pos++ ) {
    (&VBAT)[ pos ] = sys_reset_to_loader_magic[ pos ];
  }
  SCB_AIRCR = 0x05FA0004; // software reset
}
