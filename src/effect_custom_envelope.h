#ifndef EFFECT_CUSTOM_ENVELOPE_H
#define EFFECT_CUSTOM_ENVELOPE_H

#include "AudioStream.h"
#include "envelope.h"


class AudioEffectCustomEnvelope : public AudioStream {
  static const int ENVELOPE_MAX = 0x10000;
public:
	AudioEffectCustomEnvelope() : AudioStream(1, inputQueueArray), env(ENVELOPE_MAX) { }

	void noteOn();
	void noteOff();
	void attack(int milliseconds);
	void decay(int milliseconds);
	void sustain(float level);
	void release(int milliseconds);

	using AudioStream::release;
	virtual void update(void);

private:
        void updateEnv();

	audio_block_t *inputQueueArray[1];
        Envelope env;
        int attackMs;
        int decayMs;
        int sustainVal;
        int releaseMs;
};

#endif /* EFFECT_CUSTOM_ENVELOPE_H */
