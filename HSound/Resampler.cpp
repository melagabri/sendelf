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
		resampleInBufferContentLength(0),
		resampleInBufferLength(0),
		resampleOutBufferLength(0),
		resampleOutBufferContentLength(0),
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
		float *resampleInBufferPos;

		unsigned int goalReadBufferLength=inLength*2;

		if(readBufferLength<goalReadBufferLength) {
			delete[] readBuffer;
			readBuffer= new SoundSample[goalReadBufferLength];
			readBufferLength=goalReadBufferLength;
		}

		unsigned int goalResampleInBufferLength=goalReadBufferLength+resampleInBufferContentLength;

		if(resampleInBufferLength<goalResampleInBufferLength) {
			float *newResampleInBuffer= new float[goalResampleInBufferLength];

			//note: this loop should itterate exactly zero times
			//if the old buffer doesn't exist, meaning that there
			//is no need for a condition to prevent accessing bad memory
			resampleInBufferPos=newResampleInBuffer;
			for(unsigned int i=0;i<resampleInBufferContentLength;++i) {
				*(resampleInBufferPos++)=resampleInBuffer[i];
			}

			delete[] resampleInBuffer;
			resampleInBuffer=newResampleInBuffer;
		}

		unsigned int goalResampleOutBufferLenght=outLength*2+resampleOutBufferContentLength;

		if(resampleOutBufferLength<goalResampleOutBufferLenght) {
			
			float *newResampleOutBuffer = new float[goalResampleOutBufferLenght];

			//note: this loop should itterate exactly zero times
			//if the old buffer doesn't exist, meaning that there
			//is no need for a condition to prevent accessing bad memory
			resampleOutBufferPos=newResampleOutBuffer;
			for(unsigned int i=0;i<resampleOutBufferContentLength;++i) {
				*(resampleOutBufferPos++)=resampleOutBuffer[i];
			}

			delete[] resampleOutBuffer;
			resampleOutBuffer=newResampleOutBuffer;


			resampleOutBufferLength=goalResampleOutBufferLenght;
			resampleOutBufferPos=resampleOutBuffer+resampleOutBufferContentLength;
		}

		LWP_MutexLock(sourceMutex);
		unsigned int readLength = source->loadNextBuffer(readBuffer,inLength);
		LWP_MutexUnlock(sourceMutex);

		
		for( unsigned int i=0;i<readLength;++i) {
			*(resampleInBufferPos++)=readBuffer[i].left;
			*(resampleInBufferPos++)=readBuffer[i].right;
		}

		resampleInBufferContentLength+=readLength*2;

		resamplingData.data_in=resampleInBuffer;
		resamplingData.input_frames=(resampleInBufferContentLength)/2;

		resamplingData.output_frames=resampleOutBufferLength/2;
		resamplingData.data_out=resampleOutBufferPos;

		resamplingData.end_of_input=readLength!=inLength;


		int SRCstatus=src_process(resamplingState,&resamplingData);

		if(SRCstatus!=0) {
			//_break();
			throw BadSRCStatus();
		}

		//move the unused read and typecasted samples to the begining of the array
		//no need to do it if it would be moved to it's current location
		unsigned int slackSize=resampleInBufferContentLength-resamplingData.input_frames_used*2;
		if(resamplingData.input_frames_used!=0) {
			float *slackPos=resampleInBuffer+(resamplingData.input_frames_used*2);
			float *endOfInBufferContents=resampleInBuffer+resampleInBufferContentLength;
			while(slackPos<endOfInBufferContents) {
				*(resampleInBufferPos++)=*(slackPos++);
			}
		}
		resampleInBufferContentLength=slackSize;

		//with that out of the way, let's actualy write to the output array!
		unsigned int writeLength=resampleOutBufferContentLength/2;
		if(writeLength > outLength) {
			writeLength=outLength;
		}
		for(unsigned int i=0;i<writeLength;++i) {
			outBuffer[i].left= *(resampleOutBuffer++);
			outBuffer[i].right= *(resampleOutBuffer++);
		}


		//now we save what we didn't get to write to the output array this time
		//we will just move it to the begining of the buffer
		slackSize=resampleOutBufferContentLength-outLength*2;
		if(slackSize>0) {
			float *slackPos=resampleOutBuffer+slackSize;
			float *endOfOutBufferContents=resampleOutBuffer+resampleOutBufferContentLength;
			while(slackPos<endOfOutBufferContents) {
				*(resampleOutBufferPos++)= *(slackPos++);
			}

		}
		resampleOutBufferContentLength=slackSize;

		//oh, and we gotta return how much we wrote to the buffer, in samples
		return writeLength;

	}

}//end of namespace
