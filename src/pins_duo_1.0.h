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
#define SEQ_0_4
#define POT_1       A10
#define POT_2       A11
#define MUX_IO       29
#define UNCONNECTED_ANALOG 13 //ADC0_DM3/A13 is unconnected
// DAC_OUT
#define SYN_ADDR0    33
#define SYN_ADDR1    24
#define SYN_ADDR2     3
#define HP_ENABLE     4
#define TOUCH_1      16
#define TOUCH_2      17
#define TOUCH_3      19
#define TOUCH_4      18
#define RX1           0
#define RX2           1
#define LED_DATA     32
#define LED_CLK      25
#define ROW_1        15
#define ROW_2        22
#define ROW_3        23
#define ROW_4         9
#define COL_1        10
#define COL_2        13
#define COL_3        11
#define COL_4        12
#define COL_5        28
#define COL_6        27
#define JACK_DETECT  26
#define SYNC_DETECT   2
#define SYNC_IN      14
#define SYNC_OUT      7
#define AMP_ENABLE    6
#define LED_1        20
#define LED_2        21
#define LED_3         5
#define SW1           8
#define SW2          30
#define PROG         31

const int BTN_SYN1 = SW1;
const int BTN_SYN2 = SW2;

const int TOUCH1 = TOUCH_1;
const int TOUCH2 = TOUCH_2;
const int TOUCH3 = TOUCH_3;
const int TOUCH4 = TOUCH_4;

const int ACCENT_PIN = BTN_SYN1;
const int BITC_PIN = BTN_SYN2;

const int OSC_LED = LED_1;
const int FILTER_LED = LED_2;
const int ENV_LED = LED_3;

const int SYNC_OUT_PIN = SYNC_OUT;



// One more LED than the physical number of leds for loopback testing
const int NUM_LEDS = 19+1;
const int led_order[NUM_LEDS] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};

// Multiplexer channels.
#define POT_SYN1 0
#define POT_SYN2 1

#define SW_SYN3  2
#define POT_SYN3 3

#define SW_SYN4  4 

#define POT_SYN4 5 
#define POT_SYN5 6 
#define POT_SYN6 7 

const int SLIDE_PIN = 1;
const int DELAY_PIN = 2;
const int AMP_ENV_POT = 4;
const int FILTER_RES_POT = 0;

const int FILTER_FREQ_POT = 5;

const int OSC_DETUNE_POT = 3;  

const int OSC_PW_POT = 7;

const int AMP_POT = 6; 

const int TEMPO_POT = 1;
const int GATE_POT = 2;

int muxAnalogRead(uint8_t channel) { 
  // Any call to pinMode sets the port mux to GPIO mode.
  // We want to force it back to analog mode
  // Might be equivalent to pinMode(MUX_IO, INPUT_DISABLE);
  volatile uint32_t *config;
  config = portConfigRegister(MUX_IO);
  *config = PORT_PCR_MUX(0);

  digitalWriteFast(SYN_ADDR0, bitRead(channel,0));
  digitalWriteFast(SYN_ADDR1, bitRead(channel,1));
  digitalWriteFast(SYN_ADDR2, bitRead(channel,2));
  //do we need to wait a few microseconds?
  delayMicroseconds(50);
  return analogRead(MUX_IO);
}

int potRead(uint8_t num) {
  if(num == 1) {
    return 1023-analogRead(POT_1);
  }
  if(num == 2) {
    return 1023-analogRead(POT_2);
  }
  return muxAnalogRead(num);
}

uint8_t muxDigitalRead(uint8_t channel) { 
  pinMode(MUX_IO, INPUT_PULLUP);
  digitalWriteFast(SYN_ADDR0, bitRead(channel,0));
  digitalWriteFast(SYN_ADDR1, bitRead(channel,1));
  digitalWriteFast(SYN_ADDR2, bitRead(channel,2));
  delayMicroseconds(50);
  //Wait a few microseconds for the selection to propagate. 
  uint8_t p = digitalRead(MUX_IO);
  return p;
}

void pins_init() {
  // PROG pin is unused but connected to reset via a resistor. Make output to be sure.
  pinMode(PROG, INPUT);

  pinMode(BITC_PIN, INPUT_PULLUP);
  pinMode(ACCENT_PIN, INPUT_PULLUP);

  pinMode(SYNC_OUT_PIN, OUTPUT);
  // pinMode(AMP_ENABLE, OUTPUT);
  pinMode(HP_ENABLE, OUTPUT);
  pinMode(SYNC_IN, INPUT);

  pinMode(JACK_DETECT, INPUT);
  pinMode(SYNC_DETECT, INPUT);

  pinMode(SYN_ADDR0, OUTPUT);
  pinMode(SYN_ADDR1, OUTPUT);
  pinMode(SYN_ADDR2, OUTPUT);

  // LED_CLK pin is connected to the last LED in the string for loopback testing
  pinMode(LED_CLK, INPUT_PULLUP);

  randomSeed(analogRead(UNCONNECTED_ANALOG));
}