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

 Does the tempohandler handle sync source switching as well? If so
 it needs to know about pins

 Are we going to pass the tempo pot pin value? Or the tempo pot pin number?

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
    int pot_pin;
    uint8_t _source = 0;
    uint8_t _sync_pin_previous_value = 1;
    const int TEMPO_MIN_INTERVAL_MSEC = 666; // Tempo is actually an interval in ms
    const int TEMPO_MAX_INTERVAL_MSEC = 40;
    uint32_t _clock_time;
    uint32_t _previous_clock_time;
    uint16_t _tempo_interval;
    bool _midi_clock_block = false;
    uint8_t _midi_divider;

    void update_midi() { 
      if(double_speed) {
        _midi_divider = 6; 
      } else {
        _midi_divider = 12;
      }
      if(midi_clock % _midi_divider == 0) {
        if (tTempoCallback != 0 && !_midi_clock_block) {
          _midi_clock_block = 1; // Block callback from triggering multiple times
          _previous_clock_time = millis();
          tTempoCallback();
        }
      } else {
        _midi_clock_block = 0;
      }
    }
    void update_sync() {
      uint8_t _sync_pin_value = digitalRead(SYNC_IN);

      if(_sync_pin_previous_value && !_sync_pin_value) {
        if (tTempoCallback != 0) {
          _previous_clock_time = millis();
          tTempoCallback();
        }
      }
      _sync_pin_previous_value = _sync_pin_value;
    }

    void update_internal() {
      // Read the position of the tempo pot
      int potvalue = analogRead(TEMPO_POT);
      _tempo_interval = map(potvalue,0,1023,TEMPO_MAX_INTERVAL_MSEC,TEMPO_MIN_INTERVAL_MSEC);
      if(double_speed) {
        _tempo_interval /= 2;
      }
      _clock_time = _previous_clock_time + _tempo_interval;

      if(millis() >= _clock_time)  {
        if (tTempoCallback != 0) {
          _previous_clock_time = millis();
          tTempoCallback();
        }
      }
    }
};

#endif