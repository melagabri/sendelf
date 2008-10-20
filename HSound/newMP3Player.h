#pragma once
#ifndef newMP3PLAYER_H
#define newMP3PLAYER_H
#include "SoundPlayer.h"

#include <mad/mad.h>
#include <samplerate.h>

#include <boost/noncopyable.hpp>

#include <vector>
#include <list>
#include <deque>
#include <exception>

//code heavily borrowed from madld, under a bsd license

namespace HSound {

	using std::vector;
	using std::list;
	using std::deque;

	class newMP3Player :	public SoundPlayer, public boost::noncopyable {
	public:
		newMP3Player();
		~newMP3Player();

		virtual void seek(const unsigned int pos);

	private:

		class EndOfInput : public std::exception {};//todo, make it extend some base exception class
		class BadFrame : public std::exception {};
		class BadSRCStatus : public std::exception {};

		struct mad_stream	Stream;
		struct mad_frame	Frame;
		struct mad_synth	Synth;

		SRC_STATE* resamplingState;
		SRC_DATA resamplingData;

		bool setSampleRate;
		double inputPerOutputRatio;

		size_t readPos;
		static const unsigned int readBufferLength = 5*8192;//in bytes
		u8 readBuffer[readBufferLength+MAD_BUFFER_GUARD];
		u8 *GuardPtr;
		bool readLastFrame;

		deque< std::pair<float,float> > synthedSamples;
		deque<SoundSample> outputBuffer;

		unsigned int resampleInBufferLength;
		float *resampleInBuffer;

		unsigned int resampleOutBufferLength;
		float *resampleOutBuffer;

		bool decodeMP3Frame() throw(EndOfInput,BadFrame);
		void readMP3Frame() throw(EndOfInput);
		void synthSamples() throw(EndOfInput,BadFrame);
		virtual unsigned int loadNextBuffer(SoundSample *buffer,unsigned int length) throw();//length is in samples
		void generateSamples(unsigned int minLength) throw(EndOfInput,BadFrame,BadSRCStatus);
	};
}

signed short MadFixedToSshort(mad_fixed_t Fixed);
#endif
