// Key matrix
/*

  Rows are set to input_pullup and cols are pulled low one by one
*/
const byte ROWS = 5;
const byte COLS = 5; 

#ifdef BRAINS_FEB
  byte row_pins[ROWS] = {6,7,8,9,10}; //connect to the row pinouts of the keypad
  byte col_pins[COLS] = {20,2,3,4,5}; //connect to the column pinouts of the keypad
#endif
#ifdef BRAINS_AUG
  byte row_pins[ROWS] = {5,7,9,11,33}; 
  byte col_pins[COLS] = {4,6,8,10,12};
#endif
#ifdef BRAINS_SEP
  byte col_pins[COLS] = {2,14,7,8,6}; 
  byte row_pins[ROWS] = {10,13,11,12,28};
#endif
// Enumeration of the keys that are present
enum BUTTONS {
  KEYB_0,  KEYB_1,  KEYB_2,  KEYB_3,  KEYB_4,  KEYB_5,  KEYB_6,  KEYB_7,  KEYB_8,  KEYB_9,
  STEP_1,  STEP_2,  STEP_3,  STEP_4,  STEP_5,  STEP_6,  STEP_7, STEP_8,
  BTN_DOWN, BTN_UP,
  BTN_SEQ1, BTN_SEQ2,
  SEQ_START
};

// // Key matrix hookup
char keys[ROWS][COLS] = {
{ BTN_SEQ1,  STEP_8, NO_KEY,    STEP_1, BTN_SEQ2 },
{ STEP_6,    STEP_6, SEQ_START, STEP_2, STEP_3 },
{ BTN_DOWN,  STEP_5, NO_KEY,    STEP_4, BTN_UP },
{ KEYB_0,    KEYB_1, KEYB_2,    KEYB_3, KEYB_4 },
{ KEYB_5,    KEYB_6, KEYB_7,    KEYB_8, KEYB_9 }
};
Keypad keypad = Keypad( makeKeymap(keys), row_pins, col_pins, ROWS, COLS );

void keys_init();


void keys_init() {
    keypad.setDebounceTime(15);
}