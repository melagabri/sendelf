
#include "newMP3Player.h"

//memset and company:
#include <cstring>

//uhm, I fotgot why
#include <climits>

//rounding
#include <cmath>

#include <debug.h>

#define NO_RESAMPLE

/*
This code is based on the madld sample library copyright to Bertrand Petit 2001-2004
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions	
  are met:													
 																		
  1. Redistributions of source code must retain the above copyright	
     notice, this list of conditions and the following disclaimer.
 																
  2. Redistributions in binary form must reproduce the above
     copyright notice, this list of conditions and the following
     disclaimer in the documentation and/or other materials provided
     with the distribution.
  													
  3. Neither the name of the author nor the names of its contributors
     may be used to endorse or promote products derived from this
     software without specific prior written permission.
 
  THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS''
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
  TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
  PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
  USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
  OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
  SUCH DAMAGE.
*/

namespace HSound {

	newMP3Player::newMP3Player() : resampleInBufferLength(0),
		resampleInBuffer(0),
		setSampleRate(false),
		resampleOutBufferLength(0),
		resampleOutBuffer(0),
		readLastFrame(false)
	{
		mad_stream_init(&Stream);
		mad_frame_init(&Frame);
		mad_synth_init(&Synth);
		resamplingState=src_new(SRC_ZERO_ORDER_HOLD , 2, 0);
	};

	newMP3Player::~newMP3Player() {
		mad_synth_finish(&Synth);
		mad_frame_finish(&Frame);
		mad_stream_finish(&Stream);
		src_delete(resamplingState);
	};

	bool newMP3Player::decodeMP3Frame() throw(EndOfInput,BadFrame) {

		if(Stream.buffer==NULL || Stream.error==MAD_ERROR_BUFLEN) {
			if(readLastFrame) {
				throw EndOfInput();
			} else {
				readMP3Frame();
			}
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
					_break();
				}
				return false;
			} else if(Stream.error==MAD_ERROR_BUFLEN) {
				return false;
			} else {
				/*fprintf(stderr,"%s: unrecoverable frame level error (%s).\n",
							ProgName,MadErrorString(&Stream));*/
				throw BadFrame();
			}
		}//end of if mad_frame_decode

		//FrameCount++;
		//mad_timer_add(&Timer,Frame.header.duration);

		return true;

	};

	void newMP3Player::synthSamples() throw(EndOfInput,BadFrame) {

		while(!decodeMP3Frame());//get us a frame!

		mad_synth_frame(&Synth,&Frame);

		for(unsigned int i=0;i<Synth.pcm.length;i++) {
			signed short left,right;

			left=MadFixedToSshort(Synth.pcm.samples[0][i]);
			
			if(MAD_NCHANNELS(&Frame.header)==2) {
				right=MadFixedToSshort(Synth.pcm.samples[1][i]);
			} else {
				right=left;
			}
#ifdef NO_RESAMPLE
			outputBuffer.push_front(SoundSample(left,right));
#else
			synthedSamples.push_front(std::make_pair(left,right));
#endif
		}
	}

	unsigned int newMP3Player::loadNextBuffer(SoundSample *outBuffer,unsigned int length) throw() {
		while(outputBuffer.size()<length) {
			try {
				generateSamples(length-outputBuffer.size());
			} catch (EndOfInput &) {
				soundComplete=true;
				break;
			}
		}

		deque<SoundSample>::const_reverse_iterator itr=outputBuffer.rbegin();
		deque<SoundSample>::const_reverse_iterator enditr=outputBuffer.rend();
		unsigned int i=0;
		for(;i<length && itr!=enditr;++i,++itr) {
			*(outBuffer++)=*itr;
		}

		outputBuffer.resize(outputBuffer.size()-i);

		return i;
	}

	void newMP3Player::generateSamples(unsigned int minLength) throw(EndOfInput,BadFrame,BadSRCStatus) {

		bool endOfInput=false;

		if(!setSampleRate) {
			synthSamples();//so that the frame header is valid
			resamplingData.src_ratio=(48000.0)/((double)Frame.header.samplerate);
			inputPerOutputRatio=((double)Frame.header.samplerate) / (48000.0);
			setSampleRate=true;
		}

		unsigned int resampleInputSize=int(ceil(double(minLength)*inputPerOutputRatio));

		do {//read in enough frames
			try {
				synthSamples();
			} catch (EndOfInput &) {
				endOfInput=true;
				break;
			} catch (...) {
				_break();
			}
#ifndef NO_RESAMPLE
		} while (synthedSamples.size()<resampleInputSize);

#else
		} while (false);
		return;
