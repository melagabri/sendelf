#include "SoundDumper.h"

void SoundDumper::doDump() {
	runDump=true;
};

SoundDumper::SoundDumper() : Canvas(width,256), HSound::SoundSource(), HSound::SoundSink(),runDump(false),scale(1) {
	clear(rgbaImage::pixel(10,10,10));
	flush();
};

unsigned int SoundDumper::loadNextBuffer(HSound::SoundSample *buffer, unsigned int length) throw() {
	LWP_MutexLock(sourceMutex);
	length = source->loadNextBuffer(buffer,length);
	LWP_MutexUnlock(sourceMutex);

	if(runDump) {
		runDump=false;

		clear(rgbaImage::pixel(10,10,10));

		unsigned int loopSpeed=length/width;

		for(unsigned int i = 0, j =0; i<width;++i,j+=loopSpeed) {
			this->vline(
				i,
				256-(buffer[j].left/scale),
				256,
				rgbaImage::pixel(255,0,0)
			);
		}
		flush();
	}

	return length;
};
