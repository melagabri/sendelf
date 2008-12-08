#include "Resampler.h"

#include <cmath>

void _break();

namespace HSound {

	Resampler::Resampler(SoundSource *s) :
		resampleInBuffer(0),
		resampleOutBufferLength(0),
		resampleOutBuffer(0),
		SoundSink(s),
		readBufferLength(0),
		readBuffer(0)
	{
			init(s->getSampleRate());
	};

	Resampler::Resampler(SoundSource *s,unsigned int samplerateOverride) :
		resampleInBuffer(0),
		resampleOutBufferLength(0),
		resampleOutBuffer(0),
		SoundSink(s),
		readBufferLength(0),
		readBuffer(0)
	{
		init(samplerateOverride);
	};

	void Resampler::init(unsigned int sampleRate) {
		resamplingState=src_new(SRC_ZERO_ORDER_HOLD , 2, 0);
		resamplingData.src_ratio=HSound::globalSampleRate/sampleRate;
		inputPerOutputRatio=sampleRate/HSound::globalSampleRate;
	};

	Resampler::~Resampler(void)	{
		src_delete(resamplingState);
		delete[] readBuffer;
		delete[] resampleInBuffer;
		delete[] resampleOutBuffer;
	};

	unsigned int Resampler::loadNextBuffer(HSound::SoundSample *outBuffer, unsigned int outLength) throw() {

		unsigned int inLength=int(ceil(double(outLength)*inputPerOutputRatio));

		if(readBufferLength<inLength*2) {
			delete[] readBuffer;
			readBuffer= new SoundSample[inLength*2];
			readBufferLength=inLength*2;

			delete[] resampleInBuffer;
			resampleInBuffer= new float[inLength*2];
		}

		if(resampleOutBufferLength<outLength*2) {
			delete[] resampleOutBuffer;
			resampleOutBuffer = new float[outLength*2];
			resampleOutBufferLength=outLength*2;
		}

		LWP_MutexLock(sourceMutex);
		unsigned int readLength = source->loadNextBuffer(readBuffer,inLength);
		LWP_MutexUnlock(sourceMutex);

		float *resampleInBufferPos;
		for( unsigned int i=0;i<readLength;++i) {
			*(resampleInBufferPos++)=readBuffer[i].left;
			*(resampleInBufferPos++)=readBuffer[i].right;
		}

		resamplingData.data_in=resampleInBuffer;
		resamplingData.input_frames=(resampleInBufferPos-resampleInBuffer)/2;

		resamplingData.output_frames=resampleOutBufferLength/2;
		resamplingData.data_out=resampleOutBuffer;

		resamplingData.end_of_input=readLength!=inLength;


		int SRCstatus=src_process(resamplingState,&resamplingData);

		if(SRCstatus!=0) {
			_break();
			throw BadSRCStatus();
		}
	}

}//end of namespace
