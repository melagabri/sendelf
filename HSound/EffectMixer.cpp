#include "EffectMixer.h"

#include <debug.h>

namespace HSound {

	EffectMixer::EffectMixer(void): s2(0),inputBufferLength(0),inputBuffer(0) {
		LWP_MutexInit(&s2Mutex,0);
	};

	EffectMixer::~EffectMixer(void) {
		LWP_MutexDestroy(s2Mutex);
		delete[] inputBuffer;
	};

	void EffectMixer::SetSource2(SoundSource *s) {

		LWP_MutexLock(s2Mutex);

		s2=s;

		LWP_MutexUnlock(s2Mutex);
	};

	unsigned int EffectMixer::loadNextBuffer(SoundSample *mixingBuffer, unsigned int length) throw() {
		if(inputBufferLength<length) {
			delete[] inputBuffer;
			inputBuffer=new SoundSample[length];
			inputBufferLength=length;
		}


		//read the first channel pair
		unsigned int inLength=source->loadNextBuffer(mixingBuffer,length);

		if(inLength!=length) {
			_break();
		}

		//and the other one
		inLength=s2->loadNextBuffer(inputBuffer,length);

		if(inLength!=length) {
			_break();
		}

		for(unsigned int i=0;i<length;++i) {
			mixingBuffer[i]*=inputBuffer[i];
		}

		return length;
	};
}
