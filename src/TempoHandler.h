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
#define TEMPO_SYNC_DIVIDER   12
#define TEMPO_SYNC_PULSE_MS  12
 
class TempoHandler 
{
  public:
    TempoHandler(){
      last_millis = millis();
    }

    inline void setHandleTempoEvent(void (*fptr)()) {
      tTempoCallback = fptr;
    }
    inline void setHandleAlignEvent(void (*fptr)()) {
      tAlignCallback = fptr;
    }
    void setPPQN(int ppqn) {
      _ppqn = ppqn;
    }
    void update() {
      // Determine which source is selected for tempo
      if((pinRead(SYNC_DETECT_PIN))) {
        if(_source != TEMPO_SOURCE_SYNC) {
          _source = TEMPO_SOURCE_SYNC;
        }
        _midi_clock_received_flag = 0;
      } else if (_midi_clock_received_flag) { 
        if(_source != TEMPO_SOURCE_MIDI) {
          _source = TEMPO_SOURCE_MIDI;
        }
      } else {
        if(_source != TEMPO_SOURCE_INTERNAL) {
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

      if(syncStart >= TEMPO_SYNC_PULSE_MS) {
        Sync::write(LOW);
      }
    }
    void midi_clock_received() {
      _midi_clock_received_flag = 1;
    }
    void midi_clock_reset() {
      midi_clock = 0;
      _previous_midi_clock = 0;
    }
    void reset_clock_source() {
      _midi_clock_received_flag = 0;
      _source = TEMPO_SOURCE_INTERNAL;
    }
    bool is_clock_source_internal() {
      return _source == TEMPO_SOURCE_INTERNAL;
    }
  private:
    elapsedMillis syncStart;
    void (*tTempoCallback)();
    void (*tAlignCallback)();
    int pot_pin;
    uint8_t _source = 0;
    uint32_t _previous_clock_time;
    uint32_t _previous_sync_time;
    uint32_t _tempo_interval;
    bool _midi_clock_block = false;
    uint32_t _previous_midi_clock = 0;
    bool _midi_clock_received_flag = 0;
    uint16_t _clock = 0;
    uint16_t _ppqn = 24;
    uint32_t accum = 0;
    uint32_t last_millis = 0;


    void update_midi() { 
      if(midi_clock != _previous_midi_clock) {
        _previous_midi_clock = midi_clock;
        _previous_clock_time = micros();
        trigger();
      }
    }
    void update_sync() {
      static uint8_t _sync_pin_previous_value = 1;
      uint8_t _sync_pin_value = Sync::read();

      if(_sync_pin_previous_value && !_sync_pin_value) {
        _tempo_interval = (micros() - _previous_sync_time) / TEMPO_SYNC_DIVIDER;
        _clock = 0;
        _previous_sync_time = micros();
        _previous_clock_time = micros();
        if (tAlignCallback != 0) {
          tAlignCallback();
        }
        if (tTempoCallback != 0) {
          trigger();
        }
      } else {
        if(micros() - _previous_clock_time > _tempo_interval) {
          if(_clock < TEMPO_SYNC_DIVIDER) {
            if (tTempoCallback != 0) {
              _previous_clock_time = micros();
              trigger();
            }
          }
        }
      }
      _sync_pin_previous_value = _sync_pin_value;
    }

    #define BPM_TO_MILLIS(bpm) (2500000 / bpm)
    void update_internal() {
      int potvalue = synth.speed;
      uint32_t scaled_millis_per_beat; // 2 x beats per minute

      if(potvalue < 128) {
        scaled_millis_per_beat = map(potvalue,0,128,BPM_TO_MILLIS(30),BPM_TO_MILLIS(60));
      } else if(potvalue < 895) {
        scaled_millis_per_beat = map(potvalue, 128,895,BPM_TO_MILLIS(60),BPM_TO_MILLIS(200));
      } else {
        // For Toon: 603 BPM in gives 600 BPM out
        scaled_millis_per_beat = map(potvalue, 895,1023,BPM_TO_MILLIS(200),BPM_TO_MILLIS(603));
      }

      const auto cur = millis();
      // Multiply by 100 as a scaling factor for millis_per_beat
      // and a factor of 10 to match original Duo
      accum += (cur - last_millis)*1000;
      last_millis = cur;
      
      while(accum >= scaled_millis_per_beat)  {
        accum -= scaled_millis_per_beat;
        _previous_clock_time = micros();
        trigger();
      }
    }

    /*
     * Calls the callback, updates the clock and sends out MIDI/Sync pulses
     */
    void trigger() {
      MIDI.sendRealTime(midi::Clock);
      usbMIDI.sendRealTime(midi::Clock);

      if((_clock % _ppqn) == 0) {
        _clock = 0;
      }
      if((_clock % TEMPO_SYNC_DIVIDER) == 0) {
        Sync::write(HIGH);
        syncStart = 0;
      } 
      if (tTempoCallback != 0) {
        tTempoCallback();
      }
      _clock++;
    }
};

#endif
