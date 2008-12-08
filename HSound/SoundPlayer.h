#pragma once
#ifndef SOUNDPLAYER_H
#define SOUNDPLAYER_H

#include "SoundSource.h"
#include "Resampler.h"
#include "FileReader.h"
#include "FileParser.h"
#include "FileType.h"

#include <deque>

namespace HSound {

	using std::deque;

	class Codec;

	class SoundPlayer :	public SoundSource {
	public:
		SoundPlayer(FileReader *reader,FileType type);
		virtual ~SoundPlayer(void);
		bool soundComplete;
		virtual unsigned int loadNextBuffer(SoundSample *buffer,unsigned int length) throw();//length is in samples
	private:
		Resampler *resampler;
		FileReader *reader;
		FileParser *parser;
		Codec *codec;

		void createResampler(unsigned int bitrate);

		friend class WavParser;

		deque<SoundSample> outputBuffer;
	};

};
#endif
