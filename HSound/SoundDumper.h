#include "SoundSample.h"
#include "SoundSource.h"
#include "SoundSink.h"

#include <Canvas.h>

//SoundDumper, a quick debugging class for visualy dumping a few samples

class SoundDumper : public Canvas, public HSound::SoundSource, public HSound::SoundSink {
public:
	SoundDumper();
	void doDump();
	virtual unsigned int loadNextBuffer(HSound::SoundSample *buffer, unsigned int length) throw();
	signed char scale;
private:
	static const unsigned int width=600;
	bool runDump;
};
