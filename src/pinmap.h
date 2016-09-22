
#ifdef BRAINS_FEB
  const int FILTER_RES_POT = A2;
  const int AMP_ENV_POT = A1;
  const int FILTER_FREQ_POT = A3;
  const int OSC_DETUNE_POT = A4;
  const int GATE_POT = A7;
  const int TEMPO_POT = A8;
  const int FADE_POT = A9; 

  const int BITC_PIN = 14;
  const int NOISE_PIN = 19;

  const int SYNC_PIN = 1;

#endif
#ifdef BRAINS_AUG
  const int FILTER_RES_POT = A12;
  const int AMP_ENV_POT = A13;
  const int FILTER_FREQ_POT = A17;
  const int OSC_DETUNE_POT = A6;
  const int GATE_POT = A10;
  const int TEMPO_POT = A11;
  const int FADE_POT = A7; 

  const int BITC_PIN = 24;
  const int NOISE_PIN = 25;

  const int SYNC_PIN = 1;

#endif

const int AMP_ENABLE = 31;
// Keypad matrix definitions are in Buttons.h


void pins_init() {
  pinMode(BITC_PIN, INPUT_PULLUP);
  pinMode(NOISE_PIN, INPUT_PULLUP);
  pinMode(SYNC_PIN, OUTPUT);
  pinMode(AMP_ENABLE, OUTPUT);
}