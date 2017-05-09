/*
 * Returns 0 when no touch is detected. 
 * Otherwise returns a value that corresponds to the relative
 * location of the first touch on the slider
 * 
 * Requires Teensy's touchRead
 * 
 */
 
#ifndef TouchSlider_h
#define TouchSlider_h

#include "Arduino.h"

#define NUM_SAMPLES 16

class TouchSensor {
  public:
    uint8_t pin = 0; // Arduino pin number

    TouchSensor(int pin) : pin(pin)  {
      _threshold = 50;
      _cnt = 0;
    }
    void updateBaseline() {
        if(_baselineIndex >= NUM_SAMPLES) { _baselineIndex = 0; }
        if(_cnt < NUM_SAMPLES) { _cnt++; }
        
        _baselineValues[_baselineIndex] = TSIRead(pin);
        _baselineIndex++;
        
        int sum = 0;
        for(int i = 0; i < _cnt; i++) {
          sum += _baselineValues[i];
        }
        _baseline = sum / _cnt;
    }

    void setThreshold(int32_t threshold) {
        _threshold = (int16_t)threshold;
    }

    int32_t getBaseline() {
        return _baseline;
    }
    
    int32_t getDelta() {
      int32_t delta = getSignal() - getBaseline();
      return delta;
    }

    int32_t getSignal() {
        return _signal;
    }

    int32_t getThreshold() {
        return _threshold;
    }

    int TSIRead(int8_t pin)
    {
      return touchRead(pin);
    }
    bool touchDetected() {
      if(_cnt < NUM_SAMPLES) {
        return 0; 
      } else {
        return (getSignal() - getBaseline()) > getThreshold();
      }
    }
    void update() {
      _signal = TSIRead(pin);
    }
  private:
    int _cnt;
    int _baseline;
    int _baselineValues[NUM_SAMPLES];
    int _baselineIndex;
    int32_t _threshold;
    int16_t _signal;
};

class TouchSlider
{
public:
  TouchSensor a, b;
  TouchSlider(int pinA, int pinB) : a(pinA), b(pinB) {
    a.updateBaseline();
    b.updateBaseline();
    numTouches = 0;
    updateInterval = 100;
  }
  inline void setHandleTouchEvent(void (*fptr)(uint8_t event, int value)) {
    tTouchCallback  = fptr;
  }
  bool touchDetected;
  void update(unsigned long sysTick){
    a.update();
    b.update();
    
    if(sysTick > lastUpdate + updateInterval) {
      lastUpdate = sysTick;
      a.updateBaseline();
      b.updateBaseline();
    }
  
    int32_t da = a.getDelta();
    int32_t db = b.getDelta();

    numTouches <<= 1;
    
    if(a.touchDetected() || b.touchDetected()) {
      numTouches |= 1UL;
    } else {
      numTouches &= ~1UL;
    }

    uint16_t bottomBits = numTouches & B1111;

    if(!touchDetected && bottomBits == B1111) {
      touchDetected = 1;
      _sliderValue = (db - da);
      if (tTouchCallback != 0) {
        tTouchCallback(1, _sliderValue);
      }
      _lastTouch = sysTick;
    }
    
    if(touchDetected && bottomBits == 0) {
      touchDetected = 0;
    }
  }
 private:
  void (*tTouchCallback)(byte event, int value);
  unsigned long numTouches;
  int _sliderValue;
  unsigned long _lastTouch;
  unsigned long lastUpdate;
  int updateInterval;
};

#endif