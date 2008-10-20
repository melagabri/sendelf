#pragma once
#ifndef MP3PLAYER_H
#define MP3PLAYER_H
#include "SoundPlayer.h"

#include <mad/mad.h>
#include <samplerate.h>

#include <boost/noncopyable.hpp>

#include <vector>
#include <list>

//code heavily borrowed from madld, under a bsd license

namespace HSound {

	using std::vector;
	using std::list;

	class MP3Player :	public SoundPlayer, public boost::noncopyable {
	public:
		MP3Player(void);
		virtual ~MP3Player(void);
		virtual unsigned int loadNextBuffer(SoundSample *buffer,unsigned int length) throw();//length is in samples

		virtual void seek(const unsigned int pos);

	private:

		static const unsigned int inputbufferLength = 5*8192;//in bytes

		vector<SoundSample> slackBuffer;//resampled samples that was generated a little too early
		unsigned int slackBufferContentLength;
		vector<SoundSample> sampleBuffer;//final resampled samples

		unsigned int readPos;

		bool noMoreSamples;

		static signed short MadFixedToSshort(mad_fixed_t Fixed);

		//decoding structs
		struct mad_stream	Stream;
		struct mad_frame	Frame;
		struct mad_synth	Synth;

		mad_timer_t	Timer;
		unsigned char	InputBuffer[inputbufferLength+MAD_BUFFER_GUARD];

		int	Status;
		unsigned long	FrameCount;

		//some resampling stuff

		SRC_STATE* resamplingState;

		unsigned int resampleInputBufferLength;
		float *resampleInputBuffer;

		float *resampleOutputBuffer;
		unsigned int resampleOutputBufferLength;

		list<float> resampleSlackBuffer;//stuff not used last time we resampled

	};

};
#endif
