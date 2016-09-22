/*
  TODO:
  - split this into header file and source file
  - have the main program determine the led colours

 */
#ifndef Leds_h
#define Leds_h

#include "Arduino.h"
#include <FastLED.h>

// KINGBRIGHT  KPHF-1612QBDSURKZGC scaling values
#define CORRECTION_KINGBRIGHT 0xE050FF
// #define CORRECTION_KINGBRIGHT 0x902840


#ifdef BRAINS_FEB
  #define LED_DT 12
  #define LED_CLK 11
  #define COLOR_ORDER BRG
  #define LED_TYPE WS2803
#endif
#ifdef BRAINS_AUG
  #define LED_DT 32
  #define LED_CLK 30
  #define COLOR_ORDER BRG
  #define LED_TYPE WS2803
#endif

#define NUM_LEDS 12


const int led_order[NUM_LEDS] = {
  9,6,7,8,4,5,0,1
};

#define leds(A) physical_leds[led_order[A]]

CRGB physical_leds[NUM_LEDS];

const int LED_BRIGHTNESS = 255;
const int LED_PIN = 13;

const CRGB COLOURS[] = {
  0xFF0000,
  0xFF2200,
  0xFF8800,
  0xEEFF00,
  0x00FF00,
  0x00FF33,
  0x00FFCC,
  0x0000FF,
  0x6600FF,
  0x990099
};

void led_init();
void update_leds();

void led_init() {
  pinMode(LED_PIN, OUTPUT);

  FastLED.addLeds<LED_TYPE, LED_DT, LED_CLK, COLOR_ORDER>(physical_leds, NUM_LEDS);
  FastLED.setBrightness(LED_BRIGHTNESS); 
 
  FastLED.setCorrection(CORRECTION_KINGBRIGHT);
  FastLED.clear();
  FastLED.show();
}

// Updates the LED colour and brightness to match the stored sequence
void update_leds() {

  for (int l = 0; l < NUM_STEPS; l++) {
    if (step_enable[l]) {

      leds(l) = COLOURS[step_note[l]];

      if (step_velocity[l] < 50) {
        leds(l).nscale8_video(step_velocity[l]+20);
      }
    }
     
    if(note_is_playing) {
      leds(current_step) = CRGB(190,255,190);
    } else {
    if(!step_enable[current_step]) {
      leds(current_step) = CRGB::Black;
    }}
  }

  FastLED.show();
}

#endif