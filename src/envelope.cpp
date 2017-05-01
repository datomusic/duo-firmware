
#include "envelope.h"



void Envelope::adsr(int attack, int decay, int sustain, int release){
  this->conf = Envelope::Config(maxValue, attack, decay, sustain, release);
}


int Envelope::step(){
  int sustainMax = conf.getSustainLevel();
  if(sustainMax > maxValue){
    sustainMax = maxValue;
  }

  switch(state){
    case Idle:
    case Sustain:
      break;
    case Attack:
      curVal += conf.getAttackRate();
      if(curVal >= maxValue){
        curVal = maxValue;
        state = Decay;
      }
      break;
    case Decay:
      curVal -= conf.getDecayRate();
      if(curVal <= sustainMax){
        curVal = sustainMax;
        state = Sustain;
      }
      break;
    case Release:
      curVal -= conf.getReleaseRate();
      if(curVal <= 0){
        curVal = 0;
        state = Idle;
      }
      break;
  }
  return curVal;
}

void Envelope::on(){
  state = Attack;
}

void Envelope::off(){
  state = Release;
}


Envelope::Config::Config(int maxValue, int attack, int decay, int sustain, int release){
  this->sustainLevel = sustain;

  if(attack > 0){
    this->attackRate = static_cast<int>(static_cast<float>(maxValue)/attack);
  }

  if(decay > 0){
    this->decayRate = static_cast<int>(static_cast<float>(maxValue-sustain)/decay);
  }

  if(release > 0){
    this->releaseRate = static_cast<int>(static_cast<float>(sustain)/release);
  }
};
