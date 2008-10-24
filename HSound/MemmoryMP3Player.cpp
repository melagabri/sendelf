#include "MemmoryMP3Player.h"

#include <cstring>
#include <stdexcept>
#include <debug.h>

using std::memcpy;

namespace HSound {

	MemmoryMP3Player::MemmoryMP3Player(const u8 *buffer_p,const unsigned int bufferLength_p) : buffer(buffer_p), bufferLength(bufferLength_p), readPos(0) {
	}

	MemmoryMP3Player::~MemmoryMP3Player(void) {
	}

	unsigned int MemmoryMP3Player::readFile(u8 *outBuffer, unsigned int readLength) {
		if(readPos>bufferLength) {
			if(readPos-readLength>=bufferLength) {
				_break();
			}
				//normal reading off the buffer
			return 0;
		}
		if(readPos+bufferLength-1<readLength) {
			readLength=readPos+bufferLength-1;
		}
		memcpy(outBuffer,buffer+readPos,readLength);

		readPos+=readLength;

		return readLength;	
	}

	void MemmoryMP3Player::seek(const unsigned int pos) {
		readPos=pos;
		newMP3Player::seek(pos);
	}

};
