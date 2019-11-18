#include "Arduino.h"
#include <Keypad.h>
#include "../src/TouchSlider.h"
#include "pinmap.h"
#include "Touch.h"
#include "../src/Buttons.h"
#include "Leds.h"
#include "Audio.h"
#include <MIDI.h>

#define VERSION "0.5.0"
const uint8_t FIRMWARE_VERSION[] = { 0, 5, 0 };
/*
 changelog:
 0.5.0:
 - added sysex handling to reboot to bootloader

 0.4.0:
 - changed sine frequency from 170 to 233Hz
 - changed sine amplitude on speaker from .3 to .5
*/
#define SYSEX_DATO_ID 0x7D
#define SYSEX_UNIVERSAL_NONREALTIME_ID 0x7e
#define SYSEX_UNIVERSAL_REALTIME_ID 0x7f
#define SYSEX_DUO_ID 0x64
#define SYSEX_ALL_ID 0x7f

#define SYSEX_FIRMWARE_VERSION 0x01
#define SYSEX_SERIAL_NUMBER 0x02
#define SYSEX_REBOOT_BOOTLOADER 0x0B
#define SYSEX_SELFTEST 0x0A

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
void sync_pulse();
void midi_print_serial_number();
void midi_print_identity();
void midi_print_firmware_version();

int pot_low_values[] = {900,900,900,900,900,900,900,900};
int pot_high_values[] = {100,100,100,100,100,100,100,100};

uint8_t current_pot = 10;
bool start_flag = 0;
bool dfu_flag = 0;

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
  attachInterrupt(SYNC_IN,sync_pulse,CHANGE);
}

void loop() {
  keys_scan(); // 14 or 175us (depending on debounce)
  MIDI.read();
  if(start_flag) {
    drum_read();
    audio_stop();
    pots_read(); // ~ 100us
    touch_read();
    led_update(); // ~ 2ms
  } else {
    led_chaser();
    if(digitalRead(JACK_DETECT)) {
      sine1.amplitude(1.0);
      amp_disable();
    } else {
      sine1.amplitude(.5);
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

#define TOUCH_THRESHOLD 15
void touch_read() {

  static int previous_touch1 = (touchRead(TOUCH_1));
  int touch1 = (touchRead(TOUCH_1));

  if(touch1 > previous_touch1+TOUCH_THRESHOLD || touch1+TOUCH_THRESHOLD < previous_touch1) {
    physical_leds[NUM_LEDS-2].setHue(touch1%200+32);
  }

  if(touchRead(TOUCH_1) < 1000) {
     physical_leds[NUM_LEDS-2] = CRGB::Red;
  } 

  static int previous_touch3 = (touchRead(TOUCH_3)%200);
  int touch3 = (touchRead(TOUCH_3)%200);

  if(touch3 > previous_touch3+TOUCH_THRESHOLD || touch3+TOUCH_THRESHOLD < previous_touch3) {
    physical_leds[NUM_LEDS-1].setHue(touch3%200+32);
  }

  if(touchRead(TOUCH_3) < 1100) {
     physical_leds[NUM_LEDS-1] = CRGB::Red;
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
                  physical_leds[k-KEYB_0+9] = CRGB::HotPink;
                } else if (k <= STEP_8 && k >= STEP_1) {
                  physical_leds[k-STEP_1+1] = CRGB::HotPink;
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
                  FastLED.clear();
                  physical_leds[0] = CRGB::Blue;
                  FastLED.show();
                  dfu_flag = 1;                  
                } 
                break;
            case RELEASED:
                if (k <= KEYB_9 && k >= KEYB_0) {
                 
                } else if (k == BTN_SEQ2) {
                  
                } else if (k == SEQ_START) {
                  if(dfu_flag == 1) {
                    enter_dfu();
                  }
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

void sync_pulse() {
  if(digitalRead(SYNC_DETECT)) {
    digitalWrite(SYNC_OUT, digitalRead(SYNC_IN));
  }
}

void midi_usb_sysex(const uint8_t *data, uint16_t length, bool complete) {

  if(data[1] == SYSEX_DATO_ID && data[2] == SYSEX_DUO_ID) {
    switch(data[3]) {
      case SYSEX_FIRMWARE_VERSION:
        midi_print_firmware_version();
        break;
      case SYSEX_SERIAL_NUMBER:
        midi_print_serial_number();
        break;
      case SYSEX_SELFTEST:
        // enter_selftest();
        break;
      case SYSEX_REBOOT_BOOTLOADER:
        enter_dfu();
        break;
    }
  }
  if(data[1] == SYSEX_UNIVERSAL_NONREALTIME_ID) {
    if(data[2] == SYSEX_DUO_ID || data[2] == SYSEX_ALL_ID) {
      if(data[3] == 06 && data[4] == 01) { // General Information Identity Request
        midi_print_identity();
      }
    }
  }
}

void midi_print_identity() {

  uint8_t sysex[] = { 
    0xf0,
    0x7e,
    SYSEX_DUO_ID,
    0x06, // General Information (sub-ID#1)
    0x02, // Identity Reply (sub-ID#2)
    SYSEX_DATO_ID, // Manufacturers System Exclusive id code
    0x00, 0x00, // Device family code (14 bits, LSB first)
    0x00, 0x00, // Device family member code (14 bits, LSB first)
    FIRMWARE_VERSION[0], // Software revision level. Major version
    FIRMWARE_VERSION[1], // Software revision level. Minor version
    FIRMWARE_VERSION[2], // Software revision level. Revision
    0xf7 };

  usbMIDI.sendSysEx(sizeof(sysex), sysex);
}

void midi_print_serial_number() {
  // Serial number is sent as 4 groups of 5 7bit values, right aligned
  uint8_t sysex[24];

  sysex[0] = 0xf0;
  sysex[1] = SYSEX_DATO_ID;
  sysex[2] = SYSEX_DUO_ID;

  sysex[3] = (SIM_UIDH >> 28) & 0x7f;
  sysex[4] = (SIM_UIDH >> 21) & 0x7f;
  sysex[5] = (SIM_UIDH >> 14) & 0x7f;
  sysex[6] = (SIM_UIDH >> 7) & 0x7f;
  sysex[7] = (SIM_UIDH) & 0x7f;

  sysex[8]  = (SIM_UIDMH >> 28) & 0x7f;
  sysex[9]  = (SIM_UIDMH >> 21) & 0x7f;
  sysex[10] = (SIM_UIDMH >> 14) & 0x7f;
  sysex[11] = (SIM_UIDMH >> 7) & 0x7f;
  sysex[12] = (SIM_UIDMH) & 0x7f;

  sysex[13] = (SIM_UIDML >> 28) & 0x7f;
  sysex[14] = (SIM_UIDML >> 21) & 0x7f;
  sysex[15] = (SIM_UIDML >> 14) & 0x7f;
  sysex[16] = (SIM_UIDML >> 7) & 0x7f;
  sysex[17] = (SIM_UIDML) & 0x7f;

  sysex[18] = (SIM_UIDL >> 28) & 0x7f;
  sysex[19] = (SIM_UIDL >> 21) & 0x7f;
  sysex[20] = (SIM_UIDL >> 14) & 0x7f;
  sysex[21] = (SIM_UIDL >> 7) & 0x7f;
  sysex[22] = (SIM_UIDL) & 0x7f;

  sysex[23]= 0xf7;

  usbMIDI.sendSysEx(24, sysex);
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