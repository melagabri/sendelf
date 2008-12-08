#include "SoundPlayer.h"
#include "EofException.h"

#include "WavParser.h"

namespace HSound {

	SoundPlayer::SoundPlayer(FileReader *reader_p,FileType type) : soundComplete(false),reader(reader_p) {
		if(reader_p==0) {
			throw "The reader can not be null!";
		}

		switch(type) {
			case WAV:
				parser=new WavParser(this,reader);
			break;

			case MP3:
				//TODO: write the skeleton stuff for this one

			default:
				throw "Unknown filetype";
			break;
		}
	};

	SoundPlayer::~SoundPlayer(void) {
		delete parser;
		delete resampler;
		reader=0;
	};

	unsigned int SoundPlayer::loadNextBuffer(SoundSample *outBuffer,unsigned int length) throw () {
		return resampler->loadNextBuffer(outBuffer,length);
	};

	void SoundPlayer::createResampler(unsigned int sampleRate) {
		resampler=new Resampler(parser,sampleRate);
	}

}
