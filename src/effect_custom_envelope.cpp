#include "./effect_custom_envelope.h"
#include "utility/dspinst.h"

static const int SAMPLES_PER_MSEC = (AUDIO_SAMPLE_RATE_EXACT/1000.0);

int milliseconds2count(float milliseconds) {
  if (milliseconds < 0.0){
    milliseconds = 0.0;
  }
  return static_cast<int>(milliseconds*SAMPLES_PER_MSEC);
}


void AudioEffectCustomEnvelope::updateEnv(){
  env.adsr(
      static_cast<int>(milliseconds2count(attackMs)/8.0f)
      ,static_cast<int>(milliseconds2count(decayMs)/8.0f)
      ,sustainVal
      ,static_cast<int>(milliseconds2count(releaseMs)/8.0f)
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


void AudioEffectCustomEnvelope::update(void) {
  if (env.state == Envelope::Idle) {
    return;
  }

  audio_block_t *block;
  uint32_t *p, *end;
  uint32_t sample12, sample34, sample56, sample78, tmp1, tmp2;


  block = receiveWritable();
  if (!block) return;
  p = (uint32_t *)(block->data);
  end = p + AUDIO_BLOCK_SAMPLES/2;


  while (p < end) {
    // process 8 samples
    sample12 = *p++;
    sample34 = *p++;
    sample56 = *p++;
    sample78 = *p++;
    p -= 4;
    const int v = env.step();
    tmp1 = signed_multiply_32x16b(v, sample12);
    tmp2 = signed_multiply_32x16t(v, sample12);
    sample12 = pack_16b_16b(tmp2, tmp1);
    tmp1 = signed_multiply_32x16b(v, sample34);
    tmp2 = signed_multiply_32x16t(v, sample34);
    sample34 = pack_16b_16b(tmp2, tmp1);
    tmp1 = signed_multiply_32x16b(v, sample56);
    tmp2 = signed_multiply_32x16t(v, sample56);
    sample56 = pack_16b_16b(tmp2, tmp1);
    tmp1 = signed_multiply_32x16b(v, sample78);
    tmp2 = signed_multiply_32x16t(v, sample78);
    sample78 = pack_16b_16b(tmp2, tmp1);
    *p++ = sample12;
    *p++ = sample34;
    *p++ = sample56;
    *p++ = sample78;

  }
  transmit(block);
  release(block);
}


