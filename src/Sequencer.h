#ifndef Sequencer_h
#define Sequencer_h

//Initial sequencer values
uint8_t step_note[] = { 1,0,6,9,0,4,0,5 };
uint8_t step_enable[] = { 1,0,1,1,1,1,0,1 };
uint8_t step_velocity[] = { 100,100,100,100,100,100,100,100 };

void sequencer_start();
void sequencer_stop();
void sequencer();
void sequencer_trigger_note();
void sequencer_untrigger_note();
bool sequencer_is_running = false;

uint32_t next_step_time = 0;
uint32_t gate_off_time = 0;
uint32_t sync_off_time = 0;
uint32_t note_on_time;
uint32_t previous_note_on_time;
uint32_t note_off_time;

const uint8_t SEQUENCER_NUM_STEPS = 8;

void sequencer_start() {
  sequencer_is_running = true;
}

void sequencer_stop() {
  sequencer_is_running = false;
  note_off();
}

void sequencer() {

  if(!double_speed) {
    tempo_interval = tempo_interval_msec();
  } else {
    tempo_interval = (tempo_interval_msec()/2);
  }

  // Make sure the gate length is never longer than one step
  if(gate_length_msec > tempo_interval) {
    gate_length_msec = tempo_interval;
  }

  note_on_time = previous_note_on_time + tempo_interval;

  if(millis() >= note_on_time)  {
    if(!note_is_triggered) {
      sequencer_trigger_note();
      digitalWrite(SYNC_OUT_PIN, HIGH);
      sync_off_time = millis() + SYNC_LENGTH_MSEC;
    }
  }

  if(millis() >= sync_off_time) {
    digitalWrite(SYNC_OUT_PIN, LOW);
  }

  if(!note_is_played && millis() >= note_off_time) { // We want to update the gate length continuously responding to pot readings
    sequencer_untrigger_note();
  }
}

void sequencer_trigger_note() {
  note_is_triggered = true;
  note_is_played = false;
  previous_note_on_time = millis();
  note_off_time = previous_note_on_time + gate_length_msec;

  if (!next_step_is_random && !random_flag) {
    current_step++;
    if (current_step >= SEQUENCER_NUM_STEPS) current_step = 0;
  } else {
    random_flag = false;
    int random_step = random(SEQUENCER_NUM_STEPS);

    while (random_step == current_step || random_step == current_step+1 || random_step+SEQUENCER_NUM_STEPS == current_step) { // Prevent random from stepping to the current or next step
      random_step = random(SEQUENCER_NUM_STEPS);
    }

    current_step = random_step;
  }
  step_velocity[current_step] = INITIAL_VELOCITY;

  note_on(SCALE[step_note[current_step]]+transpose, step_velocity[current_step], step_enable[current_step]);
}

void sequencer_untrigger_note() {
  note_is_played = true;
  note_off();
  note_is_triggered = false;
  target_step = current_step + 1;
  if (target_step >= SEQUENCER_NUM_STEPS) target_step = 0;
  note_off_time = millis() + tempo_interval - gate_length_msec; // Set note off time to sometime in the future
}

#endif