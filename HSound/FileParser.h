#ifndef FILEPARSER_H
#define FILEPARSER_H

#include "SoundSource.h"

#include <cstring>

namespace HSound {
	class FileReader;
	class FileParser : public SoundSource {
	public:
		FileParser(FileReader *reader);
		virtual ~FileParser();
		virtual unsigned int loadNextBuffer(SoundSample *buffer, unsigned int length) throw()=0;
		virtual unsigned int readSoundData(char *,size_t)=0;
	protected:
		FileReader *reader;
	};
};
#endif
