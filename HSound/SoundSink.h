#pragma once
#ifndef SOUNDSINK_H
#define SOUNDSINK_H

#include <ogc/mutex.h>

namespace HSound {

	class SoundSource;

	class SoundSink {
	public:
		SoundSink(void);
		virtual ~SoundSink(void);
		virtual void SetSource(SoundSource *);
	protected:
		mutex_t sourceMutex;
		SoundSource *source;
	};

}
#endif
