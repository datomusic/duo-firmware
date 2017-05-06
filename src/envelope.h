
#ifndef ENVELOPE_H
#define ENVELOPE_H

#include <stdio.h>


struct Envelope{
  Envelope(int maxValue, int pieceSize) : maxValue(maxValue), pieceSize(pieceSize), conf(maxValue, 0,0,0,0){ }

  inline void adsr(int attack, int decay, int sustain, int release){
    conf = Envelope::Config(maxValue, attack, decay, sustain, release);
  }

  inline void on(){ state = Attack; }
  inline void off(){ state = Release; }

  inline int step(){
      int curInc;
      int oldVal = curVal;
      switch(state){
        case Idle:
        case Sustain:
          return 0;
          break;
        case Attack:
          curInc = conf.attackRate;
          curVal += curInc*pieceSize;
          if(curVal >= maxValue){
            curVal = maxValue;
            curInc = Config::calcRate(curVal-oldVal, pieceSize);
            state = Decay;
          }
          break;
        case Decay:
          curInc = conf.decayRate;
          curVal += curInc*pieceSize;
          if(curVal <= conf.sustainLevel){
            curVal = conf.sustainLevel;
            curInc = Config::calcRate(curVal-oldVal, pieceSize);
            state = Sustain;
          }
          break;
        case Release:
          curInc = conf.releaseRate;
          curVal += curInc*pieceSize;
          if(curVal <= 0){
            curVal = 0;
            curInc = Config::calcRate(oldVal, pieceSize);
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
    const int pieceSize = 0;
    int curVal = 0;
    Config conf;

};

#endif /* ENVELOPE_H */
