#include "Arduino.h"
#include "pinmap.h"
#include <MIDI.h>

MIDI_CREATE_DEFAULT_INSTANCE();
#define MIDI_RETRY_COUNT 10
int message_received = 0;
void midi_note_on(uint8_t channel, uint8_t note, uint8_t velocity);
void headphone_enable();
void headphone_disable();
int sync_test();
bool midi_ok = false;

void setup() {
  Serial.begin(57600);
  MIDI.begin(MIDI_CHANNEL_OMNI);
  pins_init();
  headphone_enable();

  // The output is AC coupled, so ramp to half of full scale
  for(int i =0; i < 127; i++) {
    analogWrite(A14, i);
    delay(1);
  }

}

void loop() {
  midi_ok = false;

  Serial.println("Sending MIDI");
  message_received = 0;
  MIDI.sendNoteOn(63,64,1);
  MIDI.read();
  
  for(int i = 0; i < 1000; i++) {
    MIDI.read();
    delay(3);
  }
  if(!message_received) {
    Serial.println("no note received");
  } else {
    Serial.println("OK");
    midi_ok = true;
  }
  Serial.println();

  Serial.println("Checking sync...");
  int sync_ok = sync_test();

  Serial.println();

  if(sync_ok && midi_ok) {
    Serial.println("Sync and MIDI OK!");
  }
  delay(1000);
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
        Serial.print("Received sync at ");
        Serial.print(3.3f-i);
        Serial.println("V");
        sync_received_count++;
        sync_received = 1;
      } 
      delay(10);
    } 

    if(!sync_received) {
      Serial.print("No sync at ");
      Serial.print(3.3f-i);
      Serial.println("V");
    }

    analogWrite(A14,127); // The output is AC coupled, so we rest at half of full scale

    delay(sync_interval_ms - pulse_length_ms);
  }
  return sync_received_count;
}

void midi_note_on(uint8_t channel, uint8_t note, uint8_t velocity) {
  Serial.printf("Channel %d, note %d, velocity %d\n", channel,note,velocity);
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