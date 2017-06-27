#include "Arduino.h"
#include "pinmap.h"
#include <MIDI.h>
#include "Leds.h"

MIDI_CREATE_DEFAULT_INSTANCE();

#define MIDI_RETRY_COUNT 10

int message_received = 0;
void midi_note_on(uint8_t channel, uint8_t note, uint8_t velocity);
void headphone_enable();
void headphone_disable();
int sync_test();
bool keys_scan_powerbutton();
bool midi_ok = false;
void enter_dfu();

void setup() {
  Serial.begin(57600);
  pinMode(AMP_ENABLE, INPUT);

  MIDI.setHandleNoteOn(midi_note_on);
  MIDI.begin();
  MIDI.turnThruOff();
  pins_init();
  headphone_enable();
  led_init();

  // The output is AC coupled, so ramp to half of full scale
  for(int i =0; i < 127; i++) {
    analogWrite(A14, i);
    delay(1);
  }

}

void loop() {
  while(!keys_scan_powerbutton()) {
    FastLED.clear();
    FastLED.show();
    delay(100);
    physical_leds[0] = CRGB::Yellow;
    FastLED.show();
    delay(100);

    if(!digitalRead(BITC_PIN) && !digitalRead(ACCENT_PIN)) {
      enter_dfu();
    }
  }
  midi_ok = false;
  FastLED.clear();
  FastLED.show();

  Serial.println("Sending MIDI");
  message_received = 0;
  MIDI.sendNoteOn(63,64,1);
  MIDI.sendNoteOff(63,64,1);

  for(int i = 0; i < 100; i++) {
    delay(10);
    MIDI.read();
    physical_leds[i/10+9] = CRGB::Red;
    FastLED.show();
    if(message_received) {
      Serial.println("OK");
      for(int j = 0; j < 10; j++) {
        physical_leds[j+9] = CRGB::Green;
      }
      midi_ok = true;
      break;
    }
  }
  if(!message_received) {
    Serial.println("No note received");
    for(int j = 0; j < 10; j++) {
        physical_leds[j+9] = CRGB::Red;
    }
  } 
  FastLED.show();
  Serial.println();

  Serial.println("Checking sync...");
  int sync_ok = sync_test();

  Serial.println();

  if(sync_ok && midi_ok) {
    Serial.println("Sync and MIDI OK!");
  }
  delay(800);

}

int sync_test() {
  int sync_received_count = 0;
  const int sync_interval_ms = 100;
  // Go from 0.1 to 3.3V in 0.1V steps (assuming the voltage reference is 3.3V)
  for(float i = 3.3f; i > 0.0f; i -= 0.2f) {
    const int pulse_length_ms = 12;
    unsigned long pulse_timeout = millis() + pulse_length_ms;
    int sync_received = 0;
    float v = (127.0f/3.3f)*i;

    analogWrite(A14,v);

    while(millis() < pulse_timeout) {
      if(!digitalRead(SYNC_IN) && !sync_received) {
        leds((int)(i/0.4f)) = CRGB::Green;
        Serial.print("Received sync at ");
        Serial.print(3.3f-i);
        Serial.println("V");
        sync_received_count++;
        sync_received = 1;
      } 
      delay(10);
    } 

    if(!sync_received) {
      leds((int)(i/0.4f)) = CRGB::Red;
      Serial.print("No sync at ");
      Serial.print(3.3f-i);
      Serial.println("V");
    }

    FastLED.show();
    analogWrite(A14,127); // The output is AC coupled, so we rest at half of full scale

    delay(sync_interval_ms - pulse_length_ms);
  }
  return sync_received_count;
}

void midi_note_on(uint8_t channel, uint8_t note, uint8_t velocity) {
  Serial.printf("Note: channel %d, note %d, velocity %d\n", channel,note,velocity);
  if(channel == 1 && note == 63 && velocity == 64) {
    message_received = 1;
  }
}

void headphone_disable() {
  digitalWrite(HP_ENABLE, LOW);
}

void headphone_enable() {
  digitalWrite(HP_ENABLE, HIGH);
}

bool keys_scan_powerbutton() {
  bool r = false;

  pinMode(row_pins[powerbutton_row],INPUT_PULLUP);
  pinMode(col_pins[powerbutton_col],OUTPUT);
  digitalWrite(col_pins[powerbutton_col],LOW);
  r = (digitalRead(row_pins[powerbutton_row]) == LOW);

  return r;
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