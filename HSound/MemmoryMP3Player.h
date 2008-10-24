#pragma once
#ifndef MEMMORYMP3PLAYER_H
#define MEMMORYMP3PLAYER_H

#include "newMP3Player.h"

namespace HSound {

	class MemmoryMP3Player : public newMP3Player {
	public:
		MemmoryMP3Player(const u8 *buffer, const unsigned int bufferLength);
		virtual ~MemmoryMP3Player(void);

		//note: length is intentionaly non const!
		virtual unsigned int readFile(u8 *buffer, unsigned int length);//length is in bytes

		virtual void seek(const unsigned int pos);

	private:
		unsigned int readPos;
		const u8 *buffer;
		const unsigned int bufferLength;
	};

};
#endif
