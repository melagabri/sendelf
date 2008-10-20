#ifndef EFFECTMIXER_H
#define EFFECTMIXER_h

#include "SoundEffect.h"

namespace HSound {

	class EffectMixer : public SoundEffect {
	public:
		EffectMixer();
		~EffectMixer();

		virtual unsigned int loadNextBuffer(HSound::SoundSample *buffer, unsigned int length) throw();

		void SetSource2(SoundSource *);

	protected:
		SoundSource *s2;
		mutex_t s2Mutex;

		unsigned int inputBufferLength;
		SoundSample *inputBuffer;
	};
}

#endif
