#pragma once
#ifndef SOUNDSOURCE_H
#define SOUNDSOURCE_H

#include <gctypes.h>

#include "SoundSample.h"

namespace HSound {

	class SoundSource {
	public:
		SoundSource(void);
		virtual ~SoundSource(void);
		virtual unsigned int loadNextBuffer(SoundSample *buffer, unsigned int length) throw()=0;//length is in samples, not bytes!
	};

};
#endif
