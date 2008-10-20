#pragma once
#ifndef ONEPLAYBACKUP_H
#define ONEPLAYBACKUP_H

#include "InfiniteSoundSource.h"
#include "SoundSink.h"

namespace HSound {

	//proxy class that will cover for the source if it runs low on (or more likely, out of) samples

	class OnePlayBackup :	public InfiniteSoundSource, public SoundSink {
	public:
		OnePlayBackup(void);
		OnePlayBackup(SoundSource *);
		virtual ~OnePlayBackup(void);

		virtual unsigned int loadNextBuffer(SoundSample *buffer, unsigned int length) throw();
	};

}
#endif
