#ifndef EFFECT_CUSTOM_ENVELOPE_H
#define EFFECT_CUSTOM_ENVELOPE_H

#include "AudioStream.h"
#include "envelope.h"
#include "utility/dspinst.h"


class AudioEffectCustomEnvelope : public AudioStream {
  static const int ENVELOPE_MAX = 0x10000;
public:
	AudioEffectCustomEnvelope() : AudioStream(1, inputQueueArray), env(ENVELOPE_MAX, 8) { }

	void noteOn();
	void noteOff();
	void attack(int milliseconds);
	void decay(int milliseconds);
	void sustain(float level);
	void release(int milliseconds);

	using AudioStream::release;
        virtual void update(void) {
          audio_block_t *block;
          uint32_t *p, *end;
          uint32_t sample12, sample34, sample56, sample78, tmp1, tmp2;


          block = receiveWritable();
          if (!block) return;
          p = (uint32_t *)(block->data);
          end = p + AUDIO_BLOCK_SAMPLES/2;


          while (p < end) {
            // process 8 samples
            int v = env.curVal;
            int inc = env.step();
            sample12 = *p++;
            sample34 = *p++;
            sample56 = *p++;
            sample78 = *p++;
            p -= 4;
            tmp1 = signed_multiply_32x16b(v, sample12);
            v+=inc;
            tmp2 = signed_multiply_32x16t(v, sample12);
            v+=inc;
            sample12 = pack_16b_16b(tmp2, tmp1);

            tmp1 = signed_multiply_32x16b(v, sample34);
            v+=inc;
            tmp2 = signed_multiply_32x16t(v, sample34);
            v+=inc;
            sample34 = pack_16b_16b(tmp2, tmp1);
            tmp1 = signed_multiply_32x16b(v, sample56);
            v+=inc;
            tmp2 = signed_multiply_32x16t(v, sample56);
            v+=inc;
            sample56 = pack_16b_16b(tmp2, tmp1);
            tmp1 = signed_multiply_32x16b(v, sample78);
            v+=inc;
            tmp2 = signed_multiply_32x16t(v, sample78);
            v+=inc;
            sample78 = pack_16b_16b(tmp2, tmp1);
            *p++ = sample12;
            *p++ = sample34;
            *p++ = sample56;
            *p++ = sample78;

          }
          transmit(block);
          release(block);
        }

private:
        void updateEnv();

	audio_block_t *inputQueueArray[1];
        LinearEnvelope env;
        int attackMs;
        int decayMs;
        int sustainVal;
        int releaseMs;
};

#endif /* EFFECT_CUSTOM_ENVELOPE_H */
