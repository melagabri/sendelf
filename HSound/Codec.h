#ifndef CODEC_H
#define CODEC_H

#include "SoundSource.h"

namespace HSound {

	class FileParser;
	class Codec : public SoundSource {
	public:
		Codec(FileParser *);
		Codec();
		virtual ~Codec();
		virtual unsigned int loadNextBuffer(SoundSample *buffer, unsigned int length) throw()=0;
	protected:
		FileParser *parser;
	};

};
#endif
