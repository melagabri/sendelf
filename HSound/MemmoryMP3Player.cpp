#include "MemmoryMP3Player.h"

#include <cstring>
#include <stdexcept>
#include <debug.h>

using std::memcpy;

namespace HSound {

	MemmoryMP3Player::MemmoryMP3Player(const u8 *buffer_p,const unsigned int bufferLength_p) : buffer(buffer_p), bufferLength(bufferLength_p) {
	}

	MemmoryMP3Player::~MemmoryMP3Player(void) {
	}

	unsigned int MemmoryMP3Player::readFile(u8 *outBuffer, unsigned int ReadStart, unsigned int readLength) {
		if(ReadStart>bufferLength) {
			if(ReadStart-readLength>=bufferLength) {
				_break();
			}
				//normal reading off the buffer
			return 0;
		}
		if(ReadStart+bufferLength-1<readLength) {
			readLength=ReadStart+bufferLength-1;
		}
		memcpy(outBuffer,buffer+ReadStart,readLength);
		return readLength;	
	}

};
