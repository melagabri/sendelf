#ifndef DELAYBUFFER_H
#define DELAYBUFFER_H
#include "SoundEffect.h"

#include <deque>

namespace HSound {

	using std::deque;

	class DelayBuffer : public SoundEffect {
	public:
		virtual unsigned int loadNextBuffer(HSound::SoundSample *buffer, unsigned int length) throw();
		DelayBuffer();
		~DelayBuffer();
		unsigned int delay;//measured in samples
		bool allowPading;
		SoundSample padding;
	protected:
		mutex_t delayBufferMutex;
		deque<SoundSample> delayBuffer;
	};
}
#endif
