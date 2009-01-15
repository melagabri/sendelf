#include "SoundPlayer.h"
#include "EofException.h"

#include "WavParser.h"
#include "RawParser.h"
#include "MP3Codec.h"

namespace HSound {

	SoundPlayer::SoundPlayer(FileReader *reader_p,FileType type) : soundComplete(false),reader(reader_p) {
		if(reader_p==0) {
			throw "The reader can not be null!";
		}

		switch(type) {
			case WAV:
				parser=new WavParser(this);
			break;

			case MP3:
				//TODO: write a nice MP3 file parser that does ID3
				codec=new MP3Codec();
				parser=new RawParser(this);
			break;

			default:
				throw "Unknown filetype";
			break;
		}
	};

	SoundPlayer::~SoundPlayer(void) {
		delete parser;
		delete resampler;
		delete codec;
		reader=0;
	};

	unsigned int SoundPlayer::loadNextBuffer(SoundSample *outBuffer,unsigned int length) throw () {
		return resampler->loadNextBuffer(outBuffer,length);
	};

	void SoundPlayer::createResampler(unsigned int sampleRate) {
		resampler=new Resampler(parser,sampleRate);
	}

}
