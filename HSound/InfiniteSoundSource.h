#pragma once
#ifndef INFINITESOUNDSOURCE_H
#define INFINITESOUNDSOURCE_H

#include "SoundSource.h"

namespace HSound {

	class InfiniteSoundSource :	public SoundSource {
	public:
		InfiniteSoundSource(void);
		virtual ~InfiniteSoundSource(void);
	};
}
#endif
