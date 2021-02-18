#ifndef Leds_h
#define Leds_h

#include <FastLED.h>

#define COLOR_ORDER GRB

#define LED_TYPE SK6812
#define CORRECTION_SK6812 0xFFF1E0
#define CORRECTION_SK6805 0xFFD3E0
#define LED_WHITE CRGB(230,255,150)

#define leds(A) physical_leds[led_order[A]]
#define next_step ((current_step+1)%SEQUENCER_NUM_STEPS)

CRGB physical_leds[NUM_LEDS];
#define led_play physical_leds[0]

#define SK6812_BRIGHTNESS 32
#define SK6805_BRIGHTNESS 140

/* The black keys have assigned colors. The white keys are shown in gray */
const CRGB COLORS[] = {
  0x444444,
  0xFF0001,
  0x444444,
  0xFFDD00,
  0x444444,
  0x444444,
  0x11FF00,
  0x444444,
  0x0033DD,
  0x444444,
  0xFF00FF,
  0x444444,
  0x444444,
  0xFF2209,
  0x444444,
  0x99FF00,
  0x444444,
  0x444444,
  0x00EE22,
  0x444444,
  0x0099CC,
  0x444444,
  0xBB33BB,
  0x444444
};

void led_init();
void led_update();
void led_data_received();

void led_data_received() {
    FastLED.setBrightness(SK6805_BRIGHTNESS); 
    FastLED.setCorrection(CORRECTION_SK6805);
    detachInterrupt(LED_CLK);
}

void led_init() {
  FastLED.addLeds<LED_TYPE, LED_DATA, COLOR_ORDER>(physical_leds, NUM_LEDS);
  
  FastLED.setBrightness(SK6812_BRIGHTNESS); 
  FastLED.setCorrection(CORRECTION_SK6812);

  // We're going to do a loopback test first to determine brightness
  attachInterrupt(LED_CLK, led_data_received, CHANGE);
  FastLED.clear();
  physical_leds[NUM_LEDS] = CRGB(0xff6805);
  FastLED.show();
  
  FastLED.clear();
  FastLED.show();
  /* The 400ms delay introduced by this startup animation prevents
     an audible pop/click at startup
     */
     
  #ifdef DEV_MODE
     physical_leds[0] = CRGB::Blue;
  #endif

  // The key of the current MIDI channel lights up
  physical_leds[MIDI_CHANNEL+8] = COLORS[SCALE[MIDI_CHANNEL-1]%24];

  FastLED.show();
  delay(500);

  for(int i = 0; i < 10; i++) {
    analogWrite(ENV_LED,i*8);
    analogWrite(FILTER_LED,i*8);
    analogWrite(OSC_LED,i*8);

    physical_leds[i+9] = COLORS[SCALE[i]%24];
    delay(20);
    FastLED.show();
  }
}

// Updates the LED colour and brightness to match the stored sequence
void led_update() {
  for (int l = 0; l < SEQUENCER_NUM_STEPS; l++) {
    if (step_enable[l]) {
      leds(l) = COLORS[step_note[l]%24];
    } else {
      leds(l) = CRGB::Black;
    }
     
    if(note_is_playing) {
      leds(((current_step+random_offset)%SEQUENCER_NUM_STEPS)) = LED_WHITE;
    } else {
      if(!step_enable[((current_step+random_offset)%SEQUENCER_NUM_STEPS)]) {
        leds(((current_step+random_offset)%SEQUENCER_NUM_STEPS)) = CRGB::Black;
      }

      if(!sequencer_is_running) {
        if(((sequencer_clock % 24) < 12)) {
          if(step_enable[next_step]) {
            leds(next_step) = COLORS[step_note[next_step]%24];
          } else {
            leds(next_step) = CRGB::Black;
          }
          led_play = LED_WHITE;
          led_play.fadeLightBy((sequencer_clock % 12)*16);
        } else {
          led_play = CRGB::Black;
          if(step_enable[next_step]) {
            leds(next_step) = blend(LED_WHITE, COLORS[step_note[next_step]%24], (sequencer_clock % 12)*16);
          } else {
            leds(next_step) = LED_WHITE;
            leds(next_step) = leds(next_step).fadeLightBy((sequencer_clock % 12)*16);
          }
        }
      } else {
        led_play = LED_WHITE;
      }
    }
  }
  FastLED.show();
  analogWrite(ENV_LED, (peak1.read()*127.0f));
  analogWrite(FILTER_LED, 1 + ((synth.filter*synth.filter) >> 13));
  analogWrite(OSC_LED, 128 - ((synth.pulseWidth*synth.pulseWidth)>>13));
}

#endif