
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
  const int TOUCH2 = 18;
  const int TOUCH3 = 17;
  const int TOUCH4 = 19;

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