#pragma once

#include "InfiniteSoundSource.h"

namespace HSound {

	class SilenceGenerator : public InfiniteSoundSource {
	public:
		SilenceGenerator(void);
		virtual ~SilenceGenerator(void);
		virtual unsigned int loadNextBuffer(SoundSample *buffer,unsigned int length) throw();
	};

};
