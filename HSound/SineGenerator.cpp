#include "SineGenerator.h"

#include <cmath>

namespace HSound {

	SineGenerator::SineGenerator(void) : offset(0) {
	}

	SineGenerator::~SineGenerator(void) {
	};

	unsigned int SineGenerator::loadNextBuffer(SoundSample *buff,unsigned int length) throw() {
		for(unsigned int i=0;i<length;++i) {
			signed short s=(sin(offset+=step)+1)*255;
			buff[i].left=s;
			buff[i].right=s;
		}
		return length;
	};

};
