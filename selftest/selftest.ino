#include "Arduino.h"
#include <Keypad.h>
#include "../src/TouchSlider.h"
#include "pinmap.h"
#include "Touch.h"
#include "../src/Buttons.h"
#include "Leds.h"
#include "Audio.h"

#define VERSION "0.1.0"

void keys_scan();
void pots_read();
void drum_init();
void drum_read();
void enter_dfu();
void potRead();
void top_leds(CRGB c);
void led_chaser();
void sync_duplicate();

int pot_low_values[] = {1024,1024,1024,1024,1024,1024,1024,1024};
int pot_high_values[] = {0,0,0,0,0,0,0,0};

uint8_t current_pot = 0;
bool start_flag = 0;

void setup() {

  pins_init();
  led_init();

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
  if(start_flag) {
    drum_read();
    audio_stop();
    pots_read(); // ~ 100us
    led_update(); // ~ 2ms
    delay(10);
  } else {
    led_chaser();
  }
}

void pots_read() {
  const int THRESHOLD = 2;
  const int NUM_POTS = 8;

  // We need to store eight pot values and compare them
  // for(int i = 0; i < NUM_POTS; i++) {

  //   int p = potRead(i);
  //   if(p < pot_low_values[i]) {
  //     current_pot = i;
  //     physical_leds[i+9] = CRGB::White;
  //     pot_low_values[i] = p;
  //     for(int i = 0; i < 8; i++) {
  //       if(i > (p>>7)) {
  //         leds(i) = CRGB::Black;
  //       } else {
  //         leds(i) = CRGB::White;
  //       }
  //     }
  //   } else if(p > pot_high_values[i]) {
  //     current_pot = i;
  //     physical_leds[i+9] = CRGB::White;
  //     pot_high_values[i] = p;
  //     for(int i = 0; i < 8; i++) {
  //       if(i > (p>>7)) {
  //         leds(i) = CRGB::Black;
  //       } else {
  //         leds(i) = CRGB::White;
  //       }
  //     }

  //     if(pot_high_values[i] > 1000 && pot_low_values[i] < 10) {
  //       physical_leds[i+9] = CRGB::Green;
  //     }
  //   }
  // }
  if(kick_value) {
    for(int i = 0; i < 8; i++) {
      if(i > (kick_value>>4)) {
        leds(i) = CRGB::Black;
      } else {
        leds(i) = CRGB::White;
      }
    }
  }
  if(hat_value) {
    for(int i = 0; i < 8; i++) {
      if(i > (hat_value>>4)) {
        leds(i) = CRGB::Black;
      } else {
        leds(i) = CRGB::White;
      }
    }
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
  if(!muxDigitalRead(1) || !digitalRead(ACCENT_PIN)) {
    top_leds(CRGB::Blue);
  }
  if(!muxDigitalRead(2) || !digitalRead(BITC_PIN)) {
    top_leds(CRGB::Green);
  }

  if (button_matrix.getKeys())  {
    for (int i=0; i<LIST_MAX; i++) {
      if ( button_matrix.key[i].stateChanged ) {
        char k = button_matrix.key[i].kchar;
        switch (button_matrix.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
            case PRESSED:   
                if (k == SEQ_START) {
                  start_flag = 1;
                } else if (k <= KEYB_9 && k >= KEYB_0) {
                  
                } else if (k == BTN_SEQ1 || k == BTN_SEQ2) {
                  top_leds(CRGB::Yellow);
                } else if (k == BTN_UP || k == BTN_DOWN) {
                  top_leds(CRGB::Red);
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

void sync_duplicate() {
  if(digitalRead(SYNC_DETECT)) {
    digitalWrite(SYNC_OUT, digitalRead(SYNC_IN));
  } else {
    // pulse sync
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