#include "Arduino.h"
#include <Keypad.h>
#include "../src/TouchSlider.h"
#include "pinmap.h"
#include "Touch.h"
#include "../src/Buttons.h"
#include "Leds.h"
#include "Audio.h"
#include <Midi.h>

#define VERSION "0.2.0"

MIDI_CREATE_DEFAULT_INSTANCE();

void keys_scan();
void pots_read();
void seq_leds();
void drum_init();
void midi_init();
void drum_read();
void enter_dfu();
void potRead();
void top_leds(CRGB c);
void touch_read();
void led_chaser();
void sync_duplicate();

int pot_low_values[] = {900,900,900,900,900,900,900,900};
int pot_high_values[] = {100,100,100,100,100,100,100,100};

uint8_t current_pot = 10;
bool start_flag = 0;

void setup() {

  pins_init();
  led_init();
  midi_init();

  button_matrix_init();
  drum_init();
  touch_init();
  
  Serial.begin(57600);
  Serial.print("Dato DUO selftest");
  Serial.println(VERSION);

  audio_init();

}

void loop() {
  keys_scan(); // 14 or 175us (depending on debounce)
  sync_duplicate();
  MIDI.read();

  if(start_flag) {
    drum_read();
    audio_stop();
    pots_read(); // ~ 100us
    touch_read();
    led_update(); // ~ 2ms
    delay(10);
  } else {
    led_chaser();
    if(digitalRead(JACK_DETECT)) {
      sine1.amplitude(1.0);
      amp_disable();
    } else {
      sine1.amplitude(0.3);
      amp_enable();
    }

  }
}

void pots_read() {
  const int THRESHOLD = 2;
  const int NUM_POTS = 8;
  int current_value = 0;

  // We need to store eight pot values and compare them
  for(int i = 0; i < NUM_POTS; i++) {
    current_pot = 10;
    int p = potRead(i);

    if(p < pot_low_values[i]) {
      current_pot = i;
      physical_leds[current_pot] = CRGB::Black;
      pot_low_values[i] = p;
      current_value = p;
    } else if(p > pot_high_values[i]) {
      current_pot = i;
      pot_high_values[i] = p;
      current_value = p;
    }

    if(current_pot < 10) {
      for(int j = 0; j < 8; j++) {
        if(j > (current_value>>7)) {
          leds(j) = CRGB::Black;
        } else {
          leds(j) = CRGB::White;
        }
      }
      physical_leds[current_pot+9] = CRGB::Yellow;
    } 
    if(pot_high_values[i] > 1000 && pot_low_values[i] < 10) {
      physical_leds[i+9] = CRGB::Green;
    } 
  }

  analogWrite(ENV_LED, potRead(4));
  analogWrite(FILTER_LED, potRead(5));
  analogWrite(OSC_LED, potRead(7));
}

void touch_read() {
  if(touchRead(TOUCH_1) < 1000) {
     physical_leds[NUM_LEDS-1] = CRGB::Red;
  } else {
    physical_leds[NUM_LEDS-1].setHue((touchRead(TOUCH_1)%200)+32);
  }

   if(touchRead(TOUCH_3) < 1100) {
     physical_leds[NUM_LEDS-2] = CRGB::Red;
  } else {
    physical_leds[NUM_LEDS-2].setHue((touchRead(TOUCH_3)%200)+32);
  }
}

void top_leds(CRGB c) {
  physical_leds[NUM_LEDS-2] = c;
  physical_leds[NUM_LEDS-1] = c;
  FastLED.show();
  delay(100);
  physical_leds[NUM_LEDS-2] = CRGB::Black;
  physical_leds[NUM_LEDS-1] = CRGB::Black;
}

void seq_leds(CRGB c) {
  for(int i = 0; i < 8; i++) {
    physical_leds[i+1] = c;
  }
  FastLED.show();
  delay(100);
  for(int i = 0; i < 8; i++) {
    physical_leds[i+1] = CRGB::Black;
  }
  FastLED.show();
}

void seq_left_leds(CRGB c) {
  for(int i = 0; i < 4; i++) {
    physical_leds[i+5] = c;
  }
  FastLED.show();
  delay(100);
  for(int i = 0; i < 4; i++) {
    physical_leds[i+5] = CRGB::Black;
  }
  FastLED.show();
}

void seq_right_leds(CRGB c) {
  for(int i = 0; i < 4; i++) {
    physical_leds[i+1] = c;
  }
  FastLED.show();
  delay(100);
  for(int i = 0; i < 4; i++) {
    physical_leds[i+1] = CRGB::Black;
  }
  FastLED.show();
}
void led_chaser() {
  static int j = 0;
  if(j > NUM_LEDS) {
    j = 0;
  }
  for(int i = 0; i < NUM_LEDS; i++) {
    physical_leds[i] = CRGB::White;
  }
  physical_leds[j] = CRGB::Black;
  FastLED.show();
  delay(200);
  FastLED.clear();
  j++;
}

// Scans the button_matrix and handles step enable and keys
void keys_scan() {
  if(!muxDigitalRead(1)) {
    seq_right_leds(CRGB::Blue);
  }
  if(!digitalRead(ACCENT_PIN)) {
    seq_left_leds(CRGB::Blue);
  }
  if(!muxDigitalRead(2)) {
    seq_right_leds(CRGB::Green);
  }
  if(!digitalRead(BITC_PIN)) {
    seq_left_leds(CRGB::Green);
  }

  if (button_matrix.getKeys())  {
    for (int i=0; i<LIST_MAX; i++) {
      if ( button_matrix.key[i].stateChanged ) {
        char k = button_matrix.key[i].kchar;
        switch (button_matrix.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
            case PRESSED:   
                if (k == SEQ_START) {
                  start_flag = 1-start_flag;
                } else if (k <= KEYB_9 && k >= KEYB_0) {
                  
                } else if (k == BTN_SEQ1) {
                  seq_left_leds(CRGB::Yellow);
                } else if(k == BTN_SEQ2) {
                  seq_right_leds(CRGB::Yellow);
                } else if (k == BTN_DOWN){
                  seq_left_leds(CRGB::Red);
                } else if(k == BTN_UP) {
                  seq_right_leds(CRGB::Red);
                }
                break;
            case HOLD:
                if (k == SEQ_START) {
                  enter_dfu();
                } 
                break;
            case RELEASED:
                if (k <= KEYB_9 && k >= KEYB_0) {
                 
                } else if (k == BTN_SEQ2) {
                  
                } 
                break;
            case IDLE:
                if (k <= KEYB_9 && k >= KEYB_0) {
                  
                }
                break;
        }
      }
    }
  } 
}

void midi_init() {
  MIDI.begin(MIDI_CHANNEL_OMNI);
  // MIDI loopback from in to out (soft THRU) is enabled by default
}

void sync_duplicate() {
  if(digitalRead(SYNC_DETECT)) {
    digitalWrite(SYNC_OUT, digitalRead(SYNC_IN));
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