#pragma once
#include "InfiniteSoundSource.h"
#include "SoundSink.h"

#ifndef SOUNDMIXER_H
#define SOUNDMIXER_H

#include <list>
#include <vector>
#include <ogc/mutex.h>

namespace HSound {

	using std::list;
	using std::vector;

	class SoundMixer;

	class SoundChannel : public SoundSink {
	public:
		float leftLevel;
		float rightLevel;
		float preAmp;

		SoundChannel();
		SoundChannel(SoundSource *source);
		SoundChannel(float left_Level, float rightLevel);
		SoundChannel(float left_Level, float rightLevel, float preAmp);
		SoundChannel(SoundSource *source, float left_Level, float rightLevel);
		SoundChannel(SoundSource *source, float left_Level, float rightLevel, float preAmp);
		~SoundChannel();
	private:
		SoundSample *mixerBuffer;
		unsigned int bufferLength;
		friend class SoundMixer;
	};

	//needed because it's a different bigger datatype than the normal sample class
	//I should probably merge it with the normal one using templates, but that's for another time
	class MixerSample {
	public:
		float right, left;

		MixerSample();
		MixerSample(float left, float right);

		void operator +=(const MixerSample &);
	};

	class SoundMixer : public InfiniteSoundSource {
	public:
		SoundMixer(void);
		virtual ~SoundMixer(void);
		virtual unsigned int loadNextBuffer(SoundSample *buffer,unsigned int length) throw();

		void addChannel(SoundChannel*);
		void removeChannel(SoundChannel*);

	private:
		mutex_t channelsMutex;
		list<SoundChannel*> channels;
		vector<MixerSample> mixerBuffer;
	};

};

#endif
