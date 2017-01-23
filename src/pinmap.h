/*
 * pinmap.h maps the physical IO pins to software functions
 *
 * For code compatibility, we're using the Arduino pin numbers
 * as defined for the Teensy 3.2.
 *
 *   Port  Num     Arduino pin number
 *   PTA4          33
 *   PTA5          24
 *   PTA12          3
 *   PTA13          4
 *   PTB0          16
 *   PTB1          17
 *   PTB2          19
 *   PTB3          18
 *   PTB16          0
 *   PTB17          1
 *   PTB18         32
 *   PTB19         25
 *   PTC0          15
 *   PTC1          22
 *   PTC2          23
 *   PTC3           9
 *   PTC4          10
 *   PTC5          13
 *   PTC6          11
 *   PTC7          12
 *   PTC8          28
 *   PTC9          27
 *   PTC10         29
 *   PTC11         30
 *   PTD0           2
 *   PTD1          14
 *   PTD2           7
 *   PTD3           8
 *   PTD4           6
 *   PTD5          20
 *   PTD6          21
 *   PTD7           5
 *   PTE0          31
 *   PTE1          26
 */
 
#define BRAINS_SEP
#ifdef BRAINS_SEP
  
  const int FILTER_RES_POT = A8;
  const int OSC_DETUNE_POT = A13;  
  const int OSC_PW_POT = A9;
  const int AMP_ENV_POT = A10;
  const int FILTER_FREQ_POT = A12;
  const int FADE_POT = A11; 

  const int BTN_SYN1 = 3;
  const int BTN_SYN2 = 9;
  const int BTN_SYN3 = 24;
  const int BTN_SYN4 = 20;

  const int TOUCH1 = 16;
  const int TOUCH2 = 17;
  const int TOUCH3 = 19;
  const int TOUCH4 = 18;

  const int ACCENT_PIN = BTN_SYN1;
  const int SLIDE_PIN = BTN_SYN2;
  const int BITC_PIN = BTN_SYN3;
  const int DELAY_PIN = BTN_SYN4;

  const int OSC_LED = 32;
  const int FILTER_LED = 25;
  const int ENV_LED = 15;

  const int TEMPO_POT = A16;
  const int GATE_POT = A18;

  const int SYNC_OUT_PIN = 4;
  const int AMP_ENABLE = 31;
  const int LED_DATA = 5;

  #define COLOR_ORDER GRB
  #define LED_TYPE SK6812

  const int NUM_LEDS = 19;
  const int led_order[NUM_LEDS] = {1,2,3,4,5,6,7,8};

#endif
#ifdef SYNTH_JAN
  const uint8_t POT_SYN1 = 0;
  const uint8_t POT_SYN6 = 1;
  const uint8_t SW_SYN4 = 2;
  const uint8_t POT_SYN5 = 3;
  const uint8_t SW_SYN3 = 4;
  const uint8_t POT_SYN4 = 5;
  const uint8_t POT_SYN3 = 6;
  const uint8_t POT_SYN2 = 7;
#endif

//int muxAnalogRead(uint8_t channel) { 
//  digitalWrite(SYN_ADDR0, channel & (1<<0));
//  digitalWrite(SYN_ADDR1, channel & (1<<1));
//  digitalWrite(SYN_ADDR2, channel & (1<<2));
//  //do we need to wait a few nanoseconds?
//  return analogRead(MUX_IO);
//}
//
//uint8_t muxDigitalRead(uint8_t channel) { 
//  digitalWrite(SYN_ADDR0, channel & (1<<0));
//  digitalWrite(SYN_ADDR1, channel & (1<<1));
//  digitalWrite(SYN_ADDR2, channel & (1<<2));
//  //do we need to wait a few nanoseconds?
//  return digitalRead(MUX_IO);
//}

void pins_init() {

  pinMode(BITC_PIN, INPUT_PULLUP);
  pinMode(DELAY_PIN, INPUT_PULLUP);

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
