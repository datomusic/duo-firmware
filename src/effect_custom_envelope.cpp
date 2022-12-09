#include "./effect_custom_envelope.h"

static const int SAMPLES_PER_MSEC = (AUDIO_SAMPLE_RATE_EXACT/1000.0f);

int milliseconds2count(float milliseconds) {
  if (milliseconds < 0.0f){
    milliseconds = 0.0f;
  }
  return static_cast<int>(milliseconds*SAMPLES_PER_MSEC);
}


void AudioEffectCustomEnvelope::updateEnv(){
  env.adsr(
      static_cast<int>(milliseconds2count(attackMs))
      ,static_cast<int>(milliseconds2count(decayMs))
      ,sustainVal
      ,static_cast<int>(milliseconds2count(releaseMs))
    );
}

void AudioEffectCustomEnvelope::attack(int milliseconds) {
  attackMs = milliseconds;
  updateEnv();
}


void AudioEffectCustomEnvelope::decay(int milliseconds){
  decayMs = milliseconds;
  updateEnv();
}


void AudioEffectCustomEnvelope::sustain(float level) {
  sustainVal = static_cast<int>(level * ENVELOPE_MAX);
  updateEnv();
}


void AudioEffectCustomEnvelope::release(int milliseconds) {
  releaseMs = milliseconds;
  updateEnv();
}

void AudioEffectCustomEnvelope::noteOn(void) {
  __disable_irq();
  env.on();
  __enable_irq();
}

void AudioEffectCustomEnvelope::noteOff(void) {
  __disable_irq();
  env.off();
  __enable_irq();
}




