#pragma once

#ifndef SOUNDGENERATOR_H
#define SOUNDGENERATOR_H
#include "InfiniteSoundSource.h"

namespace HSound {

	class SineGenerator :	public InfiniteSoundSource {
	public:
		SineGenerator(void);
		virtual ~SineGenerator(void);
		virtual unsigned int loadNextBuffer(SoundSample *buffer,unsigned int length) throw();
		float step;
		float offset;
	};
};
#endif
