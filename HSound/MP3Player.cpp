#include "MP3Player.h"

#include <cstring>
#include <climits>

#include <debug.h>

namespace HSound {

	MP3Player::MP3Player(void) : 
		Status(0),
		FrameCount(0),
		slackBufferContentLength(0),
		readPos(0),
		slackBuffer(),
		sampleBuffer(),
		resampleInputBufferLength(0),
		resampleInputBuffer(0),
		resampleSlackBuffer(),
		resampleOutputBuffer(0),
		resampleOutputBufferLength(0),
		noMoreSamples(false) {

		mad_stream_init(&Stream);
		mad_frame_init(&Frame);
		mad_synth_init(&Synth);
		mad_timer_reset(&Timer);

		resamplingState=src_new(SRC_ZERO_ORDER_HOLD , 2, 0);

		if(resamplingState==0) {
			throw std::exception();//"src_new failed!"
		}
	};

	MP3Player::~MP3Player(void) {
		mad_synth_finish(&Synth);
		mad_frame_finish(&Frame);
		mad_stream_finish(&Stream);

		resamplingState=src_delete(resamplingState);

		delete[] resampleInputBuffer;
		delete[] resampleOutputBuffer;
	};

	unsigned int MP3Player::loadNextBuffer(SoundSample *outBuffer,unsigned int length) throw() {

		sampleBuffer.reserve(length);

		unsigned int sampleBufferPos=0;

		//load any pre computed samples we have
		for(;sampleBufferPos<slackBufferContentLength;++sampleBufferPos) {
			sampleBuffer[sampleBufferPos]=slackBuffer[sampleBufferPos];
		}

		unsigned char	*GuardPtr=0;
		soundComplete=false;

		unsigned int buferrs=0;

		while (sampleBufferPos<length && !noMoreSamples) {
			if(Stream.buffer==NULL || Stream.error==MAD_ERROR_BUFLEN) {
				size_t ReadSize,TargetReadSize;

				if(Stream.next_frame!=NULL) {//we jump back to not skip the unused parts from last time
					size_t Remaining=Stream.bufend-Stream.next_frame;
					readPos-=Remaining;
				}

				TargetReadSize=inputbufferLength;

				ReadSize=readFile(InputBuffer,readPos,TargetReadSize);
				readPos+=ReadSize;

				//see if we just ran out of buffer
				soundComplete=ReadSize<TargetReadSize;

				if(soundComplete) {
					GuardPtr=InputBuffer+ReadSize;
					memset(GuardPtr,0,MAD_BUFFER_GUARD);
					ReadSize+=MAD_BUFFER_GUARD;
				}

				//now it's not our problem anymore. You wish!
				mad_stream_buffer(&Stream,InputBuffer,ReadSize);

				

				Stream.error=MAD_ERROR_NONE;
			}

			

			if(mad_frame_decode(&Frame,&Stream)) {
				if(MAD_RECOVERABLE(Stream.error)) {
					// Do not print a message if the error is a loss of
					// synchronization and this loss is due to the end of
					// stream guard bytes.
					if(Stream.error!=MAD_ERROR_LOSTSYNC || Stream.this_frame!=GuardPtr) {
						/*fprintf(stderr,"%s: recoverable frame level error (%s)\n",
								ProgName,MadErrorString(&Stream));
						fflush(stderr);*/
					}
					continue;
				} else if(Stream.error==MAD_ERROR_BUFLEN) {
					if(++buferrs>20000) *((char*)1)=1;//dam, we are in an infinite loop
					continue;
				} else {
					/*fprintf(stderr,"%s: unrecoverable frame level error (%s).\n",
								ProgName,MadErrorString(&Stream));*/
					Status=1;
					break;
				}
			}//end of if mad_frame_decode

			FrameCount++;
			mad_timer_add(&Timer,Frame.header.duration);

			//magic filtering is not implemented

			mad_synth_frame(&Synth,&Frame);

			unsigned int slackSize=resampleSlackBuffer.size();

			unsigned int targetResampleInputBufferLength=Synth.pcm.length*2+slackSize;

			if(resampleInputBufferLength<targetResampleInputBufferLength && resampleInputBuffer) {
				//save the old buffer pointer and get a new buffer
				float *oldBuf=resampleInputBuffer;
				unsigned int oldLength=resampleInputBufferLength;
				resampleInputBufferLength=targetResampleInputBufferLength;
				resampleInputBuffer=new float[resampleInputBufferLength];

				//re add the old contents
				for(unsigned int i=0;i<oldLength;++i) {
					resampleInputBuffer[i]=oldBuf[i];
				}

				delete[] oldBuf;
			} else if(!resampleInputBuffer) {
				resampleInputBufferLength=targetResampleInputBufferLength;
				resampleInputBuffer=new float[resampleInputBufferLength];
			}

			unsigned int resampleInputBufferPos=0;

			//copy the stuff that wasn't used the last time we resampled into the input buffer
			list<float>::const_iterator endItr=resampleSlackBuffer.end();
			for(list<float>::const_iterator itr=resampleSlackBuffer.begin();itr!=endItr;++itr) {
				float s=*itr;
				resampleInputBuffer[resampleInputBufferPos++]=s;
			}
			
			//load the just now synthed samples into the resample buffer
			for(unsigned int i=0;i<Synth.pcm.length;i++) {
				signed short left,right;

				left=MadFixedToSshort(Synth.pcm.samples[0][i]);
				
				if(MAD_NCHANNELS(&Frame.header)==2) {
					right=MadFixedToSshort(Synth.pcm.samples[1][i]);
				} else {
					right=left;
				}

				//save it for resampling
				resampleInputBuffer[resampleInputBufferPos++]=left;
				resampleInputBuffer[resampleInputBufferPos++]=right;
			
			}//end of for


			unsigned int targetResampleOutputBufferLength=length*2;

			if(targetResampleOutputBufferLength>resampleOutputBufferLength && resampleOutputBuffer) {
				delete[] resampleOutputBuffer;
				resampleOutputBuffer=0;
			}

			if(!resampleOutputBuffer) {
				resampleOutputBuffer=new float[targetResampleOutputBufferLength];
				resampleOutputBufferLength=targetResampleOutputBufferLength;
			}

			noMoreSamples=Stream.error==MAD_ERROR_LOSTSYNC && Stream.this_frame==GuardPtr && EOF;

			SRC_DATA resamplingData;

			resamplingData.input_frames  = resampleInputBufferPos/2;
			resamplingData.input_frames_used = 0;
			resamplingData.output_frames_gen = 0;
			resamplingData.data_in       = resampleInputBuffer;
			resamplingData.end_of_input  = noMoreSamples;
			resamplingData.data_out      = resampleOutputBuffer;
			resamplingData.output_frames = resampleOutputBufferLength/2;
			//recomputed because vbr will change the bitrate
			resamplingData.src_ratio     = (48000.0)/((double)Frame.header.samplerate);

			/*endItr=resampleSlackBuffer.end();
			buferrs=0;
			for(list<float>::const_iterator itr=resampleSlackBuffer.begin();itr!=endItr;++itr) {
				buferrs++;
			}*/

			int SRCstatus=src_process(resamplingState,&resamplingData);

			if(SRCstatus!=0) _break();

			resampleSlackBuffer.clear();

			for(int i=resamplingData.input_frames_used;i<resamplingData.input_frames;i++) {
				resampleSlackBuffer.push_back(resampleInputBuffer[i]);
			}

			//copy and convert the resampled audio to the output buffer
			for(int i=0;i<resamplingData.output_frames_gen;) {
				unsigned short left,right;
				left =resampleOutputBuffer[i++];
				right=resampleOutputBuffer[i++];
				sampleBuffer[sampleBufferPos++]=SoundSample(left,right);
			}
		

		}//end of while

		//now load the real output buffer with "the output buffer"
		for(unsigned int i=0;i<length;i++) {
			outBuffer[i]=sampleBuffer[i];
		}

		//we might have a few extra samples, let's save those so stuff doesn't skip
		if(sampleBufferPos>=length) {
			slackBufferContentLength=sampleBufferPos-length;

			slackBuffer.reserve(slackBufferContentLength);

			//save the slack that accidently got decoded
			unsigned int sampleBufferLength=sampleBuffer.size();
			for(unsigned int i=0;sampleBufferPos<sampleBufferLength;++i) {

				slackBuffer[i]=sampleBuffer[sampleBufferPos++];
			}
		}

		//cap the return value
		return sampleBufferPos>length?length:sampleBufferPos;

	};//end of member function loadNextBuffer



