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

void power_off() { // TODO: this is super crude and doesn't work, but it shows the effect
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
  if(power_flag) {
    if(peak_update_time < millis()) {

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

      if((audio_peak_values == 0UL || digitalRead(JACK_DETECT))) {
        amp_disable();
        peak_update_time = millis() + 5; // We want to wake up quickly
      } else {
        amp_enable();
        peak_update_time = millis() + 200; // We don't want to go to sleep too fast
      }
    }
  }
}

void power_on() {
  midi_clock = 0;
  led_init();
  AudioInterrupts();
  amp_enable();
  power_flag = 1;
}

void amp_enable() {
  if(amp_enabled == 0) {
    pinMode(AMP_ENABLE, OUTPUT);
    digitalWrite(AMP_ENABLE, LOW);
    amp_enabled = 1;
  }
}

void amp_disable() {
  if(amp_enabled == 1) {
    pinMode(AMP_ENABLE, INPUT);
    amp_enabled = 0;
  }
}

void headphone_disable() {
  digitalWrite(HP_ENABLE, LOW);
}

void headphone_enable() {
  digitalWrite(HP_ENABLE, HIGH);
}

#endif