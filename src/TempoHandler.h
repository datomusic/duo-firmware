/*
 * Generates 24 pulses per quarter note based on one of the
 * following sources:
    - MIDI timecode packets
    - Sync pulses
    - Internal clock generator

    If the clock source is the internal clock, the speed is
    determined by the position of the TEMPO_POT. Tempo range is
    40ms to 666ms

    After 12 pulses the sequencer advances
    The Volca sync pulses have to be interpolated

 * Calls a callback function every time the timer advances
 * Sends MIDI timecode packets
 * Sends sync pulses
 */

#ifndef TempoHandler_h
#define TempoHandler_h

#define TEMPO_SOURCE_INTERNAL 0
#define TEMPO_SOURCE_MIDI     1
#define TEMPO_SOURCE_SYNC     2

class TempoHandler 
{
  public:
    inline void setHandleTempoEvent(void (*fptr)()) {
      tTempoCallback = fptr;
    }
    inline void setHandleResetEvent(void (*fptr)()) {
      tResetCallback = fptr;
    }
    void update() {
      // Determine which source is selected for tempo
      if(digitalRead(SYNC_DETECT)) {
        if(_source != TEMPO_SOURCE_SYNC) {
          Serial.println("Switching to SYNC as source");
          _source = TEMPO_SOURCE_SYNC;
        }
      } else if (midi_clock > 0) { // midi_clock is a global count of incoming midi clocks
        if(_source != TEMPO_SOURCE_MIDI) {
          Serial.println("Switching to MIDI as source");
          _source = TEMPO_SOURCE_MIDI;
        }
      } else {
        if(_source != TEMPO_SOURCE_INTERNAL) {
          Serial.println("Switching to INTERNAL as source");
          _source = TEMPO_SOURCE_INTERNAL;
        }
      }

      switch(_source) {
        case TEMPO_SOURCE_INTERNAL:
          update_internal();
          break;
        case TEMPO_SOURCE_MIDI:
          update_midi();
          break;
        case TEMPO_SOURCE_SYNC:
          update_sync();
          break;
      }
    }
    void midi_clock_reset() {
      if(_source == TEMPO_SOURCE_MIDI) {
        _midi_clock_block = 0;
        midi_clock = 11; // One less than our overflow, so the next MIDI clock tick triggers a callback
      }
    }
  private:
    void (*tTempoCallback)();
    void (*tResetCallback)();
    int pot_pin;
    uint8_t _source = 0;
    const unsigned int TEMPO_MAX_INTERVAL_USEC = 48000;
    const unsigned int TEMPO_MIN_INTERVAL_USEC = 3600;
    uint32_t _previous_clock_time;
    uint16_t _tempo_interval;
    bool _midi_clock_block = false;
    uint8_t _midi_divider;
    uint8_t _clock = 0;

    void update_midi() { 
      if(midi_clock % _midi_divider == 0) {
        if (!_midi_clock_block) {
          _midi_clock_block = 1; // Block callback from triggering multiple times
          _previous_clock_time = micros();
          trigger();
        }
      } else {
        _midi_clock_block = 0;
      }
    }
    void update_sync() {
      static uint8_t _sync_pin_previous_value = 1;
      uint8_t _sync_pin_value = digitalRead(SYNC_IN);

      if(_sync_pin_previous_value && !_sync_pin_value) {
        if (tTempoCallback != 0) {
          _previous_clock_time = micros();
          // TODO: Probably the worst way ever to convert Volca Sync to 24ppqn
          trigger();
        }
      }
      _sync_pin_previous_value = _sync_pin_value;
    }

    void update_internal() {
      int potvalue = analogRead(TEMPO_POT);
      _tempo_interval = map(potvalue,0,1023,TEMPO_MIN_INTERVAL_USEC,TEMPO_MAX_INTERVAL_USEC);

      if((micros() - _previous_clock_time) > _tempo_interval)  {
        _previous_clock_time = micros();
        trigger();
      }
    }

    /*
     * Calls the callback, updates the clock and sends out MIDI/Sync pulses
     */
    void trigger() {
      _clock++;
      MIDI.sendRealTime(midi::Clock);
      if((_clock % 24) == 0) {
        if (tResetCallback != 0) {
          tResetCallback();
        }
      }
      if((_clock % 12) == 0) {
        digitalWrite(SYNC_OUT_PIN, HIGH);
      } else if((_clock % 12) == 1) {
        digitalWrite(SYNC_OUT_PIN, LOW);
      }
      if (tTempoCallback != 0) {
        tTempoCallback();
      }
      if(_clock >= 24) { 
        _clock = 0;
      }
    }
};

#endif