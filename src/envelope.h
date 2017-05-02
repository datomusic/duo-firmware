
#ifndef ENVELOPE_H
#define ENVELOPE_H


struct Envelope{
  Envelope(int maxValue) : maxValue(maxValue), conf(maxValue, 0,0,0,0){ }

  inline void adsr(int attack, int decay, int sustain, int release){
    conf = Envelope::Config(maxValue, attack, decay, sustain, release);
  }

  inline void on(){ state = Attack; }
  inline void off(){ state = Release; }

  inline void step(int count, int out[]){
    const int sustainMax = conf.sustainLevel > maxValue ? maxValue : conf.sustainLevel;
    int v = curVal;
    for (int i = 0;  i < count; ++i) {
      switch(state){
        case Idle:
        case Sustain:
          break;
        case Attack:
          v += conf.attackRate;
          if(v >= maxValue){
            v = maxValue;
            state = Decay;
          }
          break;
        case Decay:
          v -= conf.decayRate;
          if(v <= sustainMax){
            v = sustainMax;
            state = Sustain;
          }
          break;
        case Release:
          v -= conf.releaseRate;
          if(v <= 0){
            v = 0;
            state = Idle;
          }
          break;
      }
      out[i] = v;
    }
    curVal = v;
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
         ,sustainLevel(sustain)
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
