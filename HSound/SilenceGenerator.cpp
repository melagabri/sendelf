#include "SilenceGenerator.h"

#include <cstring>

namespace HSound {

	using::memset;

	SilenceGenerator::SilenceGenerator(void) {
	};

	SilenceGenerator::~SilenceGenerator(void) {
	};

	unsigned int SilenceGenerator::loadNextBuffer(SoundSample *buffer, unsigned int length) throw() {
		for(unsigned int i=0;i<length;++i) {
			buffer[i].left=0;
			buffer[i].right=0;
		}
		return length;
	};

};
