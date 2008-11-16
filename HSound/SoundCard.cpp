#include "SoundCard.h"
#include "SoundSource.h"

#include <gccore.h>
#include <debug.h>

#include <cstring>
#include <malloc.h>
#include <exception>

namespace HSound {

	using std::memset;

	SoundCard::SoundCard(void) : bufferID(0), continueToRunThread(true) {
		if(created) throw 0;
		AUDIO_Init(0);
		AUDIO_SetDSPSampleRate(AI_SAMPLERATE_48KHZ);

		for(unsigned int i=0;i<bufferCount;i++) {
			buffers[i] = (u8 *) memalign(32, bufferLength*4);
			memset(buffers[i], 0, bufferLength*4 );
			DCFlushRange(buffers[i], bufferLength*4);
		}

		LWP_InitQueue(&threadQueue);
	
		//u8 *sfx_stack = (u8 *) memalign(32, SFX_THREAD_STACKSIZE);
		//memset(sfx_stack, 0, SFX_THREAD_STACKSIZE);

		LWP_CreateThread(&thread, ThreadCallback, this, 0,0, threadPrio);
		LWP_SuspendThread(thread);

		AUDIO_RegisterDMACallback(DMACallback);

		DMACallback();

		created=true;
	};

	SoundCard::~SoundCard(void) {
		AUDIO_StopDMA();
		AUDIO_RegisterDMACallback(NULL);
		continueToRunThread=false;
		LWP_ResumeThread(thread);
		LWP_ThreadBroadcast(threadQueue);
		LWP_JoinThread(thread,0);
		LWP_CloseQueue(threadQueue);

		for(unsigned int i=0;i<bufferCount;i++) {
			free(buffers[i]);
		}
		created=false;
	};

	void SoundCard::DMACallback(void) {
		//AUDIO_StopDMA();
		AUDIO_InitDMA((u32)((u8 *)soundCard.buffers[soundCard.bufferID]),bufferLength*4);
		//AUDIO_StartDMA();

		if(soundCard.source) {
			LWP_ThreadBroadcast(soundCard.threadQueue);
		}
	};

	void *SoundCard::ThreadCallback(void*) {
		return (void *) soundCard.runThread();
	};

	unsigned int SoundCard::runThread(void) {
		SoundSample *workbuffer= new SoundSample[bufferLength];
		while(continueToRunThread) {
			LWP_ThreadSleep(threadQueue);
			unsigned int nextBufferID=(bufferID+1)%bufferCount;
			if(source) {

				LWP_MutexLock(sourceMutex);
				try {
					source->loadNextBuffer(workbuffer,bufferLength);
				} catch(char *e ) {
					_break();
				} catch(int e) {
					_break();
				} catch(std::exception &e) {
					_break();
				} catch(...) {
					_break();
				}
				LWP_MutexUnlock(sourceMutex);

				for(unsigned int i=0;i<bufferLength;++i) {
					//note it's not safe to use the class for the final output, the array may have aligment padding!
					buffers[nextBufferID][i*4+0]=( workbuffer[i].right<<0 )& 0xFF;
					buffers[nextBufferID][i*4+1]=( workbuffer[i].right<<8 )& 0xFF;
					buffers[nextBufferID][i*4+2]=( workbuffer[i].left<<0 )& 0xFF;
					buffers[nextBufferID][i*4+3]=( workbuffer[i].left<<8 )& 0xFF;
				}
			}
			DCFlushRange(buffers[nextBufferID], bufferLength*4);
			bufferID=nextBufferID;
			
		}

		delete[] workbuffer;

		return 0;
	};

	void SoundCard::SetSource(SoundSource *s) {

		if(source==s) return;

		LWP_MutexLock(sourceMutex);

		if(s!=0 && source==0) {
			LWP_ResumeThread(thread);
			AUDIO_StartDMA();
		} else if(s==0 && source!=0) {
			LWP_SuspendThread(thread);
			AUDIO_StopDMA();
		}

		source=s;

		LWP_MutexUnlock(sourceMutex);
	};

	void SoundCard::SetLeftVolume(u8 level) {
		AUDIO_SetStreamVolLeft(level);
	};

	void SoundCard::SetRightVolume(u8 level) {
		AUDIO_SetStreamVolRight(level);
	};

	SoundCard soundCard;

	bool SoundCard::created=false;

};
