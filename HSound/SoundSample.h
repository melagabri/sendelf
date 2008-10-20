#pragma once

#ifndef SOUNDSAMPLE_H
#define SOUNDSAMPLE_H

namespace HSound {

	class SoundSample {
	public:
		unsigned short left,right;

		void operator +=(const SoundSample &s2);
		void operator -=(const SoundSample &s2);
		void operator *=(const SoundSample &s2);

		void operator +=(const signed short s);
		void operator -=(const signed short s);
		void operator *=(const signed short s);

		void operator=(const SoundSample &s2);
		void operator=(const signed short s);

		SoundSample();
		SoundSample(const signed short left,const signed short right);
		~SoundSample();
	};

};
#endif
