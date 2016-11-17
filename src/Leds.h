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
#define CORRECTION_SK6812 0xFFFFFF
// #define CORRECTION_KINGBRIGHT 0x902840
#define LED_WHITE CRGB(255,255,150);

#ifdef BRAINS_FEB
  #define LED_DT 12
  #define LED_CLK 11
  #define COLOR_ORDER BRG
  #define LED_TYPE WS2801
  #define NUM_LEDS 12
  const int led_order[NUM_LEDS] = {9,6,7,8,4,5,0,1};
#endif
#ifdef BRAINS_AUG
  #define LED_DT 32
  #define LED_CLK 30
  #define COLOR_ORDER BRG
  #define LED_TYPE WS2801
  #define NUM_LEDS 12
  const int led_order[NUM_LEDS] = {9,6,7,8,4,5,0,1};
#endif
#ifdef BRAINS_SEP
  #define LED_DT 5
  #define COLOR_ORDER GRB
  #define LED_TYPE SK6812
  #define NUM_LEDS 19
  const int led_order[NUM_LEDS] = {1,2,3,4,5,6,7,8};
#endif

#define leds(A) physical_leds[led_order[A]]

CRGB physical_leds[NUM_LEDS];

const int LED_BRIGHTNESS = 32;

const CRGB COLOURS[] = {
  0xFF0000,
  0xFF4400,
  0xFF9900,
  0x99FF00,
  0x11FF00,
  0x00FF33,
  0x00CCCC,
  0x0000FF,
  0x3300FF,
  0x990099
};

void led_init();
void update_leds();

void led_init() {
  // FastLED.addLeds<LED_TYPE, LED_DT, LED_CLK, COLOR_ORDER>(physical_leds, NUM_LEDS);

  FastLED.addLeds<LED_TYPE, LED_DT, COLOR_ORDER>(physical_leds, NUM_LEDS);
  
  FastLED.setBrightness(LED_BRIGHTNESS); 
 
  FastLED.setCorrection(CORRECTION_SK6812);
  FastLED.clear();
  for(int i = 0; i < 10; i++) {
    physical_leds[i+9] = COLOURS[i];
  }
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
}

#endif
