/*
 * Two drum synths controlled by two capacitive touch sliders
 * The Kick synth is a frequency sweep (disco tom-like). Slide position controls frequency
 * The Hat synth is a noise burst. Slider position controls decay
 * 
 * We're keeping a running average of the cap sensor values to establish a baseline
 * A sharp rise of the values indicates a toggle to on.
 * 
 * Kick synth
 * frequency from 36 to 52
 * duration from 177 to 116
 * modulation fixed at  1.0
 * 
 * Hat synth
 * White noise through an envelope
 */
TouchSlider slider1(TOUCH4, TOUCH3);
TouchSlider slider2(TOUCH1, TOUCH2);

void touch_init();

int kick_duration = 200;
unsigned long kick_on_time, hat_on_time;
bool kick_playing = 0;
int hat_duration = 100;
bool hat_playing = 0;

void drum_init();
void drum_read();

void kick_noteon(uint8_t velocity);
void hat_noteon(uint8_t velocity);
void kick_noteoff();
void hat_noteoff();

#define TOUCHEVENT_TOUCH 1
#define TOUCHEVENT_RELEASE 2

void kickEvent(uint8_t event, int value) {
  Serial.print("\nevent callback ");
  Serial.print(event);
  Serial.print(" a: ");
  Serial.print(slider1.a.getSignal());
  Serial.print(" b: ");
  Serial.print(slider1.b.getSignal());
  
  int val = constrain(map(value,-50,100,0,100),0,127);
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
  Serial.print("\nevent callback ");
  Serial.print(event);
  Serial.print(" a: ");
  Serial.print(slider2.a.getSignal());
  Serial.print(" b: ");
  Serial.print(slider2.a.getSignal());
  
  int val = constrain(map(value,-50,100,0,100),0,127);
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
  slider1.a.setThreshold(30);
  slider1.b.setThreshold(30);
  slider1.setHandleTouchEvent(kickEvent);
  slider2.a.setThreshold(30);
  slider2.b.setThreshold(30);
  slider2.setHandleTouchEvent(hatEvent);
}

void drum_init() {
  // Set all touch channels
  // Initialize the Audio Lib drum synth parts
  drum1.length(kick_duration);
  drum1.frequency(180);
  drum1.pitchMod(1.0);

  envelope3.attack(0.0);
  envelope3.sustain(0.0);
  envelope3.decay(15);

  noise1.amplitude(1.0);
}

void drum_read() {
  // Read all touch channels
  // States:
  slider1.update(millis());
  slider2.update(millis());
  
  if(millis() > kick_on_time + kick_duration) {
    kick_playing = 0;
  }
  if(millis() > hat_on_time + hat_duration) {
    hat_playing = 0;
  }
}


void kick_noteon(uint8_t velocity) {
  AudioNoInterrupts();
  drum1.length(kick_duration);
  drum1.frequency(velocity/2+60);
  AudioInterrupts();
  drum1.noteOn();
  kick_playing = 1;
  kick_on_time = millis();
}

void kick_noteoff() {
  kick_playing = 0;
}
 
void hat_noteon(uint8_t velocity) {
 hat_duration = velocity + 10;
 envelope3.decay(hat_duration);
 envelope3.noteOn();
 hat_playing = 1;
 hat_on_time = millis();
}

void hat_noteoff() {
  //envelope3.noteOff();
  hat_playing = 0;
}
