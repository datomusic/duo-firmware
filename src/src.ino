/*
  DATO DUO firmware
  Copyright 2016, David Menting <david@dato.mu>

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  
  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
  
  See http://creativecommons.org/licenses/MIT/ for more information.
*/
#include "Arduino.h"
#include <Keypad.h>
#include "TouchSlider.h"

#define VERSION "1.0.0-rc.6"

const int MIDI_CHANNEL = 1;

// Musical settings
const uint8_t SCALE[] = { 49,51,54,56,58,61,63,66,68,70 };
const uint8_t SCALE_OFFSET_FROM_C3[] { 1,3,6,8,10,13,15,18,20,22 };

#define HIGH_SAMPLE_RATE 44100
#define LOW_SAMPLE_RATE 2489

#define INITIAL_VELOCITY 100

// Globals that should not be globals
int gate_length_msec = 40;

uint32_t sequencer_clock = 0;
// Sequencer settings
uint8_t current_step;
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
bool dfu_flag = 0;

uint32_t midi_clock = 0;
uint16_t audio_peak_values = 0UL;

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
NoteStack<10> note_stack;

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

#include "debug.h"

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

  usbMIDI.setHandleRealTimeSystem(midi_handle_realtime);

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
    // Crude hard coded task switching
    keys_scan(); // 14 or 175us (depending on debounce)
    keyboard_to_note();  
    pitch_update();  // ~30us 

    midi_handle();
    sequencer_update();

    pots_read(); // ~ 100us

    midi_handle();
    sequencer_update();

    synth_update(); // ~ 100us
    midi_send_cc();

    midi_handle();
    sequencer_update();

    drum_read(); // ~ 700us

    midi_handle();
    sequencer_update();

    led_update(); // ~ 2ms

    midi_handle();
    sequencer_update();
  }
}

void midi_handle_clock() {
  tempo_handler.midi_clock_received();
  midi_clock++;
}

void midi_handle_realtime(uint8_t type) {
  switch(type) {
      case 0xF8: // Clock
        midi_handle_clock();
        break;
      case 0xFA: // Start
        sequencer_reset_clock();
        sequencer_start();
        break;
      case 0xFC: // Stop
        sequencer_stop();
        break;
      case 0xFB: // Continue
        sequencer_start();
        break;
      case 0xFE: // ActiveSensing
      case 0xFF: // SystemReset
        break;
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

  synth.crush = !digitalRead(BITC_PIN);

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
                    FastLED.clear();
                    physical_leds[0] = CRGB::Blue;
                    FastLED.show();
                    dfu_flag = 1;                  
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
                } else if (k == SEQ_START) {
                  #ifdef DEV_MODE
                    if(dfu_flag == 1) {
                      enter_dfu();
                    }
                  #endif
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
  synth.amplitude = muxAnalogRead(AMP_POT);
  synth.pulseWidth = muxAnalogRead(OSC_PW_POT);
  synth.resonance = muxAnalogRead(FILTER_RES_POT);
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
