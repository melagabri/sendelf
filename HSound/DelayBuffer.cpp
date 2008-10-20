#include "DelayBuffer.h"

#include "debug.h"

namespace HSound {

	unsigned int DelayBuffer::loadNextBuffer(HSound::SoundSample *buffer, unsigned int length) throw() {

		LWP_MutexLock(sourceMutex);
		unsigned int inLength=source->loadNextBuffer(buffer,length);
		LWP_MutexUnlock(sourceMutex);

		unsigned int i;

		LWP_MutexLock(delayBufferMutex);

		for(i=0;i<inLength;++i) {
			delayBuffer.push_front(buffer[i]);
		}

		if(allowPading) {
			for(i=inLength;i<length;++i) {
				delayBuffer.push_front(padding);
			}
		}

		deque<SoundSample>::const_iterator enditr = delayBuffer.end();
		deque<SoundSample>::const_iterator itr = delayBuffer.begin();
		for(i=0;i<length && itr!=enditr;++itr,++i) {
			buffer[i]=*itr;
		}

		if(delayBuffer.size()<length) {
			_break();
		}

		delayBuffer.resize(delayBuffer.size()-length);

		LWP_MutexUnlock(delayBufferMutex);

		return i;

	};

	DelayBuffer::DelayBuffer() {
		LWP_MutexInit(&delayBufferMutex,0);
	};

	DelayBuffer::~DelayBuffer() {
		LWP_MutexDestroy(delayBufferMutex);	
	};

}
