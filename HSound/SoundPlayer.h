#pragma once
#ifndef SOUNDPLAYER_H
#define SOUNDPLAYER_H

#include "SoundSource.h"

namespace HSound {

	class SoundPlayer :	public SoundSource {
	public:
		SoundPlayer(void);
		virtual ~SoundPlayer(void);
		bool soundComplete;
		virtual unsigned int readFile(u8 *buffer, unsigned int length)=0;//length is in bytes
	};

};
#endif
