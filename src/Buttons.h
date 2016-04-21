// Key matrix
const byte ROWS = 5;
const byte COLS = 5; 

byte row_pins[ROWS] = {6,7,8,9,10}; //connect to the row pinouts of the keypad
byte col_pins[COLS] = {14,2,3,4,5}; //connect to the column pinouts of the keypad

// Enumeration of the keys that are present
enum BUTTONS {
  DUMMY_KEY,
  KEYB_0,  KEYB_1,  KEYB_2,  KEYB_3,  KEYB_4,  KEYB_5,  KEYB_6,  KEYB_7,  KEYB_8,  KEYB_9,
  STEP_0,  STEP_1,  STEP_2,  STEP_3,  STEP_4,  STEP_5,  STEP_6,  STEP_7,
  OCT_DOWN, OCT_UP,
  DBL_SPEED, SEQ_RANDOM,
  BITC_0,  BITC_1,  FX_0,
  OSC1_PULSE,
  OSC1_SAW, OSC2_PULSE,
  MIC_1, MIC_2
};

// Key matrix hookup
char keys[ROWS][COLS] = {
  { STEP_0, STEP_1, STEP_2, STEP_3, DUMMY_KEY },
  { STEP_7, STEP_6, STEP_5, STEP_4, DUMMY_KEY },
  { OCT_DOWN, SEQ_RANDOM, DBL_SPEED, OCT_UP, DUMMY_KEY },
  { KEYB_0, KEYB_1, KEYB_2, KEYB_3, KEYB_4 },
  { KEYB_5, KEYB_6, KEYB_7, KEYB_8, KEYB_9 }
};
Keypad keypad = Keypad( makeKeymap(keys), row_pins, col_pins, ROWS, COLS );

void keys_init();


void keys_init() {
    keypad.setDebounceTime(15);
}