
#ifndef ENVELOPE_H
#define ENVELOPE_H


struct Envelope{
  Envelope(int maxValue) : maxValue(maxValue){}

  void adsr(int attack, int decay, int sustain, int release);
  void on();
  void off();
  int step();
  int getVal(){ return curVal; }

  private:
    enum State{
      Idle
        ,Attack
        ,Decay
        ,Sustain
        ,Release
    };

    struct Config{
      Config(){};
      Config(int maxValue, int attack, int decay, int sustain, int release);

      int getAttackRate(){return attackRate;}
      int getDecayRate(){return decayRate;}
      int getReleaseRate(){return releaseRate;}
      int getSustainLevel(){return sustainLevel;}

      private:
        int attackRate = 0;
        int decayRate = 0;
        int releaseRate = 0;
        int sustainLevel = 0;
    };

    int curVal = 0;
    int maxValue = 0;
    State state = Idle;
    Config conf;

};

#endif /* ENVELOPE_H */
