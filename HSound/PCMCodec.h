#ifndef PCMCODEC_H
#define PCMCODEC_H

#include "Codec.h"

#include <cstring>

namespace HSound {

	class FileParser;
	class PCMCodec : public Codec {
	public:
		PCMCodec(FileParser *,unsigned int channels,unsigned int sampleSize);
		virtual ~PCMCodec();
		virtual unsigned int loadNextBuffer(SoundSample *buffer, unsigned int length) throw();
	private:
		FileParser *parser;
		char *readBuffer;
		size_t readBufferLength;
		unsigned int channels;
		unsigned int sampleSize;
	};
};
#endif
