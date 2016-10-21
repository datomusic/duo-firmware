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
PTE1          26 sdsd

*/

#ifdef BRAINS_SEP
  #define BTN_SYN1 9
  #define BTN_SYN2 3
  #define BTN_SYN3 24
  #define BTN_SYN4 20

  // POT4 PTC1 RES A8
  // POT5 PTC2 OSC A9
  // POT6 ADC0_DM3 OSC_DETUNE A13
  // POT7 ADC0_DP3 FILTER A12
  // POT8 ADC0_DM0 AMP
  // POT9 ADC0_DP0 ENV

  // LED1 32 LED2 25 LED3 15
  const int OSC_LED = 32;
  const int FILTER_LED = 25;
  const int ENV_LED = 15;

  const int TEMPO_POT = A16; // PTC9
  const int GATE_POT = A18; // PTC10

  // POT4 PTC1 RES A8
  // POT5 PTC2 OSC A9
  // POT6 ADC0_DM3 OSC_DETUNE A13
  // POT7 ADC0_DP3 FILTER A12
  // POT8 ADC0_DM0 AMP
  // POT9 ADC0_DP0 ENV

  const int FILTER_RES_POT = A8;
  const int OSC_DETUNE_POT = A9;
  const int OSC_PW_POT = A13;

  const int AMP_ENV_POT = A10;

  const int FILTER_FREQ_POT = A12;

  const int FADE_POT = A11;

  const int BITC_PIN = BTN_SYN3; // Lower left button
  const int NOISE_PIN = BTN_SYN4; // Lower right button

  const int ULEFT_PIN = BTN_SYN1; // Upper left button
  const int URIGHT_PIN = BTN_SYN2; // Upper right button

  const int SYNC_OUT_PIN = 4; // PTA13
  const int AMP_ENABLE = 31; // PTE0

  // SW1 PTA12 3
  // SW2 PTA5 24
  // SW3 PTC3 9
  // SW4 PTD5 20
#endif

// Keypad matrix definitions are in Buttons.h

void pins_init() {

  pinMode(BITC_PIN, INPUT_PULLUP);
  pinMode(ULEFT_PIN, INPUT_PULLUP);
  pinMode(URIGHT_PIN, INPUT_PULLUP);
  pinMode(NOISE_PIN, INPUT_PULLUP);
  pinMode(SYNC_OUT_PIN, OUTPUT);
  pinMode(AMP_ENABLE, OUTPUT);

  pinMode(OSC_LED, OUTPUT);
  pinMode(FILTER_LED, OUTPUT);
  pinMode(ENV_LED, OUTPUT);
}