#endif

		unsigned int goalResampleInBufferLength=synthedSamples.size();

		unsigned int goalResampleOutBufferLength=int(ceil(
			double(goalResampleInBufferLength)* resamplingData.src_ratio
		));


		if(resampleInBufferLength<goalResampleInBufferLength) {
			delete[] resampleInBuffer;
			resampleInBuffer=new float[goalResampleInBufferLength];
			resampleInBufferLength=goalResampleInBufferLength;
		}

		if(resampleOutBufferLength<goalResampleOutBufferLength) {
			delete[] resampleOutBuffer;
			resampleOutBuffer=new float[goalResampleOutBufferLength];
			resampleOutBufferLength=goalResampleOutBufferLength;
		}

		deque<std::pair<float,float> >::const_reverse_iterator enditr=synthedSamples.rend();
		deque<std::pair<float,float> >::const_reverse_iterator itr=synthedSamples.rbegin();
		unsigned int resampleBufferPos=0;
		for(;itr!=enditr &&resampleBufferPos<resampleInputSize;++itr) {
			resampleInBuffer[resampleBufferPos++]=itr->first;
			resampleInBuffer[resampleBufferPos++]=itr->second;
		}

		if(resampleBufferPos-1>resampleInBufferLength) {
			_break();
		}

		resamplingData.data_in=resampleInBuffer;
		resamplingData.input_frames=resampleBufferPos/2;

		resamplingData.output_frames=resampleOutBufferLength/2;
		resamplingData.data_out=resampleOutBuffer;

		resamplingData.end_of_input=endOfInput;


		int SRCstatus=src_process(resamplingState,&resamplingData);

		if(SRCstatus!=0) {
			_break();
			throw BadSRCStatus();
		}

		unsigned int unused=synthedSamples.size()-resamplingData.input_frames_used;

		synthedSamples.resize(unused);
		resampleBufferPos=0;
		for(int i =0;i<resamplingData.output_frames_gen;++i) {
			unsigned short left=resampleOutBuffer[resampleBufferPos++];
			unsigned short right=resampleOutBuffer[resampleBufferPos++];

			outputBuffer.push_front(	SoundSample(left,right) );
		}

		if(endOfInput) {
			_break();
			throw EndOfInput();
		}

	}

	void newMP3Player::readMP3Frame() throw(EndOfInput) {
		if(readLastFrame) {
			throw EndOfInput();
		}

		unsigned int Remaining;
		u8 *ReadStart;
		unsigned int ReadSize;
		if(Stream.next_frame!=NULL) {
			Remaining=Stream.bufend-Stream.next_frame;
			memmove(readBuffer,Stream.next_frame,Remaining);
			ReadStart=readBuffer+Remaining;
			ReadSize=readBufferLength-Remaining;
		} else {
			ReadSize=readBufferLength;
			ReadStart=readBuffer;
			Remaining=0;
		}

		unsigned int readLen=readFile(readBuffer,ReadSize);//BstdRead(ReadStart,1,ReadSize,BstdFile);

		if(readLen<ReadSize) {
			GuardPtr=ReadStart+readLen;
			memset(GuardPtr,0,MAD_BUFFER_GUARD);
			ReadSize+=MAD_BUFFER_GUARD;
			readLastFrame=true;
		}

		mad_stream_buffer(&Stream,readBuffer,ReadSize+Remaining);
		Stream.error=(mad_error)0;
	}

	void newMP3Player::seek(const unsigned int) {
		readLastFrame=false;
		synthedSamples.clear();
		outputBuffer.clear();
		src_reset(resamplingState);
	}

	/*void newMP3Player::readMP3Frame() {
		size_t leftover;
		if(Stream.next_frame!=NULL) {
			leftover=Stream.bufend-Stream.next_frame;
		} else {
			leftover=0;
		}
	}*/
			
		
}
