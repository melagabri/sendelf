#include "SoundSample.h"
#include "SoundSource.h"
#include "SoundSink.h"

#ifndef SOUNDEFFECT_H
#define SOUNDEFFECT_H

namespace HSound {

	class SoundEffect : public HSound::SoundSource, public HSound::SoundSink {
	public:
		SoundEffect();
		~SoundEffect();
		virtual unsigned int loadNextBuffer(HSound::SoundSample *buffer, unsigned int length) throw()=0;
	};

}
#endif
