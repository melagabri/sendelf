#ifndef WAVPARSER_H
#define WAVPARSER_H

#include "FileParser.h"
#include "FileReader.h"

#include <cstring>

namespace HSound {
	class SoundPlayer;
	class Codec;

	class WavParser : public FileParser {
	public:
		WavParser(SoundPlayer *player,FileReader *);
		virtual ~WavParser();
		virtual unsigned int loadNextBuffer(SoundSample *buffer, unsigned int length) throw();
		virtual unsigned int readSoundData(char *,size_t);
		virtual unsigned int getSampleRate() const;
	private:
		size_t findChunk(const char*) throw(EOFException);

		struct fmt {
			unsigned int wFormatTag : 16;
			unsigned int wChannels : 16;
			unsigned int dwSamplesPerSec : 32; 
			unsigned int dwAvgBytesPerSec : 32;
			unsigned int wBlockAlign : 16;
		};

		fmt format;
		Codec *codec;

		bool simpleFile;

		size_t readPos;
		size_t dataChunkPos;
	};
};
#endif
