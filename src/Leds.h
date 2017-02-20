/*
  TODO:
  - split this into header file and source file
  - have the main program determine the led colours

 */
#ifndef Leds_h
#define Leds_h

#include "Arduino.h"
#include <FastLED.h>

#define CORRECTION_SK6812 0xFFFFFF
#define LED_WHITE CRGB(230,255,150);

#define leds(A) physical_leds[led_order[A]]

CRGB physical_leds[NUM_LEDS];
const uint8_t LEDS_UPDATE_INTERVAL = 6;
unsigned long leds_last_updated = 0;

const int LED_BRIGHTNESS = 32;

// ALternate pin colors
const CRGB COLORS[] = {
  0xFF0001,
  0xFFDD00,
  0x11FF00,
  0x0033DD,
  0xFF00FF,
  0xFF2209,
  0x99FF00,
  0x00EE22,
  0x0099CC,
  0xBB33BB
};

void led_init();
void update_leds();
void vu_meter();

void led_init() {
  FastLED.addLeds<LED_TYPE, LED_DATA, COLOR_ORDER>(physical_leds, NUM_LEDS);
  
  FastLED.setBrightness(LED_BRIGHTNESS); 
 
  FastLED.setCorrection(CORRECTION_SK6812);
  FastLED.clear();

  /* The 800ms delay introduced by this startup animation prevents
     an audible pop/click at startup
     AudioNoInterrupts() is needed for it to run smoothly
     */
  for(int i = 0; i < 10; i++) {
    physical_leds[i+9] = COLORS[i];
    FastLED.show();
  }
}

// Updates the LED colour and brightness to match the stored sequence
void update_leds() {

  //if(millis() - leds_last_updated > LEDS_UPDATE_INTERVAL) {
    if(sequencer_is_running) {
        physical_leds[0] = CRGB::Black;
    } else {
        physical_leds[0] = LED_WHITE;
    }
    for (int l = 0; l < SEQUENCER_NUM_STEPS; l++) {
      if (step_enable[l]) {

        leds(l) = COLORS[step_note[l]];

        if (step_velocity[l] < 50) {
          leds(l).nscale8_video(step_velocity[l]+20);
        }
      }
       
      if(note_is_playing) {
        leds(current_step) = LED_WHITE;
      } else {
      if(!step_enable[current_step]) {
        leds(current_step) = CRGB::Black;
      }}
    }
    AudioNoInterrupts();
    FastLED.show();
    AudioInterrupts();
    analogWrite(ENV_LED, 255-((int)(peak1.read()*255.)));
    leds_last_updated = millis();
  //}

}

#endif
