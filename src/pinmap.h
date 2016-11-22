#define BRAINS_SEP
/*
Port  Num     Arduino pin number
PTA4          33
PTA5          24
PTA12          3
PTA13          4
PTB0          16
PTB1          17
PTB2          19
PTB3          18
PTB16          0
PTB17          1
PTB18         32
PTB19         25
PTC0          15
PTC1          22
PTC2          23
PTC3           9
PTC4          10
PTC5          13
PTC6          11
PTC7          12
PTC8          28
PTC9          27
PTC10         29
PTC11         30
PTD0           2
PTD1          14
PTD2           7
PTD3           8
PTD4           6
PTD5          20
PTD6          21
PTD7           5
PTE0          31
PTE1          26

*/

#ifdef BRAINS_SEP
  #define BTN_SYN1 3 //PTA12
  #define BTN_SYN2 9
  #define BTN_SYN3 24
  #define BTN_SYN4 20

  const int OSC_LED = 32;
  const int FILTER_LED = 25;
  const int ENV_LED = 15;

  const int TEMPO_POT = A16; // PTC9
  const int GATE_POT = A18; // PTC10

  const int FILTER_RES_POT = A8;
  const int OSC_DETUNE_POT = A13;  
  const int OSC_PW_POT = A9;

  const int AMP_ENV_POT = A10;

  const int FILTER_FREQ_POT = A12;

  const int FADE_POT = A11; 

  const int BITC_PIN = BTN_SYN3; // Lower left button
  const int NOISE_PIN = BTN_SYN4; // Lower right button

  const int SYNC_OUT_PIN = 4; // PTA13
  const int AMP_ENABLE = 31; // PTE0
  #define LED_DT 5
  #define COLOR_ORDER GRB
  #define LED_TYPE SK6812
  #define NUM_LEDS 19
  const int led_order[NUM_LEDS] = {1,2,3,4,5,6,7,8};
#endif
// Keypad matrix definitions are in Buttons.h

void pins_init() {
  pinMode(BITC_PIN, INPUT_PULLUP);
  pinMode(NOISE_PIN, INPUT_PULLUP);

  pinMode(BTN_SYN1, INPUT_PULLUP);
  pinMode(BTN_SYN2, INPUT_PULLUP);
  pinMode(BTN_SYN3, INPUT_PULLUP);
  pinMode(BTN_SYN4, INPUT_PULLUP);

  pinMode(SYNC_OUT_PIN, OUTPUT);
  pinMode(AMP_ENABLE, OUTPUT);

  pinMode(OSC_LED, OUTPUT);
  pinMode(FILTER_LED, OUTPUT);
  pinMode(ENV_LED, OUTPUT);
}
