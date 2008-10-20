#include "OnePlayBackup.h"

namespace HSound {

	OnePlayBackup::OnePlayBackup(void) {
	};

	OnePlayBackup::OnePlayBackup(SoundSource *s) {
		SetSource(s);
	};

	OnePlayBackup::~OnePlayBackup(void) {
	};

	unsigned int OnePlayBackup::loadNextBuffer(HSound::SoundSample *buffer, unsigned int length) throw() {
		unsigned int readLength;

		LWP_MutexLock(sourceMutex);

		if(source) {
			readLength=source->loadNextBuffer(buffer,length);
		} else {
			readLength=0;
		}

		LWP_MutexUnlock(sourceMutex);

		for(;readLength<length;++readLength) {
			buffer[readLength]=0;
			//*((char*)1)=1;
		}
		return length;
	}

};
