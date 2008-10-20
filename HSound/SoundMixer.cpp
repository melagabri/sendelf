#include "SoundMixer.h"

#include <cstring>

namespace HSound {

	using::memset;

	SoundMixer::SoundMixer(void) : mixerBuffer() {
		LWP_MutexInit(&channelsMutex,0);
	};

	SoundMixer::~SoundMixer(void) {
		LWP_MutexDestroy(channelsMutex);
	};

	unsigned int SoundMixer::loadNextBuffer(SoundSample *outBuffer, unsigned int length) throw() {

		unsigned int channelCount=0;

		mixerBuffer.reserve(length);

		for(unsigned int i=0;i<length;i++) {
			mixerBuffer[i].left=0;
			mixerBuffer[i].right=0;
		}

		LWP_MutexLock(channelsMutex);

		list<SoundChannel*>::const_iterator endItr=channels.end();
		for(list<SoundChannel*>::const_iterator itr=channels.begin();itr!=endItr;++itr) {
			SoundChannel *chan=*itr;

			if(chan->source) {

				channelCount+=1;

				if(chan->bufferLength < length && chan->mixerBuffer) {
					delete[] chan->mixerBuffer;
					chan->mixerBuffer=0;
				}

				if(chan->mixerBuffer == 0) {
					chan->mixerBuffer=new SoundSample[length];
					chan->bufferLength=length;
				}

				chan->source->loadNextBuffer(chan->mixerBuffer,length);

				for(unsigned int i=0;i<length;i++) {
					MixerSample leveledSample=MixerSample(
						chan->mixerBuffer[i].left*chan->preAmp*chan->leftLevel,
						chan->mixerBuffer[i].right*chan->preAmp*chan->rightLevel
					);
					mixerBuffer[i]+=leveledSample;
				}

			}//end of if chan->source

		}//end of foreach channel

		LWP_MutexUnlock(channelsMutex);

		if(channelCount>0) {

			for(unsigned int i=0;i<length;++i) {
				unsigned short left,right;
				left=mixerBuffer[i].left/channelCount;
				right=mixerBuffer[i].right/channelCount;

				outBuffer[i].left=left;
				outBuffer[i].right=right;
			}
		} else {//if ChannelCount>0
			for(unsigned int i=0;i<length;i++) {
				outBuffer[i].left=0;
				outBuffer[i].right=0;
			}
		}

		return length;

	};

	void SoundMixer::addChannel(HSound::SoundChannel *c) {
		LWP_MutexLock(channelsMutex);
		channels.push_back(c);
		LWP_MutexUnlock(channelsMutex);
	};

	void SoundMixer::removeChannel(HSound::SoundChannel *c) {
		LWP_MutexLock(channelsMutex);
		channels.remove(c);
		LWP_MutexUnlock(channelsMutex);
	};

	SoundChannel::SoundChannel() : leftLevel(0), rightLevel(0), preAmp(1), mixerBuffer(0) {
	};

	SoundChannel::SoundChannel(SoundSource *s) : leftLevel(0), rightLevel(0), preAmp(1), mixerBuffer(0) {
		SetSource(s);
	};

	SoundChannel::~SoundChannel() {
		if(mixerBuffer) {
			delete[] mixerBuffer;
			mixerBuffer=0;
		}
	};

	MixerSample::MixerSample() : left(0), right(0) {};
	MixerSample::MixerSample(float left_p, float right_p) : left(left_p), right(right_p) {};

	void MixerSample::operator +=(const MixerSample &s2) {
		left +=s2.left;
		right+=s2.right;
	};
};
