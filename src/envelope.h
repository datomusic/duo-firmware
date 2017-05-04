
#ifndef ENVELOPE_H
#define ENVELOPE_H


struct Envelope{
  Envelope(int maxValue) : maxValue(maxValue), conf(maxValue, 0,0,0,0){ }

  inline void adsr(int attack, int decay, int sustain, int release){
    conf = Envelope::Config(maxValue, attack, decay, sustain, release);
  }

  inline void on(){ state = Attack; }
  inline void off(){ state = Release; }

  inline int step(){
      switch(state){
        case Idle:
        case Sustain:
          break;
        case Attack:
          curVal += conf.attackRate;
          if(curVal >= maxValue){
            curVal = maxValue;
            state = Decay;
          }
          break;
        case Decay:
          curVal -= conf.decayRate;
          if(curVal <= conf.sustainLevel){
            curVal = conf.sustainLevel;
            state = Sustain;
          }
          break;
        case Release:
          curVal -= conf.releaseRate;
          if(curVal <= 0){
            curVal = 0;
            state = Idle;
          }
          break;
      }
    return curVal;
  }

  enum State{
    Idle
      ,Attack
      ,Decay
      ,Sustain
      ,Release
  };

  State state = Idle;
  private:

    struct Config{
      Config(int maxVal, int attack, int decay, int sustain, int release)
        :attackRate(calcRate(maxVal, attack))
         ,decayRate(calcRate(maxVal-sustain, decay))
         ,releaseRate(calcRate(sustain, release))
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
    int curVal = 0;
    Config conf;

};

#endif /* ENVELOPE_H */
