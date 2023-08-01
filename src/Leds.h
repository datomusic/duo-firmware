#ifndef Leds_h
#define Leds_h

#include <FastLED.h>

#define COLOR_ORDER GRB

#define LED_TYPE SK6812
#define CORRECTION_SK6812 0xFFF1E0
#define CORRECTION_SK6805 0xFFD3E0
#define LED_WHITE CRGB(230,255,150)

#define leds(A) physical_leds[led_order[A]]
// #define next_step ((current_step+1)%SEQUENCER_NUM_STEPS)

CRGB physical_leds[NUM_LEDS];
#define led_play physical_leds[0]

#define SK6812_BRIGHTNESS 32
#define SK6805_BRIGHTNESS 140

/* The black keys have most primary colors. The white keys are inbetween */
const CRGB COLORS[] = {
  0xEE0033, // C
  0xFF0001, // C#
  0xFF4400, // D
  0xFFDD00, // D#
  0xBBEE00, // E
  0x66EE00, // F
  0x11FF00, // F#
  0x006644, // G
  0x0033DD, // G#
  0x4411EE, // A
  0xFF00FF, // A#
  0xFF1166, // B
  0xFF1122, // C
  0xFF2209, // C#
  0xFF8811, // D
  0xFFEE22, // D#
  0x99DD22, // E
  0x44BB22, // F
  0x00EE22, // F#
  0x00CC44, // G
  0x0099CC, // G#
  0x4455CC, // A
  0xBB33BB, // A#
  0xFF6666  // B
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
    if (step_enable[l] == 1) {
      leds(l) = COLORS[step_note[l]%24];
    } else if (step_enable[l] == 2) {
      leds(l) = CRGB(57, 64, 37); // skip step
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