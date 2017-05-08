// Key matrix
/*
  Rows are set to input_pullup and cols are pulled low one by one
*/

enum BUTTONS {
  DUMMY_KEY,
  KEYB_0,  KEYB_1,  KEYB_2,  KEYB_3,  KEYB_4,  KEYB_5,  KEYB_6,  KEYB_7,  KEYB_8,  KEYB_9,
  STEP_1,  STEP_2,  STEP_3,  STEP_4,  STEP_5,  STEP_6,  STEP_7, STEP_8,
  BTN_DOWN, BTN_UP,
  BTN_SEQ1, BTN_SEQ2,
  SEQ_START
};

#ifdef BRAINS_SEP
  const uint8_t ROWS = 5;
  const uint8_t COLS = 5; 
  const uint8_t powerbutton_col = 2;
  const uint8_t powerbutton_row = 1;
  
  uint8_t col_pins[COLS] = {2,14,7,8,6}; 
  uint8_t row_pins[ROWS] = {10,13,11,12,28};
  // Enumeration of the keys that are present
  
  // // Key matrix hookup
  char buttons[ROWS][COLS] = {
  { BTN_SEQ1,  STEP_8, NO_KEY,    STEP_1, BTN_SEQ2 },
  { STEP_7,    STEP_6, SEQ_START, STEP_2, STEP_3 },
  { BTN_DOWN,  STEP_5, NO_KEY,    STEP_4, BTN_UP },
  { KEYB_0,    KEYB_1, KEYB_2,    KEYB_3, KEYB_4 },
  { KEYB_5,    KEYB_6, KEYB_7,    KEYB_8, KEYB_9 }
  };
#endif
#ifdef SEQ_0_4
 const uint8_t powerbutton_col = 1;
 const uint8_t powerbutton_row = 1;
 const uint8_t ROWS = 4;
 const uint8_t COLS = 6; 
 
 uint8_t col_pins[COLS] = {COL_1,COL_2,COL_3,COL_4,COL_5,COL_6}; 
 uint8_t row_pins[ROWS] = {ROW_1, ROW_2, ROW_3, ROW_4};
 // Enumeration of the keys that are present
 
 // // Key matrix hookup
 char buttons[ROWS][COLS] = {
 { BTN_SEQ1,  STEP_8, STEP_1, BTN_SEQ2, STEP_7, STEP_6 },
 { NO_KEY,    SEQ_START, STEP_2, STEP_3,   STEP_4,  STEP_5 },
 { KEYB_0,    BTN_DOWN, KEYB_2, KEYB_1, KEYB_4,   KEYB_3 },
 { KEYB_6,    KEYB_5, KEYB_8,    KEYB_7, BTN_UP, KEYB_9 }
 };
#endif

Keypad button_matrix = Keypad( makeKeymap(buttons), row_pins, col_pins, ROWS, COLS );

void button_matrix_init();
bool keys_scan_powerbutton();

void button_matrix_init() {
    button_matrix.setDebounceTime(15);
    button_matrix.setHoldTime(2000);
}

bool keys_scan_powerbutton() {
  bool r = false;

  pinMode(row_pins[powerbutton_row],INPUT_PULLUP);
  pinMode(col_pins[powerbutton_col],OUTPUT);
  digitalWrite(col_pins[powerbutton_col],LOW);
  r = (digitalRead(row_pins[powerbutton_row]) == LOW);

  return r;
}