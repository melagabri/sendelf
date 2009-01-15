#ifndef FILEPARSER_H
#define FILEPARSER_H

#include "SoundSource.h"

#include <cstring>

namespace HSound {
	class SoundPlayer;
	class FileParser : public SoundSource {
	public:
		FileParser(SoundPlayer *);
		virtual ~FileParser();
		virtual unsigned int loadNextBuffer(SoundSample *buffer, unsigned int length) throw()=0;
		virtual unsigned int readSoundData(char *,size_t)=0;
	protected:
		SoundPlayer *player;
	};
};
#endif
