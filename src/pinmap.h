const int FILTER_RES_POT = A3;
const int AMP_ENV_POT = A4;
const int FILTER_FREQ_POT = A5;
const int OSC_DETUNE_POT = A6;
const int GATE_POT = A7;
const int TEMPO_POT = A8;
const int FADE_POT = A9; 

const int BITC_PIN = 15;
const int NOISE_PIN = 16;

const int SYNC_PIN = 1;

// Keypad matrix definitions are in Buttons.h


void pins_init() {
  pinMode(BITC_PIN, INPUT_PULLUP);
  pinMode(NOISE_PIN, INPUT_PULLUP);
  pinMode(SYNC_PIN, OUTPUT);
}