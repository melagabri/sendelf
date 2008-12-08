#include "PCMCodec.h"
#include "FileParser.h"

namespace HSound {
	PCMCodec::PCMCodec(
		HSound::FileParser *p,
		unsigned int channels_p,
		unsigned int sampleSize_p
	) : Codec(p), readBuffer(0), readBufferLength(0), channels(channels_p), sampleSize(sampleSize_p) {
		if(channels==0) {
			throw "You need at least one channel";
		}
	};

	PCMCodec::~PCMCodec() {};

	unsigned int PCMCodec::loadNextBuffer(SoundSample *buffer, unsigned int length) throw() {
		const size_t sampleLength=channels * sampleSize;
		const size_t bytesToRead=sampleLength * length;

		if(readBufferLength<bytesToRead) {
			readBufferLength=0;
			delete[] readBuffer;
			readBuffer=new char[bytesToRead];
			readBufferLength=bytesToRead;
		}

		const size_t readLength=parser->readSoundData(readBuffer,bytesToRead);

		size_t i=0;
		unsigned int j=0;
		for(;i<readLength;i+=sampleLength,++j) {
			signed short left,right;
			if(sampleSize==8) {
				left=readBuffer[i]<<8-1<<7;
				if(channels==1) {
					right=left;
				} else {
					right=readBuffer[i+1]<<8-1<<7;
				}					
			} else {
				left=*(reinterpret_cast<signed short*>(readBuffer+i));
				if(channels==2) {
					right=*(reinterpret_cast<signed short*>(readBuffer+i+sampleSize));
				} else {
					right=left;
				}
			}

			buffer[j]=SoundSample(left,right);

		}//end of for

		return readLength/sampleLength;
	};

};