	void MP3Player::seek(const unsigned int pos) {
		readPos=pos;
		soundComplete=false;
		noMoreSamples=false;
		src_reset(resamplingState);
	}


};//end of namespace

signed short MadFixedToSshort(mad_fixed_t Fixed)	{
	// A fixed point number is formed of the following bit pattern:
	//
	// SWWWFFFFFFFFFFFFFFFFFFFFFFFFFFFF
	// MSB                          LSB
	// S ==> Sign (0 is positive, 1 is negative)
	// W ==> Whole part bits
	// F ==> Fractional part bits
	//
	// This pattern contains MAD_F_FRACBITS fractional bits, one
	// should alway use this macro when working on the bits of a fixed
	// point number. It is not guaranteed to be constant over the
	// different platforms supported by libmad.
	//
	// The signed short value is formed, after clipping, by the least
	// significant whole part bit, followed by the 15 most significant
	// fractional part bits. Warning: this is a quick and dirty way to
	// compute the 16-bit number, madplay includes much better
	// algorithms.
	 

	// Clipping
	if(Fixed>=MAD_F_ONE)
		return(SHRT_MAX);
	if(Fixed<=-MAD_F_ONE)
		return(-SHRT_MAX);

	// Conversion.
	Fixed=Fixed>>(MAD_F_FRACBITS-15);
	return((signed short)Fixed);
};
