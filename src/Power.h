#ifndef Power_h
#define Power_h

void amp_update();
void power_off();
void power_on();
void amp_enable();
void amp_disable();
void headphone_disable();
void headphone_enable();

bool power_check();

bool power_flag = 1;
bool amp_enabled = 0;

elapsedMillis peak_update_time;
const unsigned int peak_interval = 5;

void power_init() {

}

bool power_check() {
  if(!power_flag) {
    analogWrite(ENV_LED,0);
    analogWrite(FILTER_LED,0);
    analogWrite(OSC_LED,0);
    if(keys_scan_powerbutton()) {
      power_on();
    } else {
      delay(100);
    }
  } else {
    amp_update();
  }
  return power_flag;
}

void power_off() {
  sequencer_stop();
  MIDI.sendControlChange(123,0,MIDI_CHANNEL);
  tempo_handler.reset_clock_source();
  transpose = 0;
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

/*
  Checks whether the audio amp needs to be on.
  This prevents idle noise from the speaker
 */
void amp_update() {
  if(peak_update_time > peak_interval) {

    audio_peak_values <<= 1;
    
    if(peak2.available()) {
      if(peak2.read() > 0.001f) {
        audio_peak_values |= 1UL;
      } else {
        audio_peak_values &= ~1UL;
      }
    } else {
      audio_peak_values &= ~1UL;
    }

    if((audio_peak_values == 0UL)) {
      amp_disable();
    } else {
      amp_enable();
    }
  }
}
void amp_enable() {
  if(amp_enabled == 0) {
    amp_enabled = 1;
    pop_suppressor.fadeIn(5.0);
  }
  if(digitalRead(JACK_DETECT)) {
    pinMode(AMP_ENABLE, INPUT);
  } else {
    pinMode(AMP_ENABLE, OUTPUT);
    digitalWrite(AMP_ENABLE, LOW);
  }
}
void amp_disable() {
  if(amp_enabled == 1) {
    pop_suppressor.fadeOut(5.0);
    pinMode(AMP_ENABLE, INPUT);
    amp_enabled = 0;
  }
}

void power_on() {
  amp_disable();
  midi_clock = 0;
  in_setup = true;
  // Clear note stack
  note_stack.Clear();
  // Read the MIDI channel from EEPROM. Lowest four bits
  uint8_t stored_midi_channel = eeprom_read_byte(EEPROM_MIDI_CHANNEL) & 0xf00;
  midi_set_channel(stored_midi_channel);
  keys_scan();
  if(midi_get_channel() != stored_midi_channel) {
    eeprom_write_byte(EEPROM_MIDI_CHANNEL, midi_get_channel());
  }
  in_setup = false;
  led_init();
  AudioInterrupts();
  amp_enable();
  power_flag = 1;
}

void headphone_disable() {
  digitalWrite(HP_ENABLE, LOW);
}

void headphone_enable() {
  digitalWrite(HP_ENABLE, HIGH);
}

#endif