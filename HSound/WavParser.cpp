#include "WavParser.h"
#include "FileReader.h"
#include "SoundPlayer.h"
#include "PCMCodec.h"

#include <gctypes.h>

namespace HSound {

	WavParser::WavParser(SoundPlayer *player, FileReader *reader_p) : FileParser(reader_p),dataChunkPos(0),readPos(0) {
		if(player==0) {
			throw "player can not be null!";
		}

		char fileHeader[12];
		reader->readAbs(0,fileHeader,sizeof(fileHeader));

		if(fileHeader[0]!='R' || fileHeader[1]!='I' || fileHeader[2]!='F' || fileHeader[3]!='F') {
			throw "Invalid RIFF file, lacks signature";
		}

		if(fileHeader[8]!='W' || fileHeader[9]!='A' || fileHeader[10]!='V' || fileHeader[11]!='E') {
			throw "Invalid WAVE file, lacks signature";
		}

		//now hunt for the fmt chunk


		size_t chunkPos;
		
		try {
			chunkPos=findChunk( "fmt " );
		} catch(EOFException) {
			throw "Invalid WAVE file, lacks fmt chunk";
		}

		reader->readAbs(chunkPos+8,(char*)&format,sizeof(format));

		switch(format.wFormatTag) {
			case 1:
				union {
					char bits_char[2];
					unsigned int bits_int : 16;
				};
				reader->readAbs(chunkPos+8+sizeof(format),bits_char,sizeof(bits_char));
				player->codec=new PCMCodec(this,format.wChannels,bits_int);
				player->createResampler(format.dwSamplesPerSec);
			break;

			default://todo: write a class for this one
				throw "Unsupported codec used";
			break;
		}

		try {
			chunkPos=findChunk("wavl");
			simpleFile=false;
			throw "Author too lazy to implement wavl WAVE file support, get a simple file thanks";
		} catch(EOFException) {
			simpleFile=true;
		}

	};

	size_t WavParser::findChunk(const char *chunkID) throw(EOFException) {
		size_t chunkPos=12;//end of the file header
		for(;;) {
			char chunkHeader[8];
			reader->readAbs(chunkPos,chunkHeader,sizeof(chunkHeader));

			for(unsigned register int i=0;i<4;++i) {
				if(chunkHeader[i]!=chunkID[i]) {
					goto nextchunk;
				}
			}
			return chunkPos;

nextchunk:
			size_t chunkSize=(u32)(chunkHeader+4);
			chunkSize+=8;//the field does not account for the lenght of the header
			chunkPos+=chunkSize;
			//if this was the last chunk, the filereader will throw an exception and get us out of here
		}

	};

	unsigned int WavParser::loadNextBuffer(SoundSample *buffer, unsigned int length) throw() {
		return codec->loadNextBuffer(buffer,length);
	};

	unsigned int WavParser::getSampleRate() const {
		return format.dwSamplesPerSec;
	}

	WavParser::~WavParser() {};

	size_t WavParser::readSoundData(char *outBuff, std::size_t readLength) {
		//assumption: only called by the main codec, not any silence dummy codec
		if(dataChunkPos==0) {
			dataChunkPos=findChunk("data");//TODO: verify capitialization
		}

		size_t endPos=readPos+readLength;
		if(endPos > reader->size) {
			readLength=reader->size-readPos;
		}

		reader->readAbs(readPos,outBuff,readLength);

		return readLength;
	};
};
