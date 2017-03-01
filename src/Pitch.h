/*
  handles oscillator 1 pitch, oscillator 2 pitch
  also does pitch bend and glide

  oscillator 1 is the master pitch. It's a pulse wave with variable pulse width
  Oscillator 2 can be detuned in steps
*/
const uint8_t DETUNE_OFFSET_SEMITONES[] = { 3,4,5,7,9 };