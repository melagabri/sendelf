#include "SoundSink.h"

namespace HSound {

	SoundSink::SoundSink(void): source(0) {
		LWP_MutexInit(&sourceMutex,0);
	};

	SoundSink::SoundSink(SoundSource *s) : source(s) {
		LWP_MutexInit(&sourceMutex,0);
	}

	SoundSink::~SoundSink(void) {
		LWP_MutexDestroy(sourceMutex);
	};

	void SoundSink::SetSource(SoundSource *s) {

		LWP_MutexLock(sourceMutex);

		source=s;

		LWP_MutexUnlock(sourceMutex);
	};

}

