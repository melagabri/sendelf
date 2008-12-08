#pragma once
#ifndef RESAMPLER_H
#define RESAMPLER_H

#include <samplerate.h>

#include "Globals.h"
#include "SoundSink.h"
#include "SoundSource.h"

#include <exception>

namespace HSound {

	class Codec;

	class Resampler : public SoundSink, public SoundSource {
	public:
		Resampler(SoundSource *);
		Resampler(SoundSource *,unsigned int samplerateOverride);
		virtual ~Resampler(void);
		virtual unsigned int loadNextBuffer(SoundSample *outBuffer,unsigned int length) throw ();

		class BadSRCStatus : public std::exception {};
	private:

		void init(unsigned int sampleRate);

		double inputPerOutputRatio;

		SRC_STATE* resamplingState;
		SRC_DATA resamplingData;

		float *resampleInBuffer;

		unsigned int resampleOutBufferLength;
		float *resampleOutBuffer;

		unsigned int readBufferLength;
		SoundSample *readBuffer;
	};
}
#endif
