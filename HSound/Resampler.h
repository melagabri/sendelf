#pragma once
#ifndef RESAMPLER_H
#define RESAMPLER_H

#include "SoundSource.h"

namespace HSound {

	class Resampler {
	public:
		Resampler(void);
		virtual ~Resampler(void);
		virtual unsigned int loadNextBuffer(SoundSample *buffer,unsigned int length);
	};
}
#endif
