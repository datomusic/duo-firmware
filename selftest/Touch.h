int kick_duration = 1000;
unsigned long kick_on_time, hat_on_time;
bool kick_playing = 0;
int hat_duration = 1000;
bool hat_playing = 0;
uint8_t hat_value = 0;
uint8_t kick_value = 0;

void drum_init();
void drum_read();
void kick_noteon(uint8_t velocity);
void hat_noteon(uint8_t velocity);
void kick_noteoff();
void hat_noteoff();
void kickEvent(uint8_t event, int value);
void hatEvent(uint8_t event, int value);

#define TOUCHEVENT_TOUCH 1
#define TOUCHEVENT_RELEASE 2

TouchSlider kickSlider(TOUCH3, TOUCH4);
TouchSlider slider2(TOUCH1, TOUCH2);

void kickEvent(uint8_t event, int value) {

  int val = constrain(value+63,0,127);

  switch(event) {
    case TOUCHEVENT_TOUCH:
      kick_noteon(val);
      break;
    case TOUCHEVENT_RELEASE:
      kick_noteoff();
      break;
  }
}

void hatEvent(uint8_t event, int value) {
  int val = constrain(value+63,0,127);

  switch(event) {
    case TOUCHEVENT_TOUCH:
      hat_noteon(val);
      break;
    case TOUCHEVENT_RELEASE:
      hat_noteoff();
      break;
  }
}


void touch_init() {
  kickSlider.a.setThreshold(30);
  kickSlider.b.setThreshold(30);
  kickSlider.setHandleTouchEvent(kickEvent);
  slider2.a.setThreshold(30);
  slider2.b.setThreshold(30);
  slider2.setHandleTouchEvent(hatEvent);
}


void drum_init() {
 
}

void drum_read() {
  kickSlider.update(millis());
  slider2.update(millis());
  
  if(millis() > kick_on_time + kick_duration) {
    kick_noteoff();
  }
  if(millis() > hat_on_time + hat_duration) {
    hat_noteoff();
  }
}

void kick_noteon(uint8_t velocity) {
  kick_value = velocity;
  Serial.println(velocity);
}

void kick_noteoff() {
  kick_value = 0;
}

void hat_noteon(uint8_t velocity) {
 hat_value = velocity;
 Serial.println(velocity);
}

void hat_noteoff() {
  hat_value = 0;
}
