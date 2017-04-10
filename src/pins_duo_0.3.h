#define BRAINS_SEP

const int FILTER_RES_POT = A8;
const int OSC_DETUNE_POT = A13;  
const int OSC_PW_POT = A9;
const int AMP_ENV_POT = A10;
const int FILTER_FREQ_POT = A12;
const int FADE_POT = A11; 

#define POT_SYN1 A8
#define POT_SYN2 A13

#define SW_SYN3  SW3
#define POT_SYN3 OSC_PW_POT

#define SW_SYN4  SW4

#define POT_SYN4 AMP_ENV_POT
#define POT_SYN5 FILTER_FREQ_POT
#define POT_SYN6 FADE_POT

const int SW1 = 3;
const int SW2 = 9;
const int SW3 = 24;
const int SW4 = 20;

const int TOUCH1 = 16;
const int TOUCH2 = 17;
const int TOUCH3 = 19;
const int TOUCH4 = 18;

const int ACCENT_PIN = SW1;
const int SLIDE_PIN = SW2;
const int BITC_PIN = SW3;
const int DELAY_PIN = SW4;

const int LED_1 = 32;
const int LED_2 = 25;
const int LED_3 = 15;

const int POT_1 = A16;
const int POT_2 = A18;

const int SYNC_OUT_PIN = 4;
const int AMP_ENABLE = 31;
const int LED_DATA = 5;

const int BTN_SYN2 = SW2;

const int OSC_LED = LED_1;
const int FILTER_LED = LED_2;
const int ENV_LED = LED_3;

const int TEMPO_POT = POT_1;
const int GATE_POT = POT_2;

// Non existent in sept version
const int HP_ENABLE = 31;
const int SYNC_IN = 33;
const int JACK_DETECT = 31;
const int SYNC_DETECT = 21; // Make sure this reads low
const int PROG = 31;
//const int LED_DATA = 5;

#define COLOR_ORDER GRB
#define LED_TYPE SK6812

const int NUM_LEDS = 19;
const int led_order[NUM_LEDS] = {1,2,3,4,5,6,7,8};

// const int SLIDE_PIN = 1;
// const int DELAY_PIN = 2;
// const int AMP_ENV_POT = 4;
// const int FILTER_RES_POT = 0;

// const int FILTER_FREQ_POT = 5;

// const int OSC_DETUNE_POT = 3;  

// const int OSC_PW_POT = 7;

// const int FADE_POT = 6; 

int muxAnalogRead(uint8_t channel) { 
  delayMicroseconds(4);
  return analogRead(channel);
}

uint8_t muxDigitalRead(uint8_t channel) { 
  pinMode(channel, INPUT_PULLUP);
  delayMicroseconds(4);
  //Wait a few microseconds for the selection to propagate. 
  //Less than 3 seems not to work so let's take 4 to be sure
  return digitalRead(channel);
}

void pins_init() {
  // PROG pin is unused but connected to reset via a resistor. Make output to be sure.
  pinMode(PROG, INPUT);

  pinMode(BITC_PIN, INPUT_PULLUP);
  pinMode(ACCENT_PIN, INPUT_PULLUP);

  pinMode(SYNC_OUT_PIN, OUTPUT);
  // pinMode(AMP_ENABLE, OUTPUT);
  pinMode(HP_ENABLE, OUTPUT);

  pinMode(JACK_DETECT, INPUT);
  pinMode(SYNC_DETECT, INPUT_PULLDOWN);

  randomSeed(analogRead(A15));

}