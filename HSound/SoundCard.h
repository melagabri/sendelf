#pragma once
#ifndef SOUNDCARD_H
#define SOUNDCARD_H

#include <gctypes.h>
#include <ogc/lwp.h>

#include <ogc/mutex.h>

#include "InfiniteSoundSource.h"
#include "SoundSink.h"

namespace HSound {

	class SoundSource;

	class SoundCard : public SoundSink {
	public:
		SoundCard(void);
		~SoundCard(void);
		void SetLeftVolume(u8);
		void SetRightVolume(u8);
		void SetSource(SoundSource*);
	private:

		static const unsigned int bufferLength=600*32;
		static const unsigned int bufferCount=3;
		static const unsigned int threadPrio=80;

		static bool created;
		static void DMACallback(void);
		volatile unsigned int bufferID;
		u8 *buffers[bufferCount];

		lwp_t thread;
		lwpq_t threadQueue;


		static void *ThreadCallback(void *);
		unsigned int runThread();

		volatile bool continueToRunThread;

	};

	#ifndef MANUAL_SOUNDCARD_CREATION
	extern SoundCard soundCard;
	#endif

};

#endif
