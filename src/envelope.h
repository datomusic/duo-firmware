
#ifndef ENVELOPE_H
#define ENVELOPE_H

#include <stdio.h>

/*
Author: Valter (ggVGc) Sundstrom

Simple linear envelope with configurable subdivision size.
Each time step() is called, an increment value is returned.
This should be used together with 'curVal' to piece-wise step the
output value.
Example:
  If we are using a buffer where we handle 8 samples at at time, it
  makses sense to create a LinearEnvelop with subdivisionCount=8.
  Each time we update our samples we do something like this:
    int inc = envelope.step();
    int val = envelope.curVal; // Kepp local value to piecewise increment for each sample
    for (int i = 0; i < 8; ++i) {
      setSampleAmplitude(samples[i], val);
      val += inc;
    }
*/



struct LinearEnvelope{
  LinearEnvelope(int maxValue, int subdivisionCount) : maxValue(maxValue), subdivisionCount(subdivisionCount), conf(maxValue, 0,0,0,0){ }

  inline void adsr(int attack, int decay, int sustain, int release){
    conf = LinearEnvelope::Config(maxValue, attack, decay, sustain, release);
  }

  inline void on(){ state = Attack; }
  inline void off(){ state = Release; }

  // Returns an incremenet value that should be applied to curVal
  // 'subdivisionCount' times to reach next value.
  inline int step(){
      int curInc = 0;
      int oldVal = curVal;
      switch(state){
        case Idle:
        case Sustain:
          return 0;
          break;
        case Attack:
          curInc = conf.attackRate;
          curVal += curInc*subdivisionCount;
          if(curVal >= maxValue){
            curVal = maxValue;
            curInc = Config::calcRate(curVal-oldVal, subdivisionCount);
            state = Decay;
          }
          break;
        case Decay:
          curInc = conf.decayRate;
          curVal += curInc*subdivisionCount;
          if(curVal <= conf.sustainLevel){
            curVal = conf.sustainLevel;
            curInc = Config::calcRate(curVal-oldVal, subdivisionCount);
            state = Sustain;
          }
          break;
        case Release:
          curInc = conf.releaseRate;
          curVal += curInc*subdivisionCount;
          if(curVal <= 0){
            curVal = 0;
            curInc = Config::calcRate(oldVal, subdivisionCount);
            state = Idle;
          }
          break;
      }
    return curInc;
  }

  enum State{
    Idle
      ,Attack
      ,Decay
      ,Sustain
      ,Release
  };

  State state = Idle;
    struct Config{
      Config(int maxVal, int attack, int decay, int sustain, int release)
        :attackRate(calcRate(maxVal, attack))
         ,decayRate(-calcRate(maxVal-sustain, decay))
         ,releaseRate(-calcRate(sustain, release))
         ,sustainLevel(sustain>maxVal?maxVal:sustain)
      {}

      int attackRate = 0;
      int decayRate = 0;
      int releaseRate = 0;
      int sustainLevel = 0;

      static inline int calcRate(int val, int time) {
        if(time > 0){
          return static_cast<int>(static_cast<float>(val)/time);
        }else{
          return 0;
        }
      }

    };

    const int maxValue = 0;
    const int subdivisionCount = 0;
    Config conf;

    int curVal = 0; // Should NOT be externally modified, but depending on
                    // compiler optimizations it might be faster to keep it as public member.

};

#endif /* ENVELOPE_H */
