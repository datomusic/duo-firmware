// Key matrix
/*

  Rows are set to input_pullup and cols are pulled low one by one
*/
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
{ STEP_7,    STEP_6, SEQ_START, STEP_2, STEP_3 },
{ BTN_DOWN,  STEP_5, NO_KEY,    STEP_4, BTN_UP },
{ KEYB_0,    KEYB_1, KEYB_2,    KEYB_3, KEYB_4 },
{ KEYB_5,    KEYB_6, KEYB_7,    KEYB_8, KEYB_9 }
};
Keypad keypad = Keypad( makeKeymap(keys), row_pins, col_pins, ROWS, COLS );

void keys_init();


void keys_init() {
    keypad.setDebounceTime(15);
    keypad.setHoldTime(2000);
}