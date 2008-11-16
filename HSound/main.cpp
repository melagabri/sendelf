#include "Sound.h"
using namespace HSound;
#include <gccore.h>

#include <wiisprite.h>
using namespace wsp;

#include <debug.h>
#include <exception>
#include "Canvas.h"

#include "SoundDumper.h"

#include "embeded_mp3.h"

GameWindow gwd;
LayerManager manager(1);

void ohshit() {
	_break();
}

int main() {
	PAD_Init();

	gwd.InitVideo();

	std::set_unexpected(ohshit);

#define SOUNDDUMPER

#ifdef SOUNDDUMPER
	SoundDumper t;
#else
	Canvas t(60,100);
	t.clear(rgbaImage::pixel(0,255,255));
	t.vline(0 ,0 ,99,rgbaImage::pixel(0,255,0));
	t.vline(59,0 ,99,rgbaImage::pixel(0,255,0));
	t.hline(1 ,59,49,rgbaImage::pixel(0,255,0));
	t.floodFill(20,5,rgbaImage::pixel(255,0,0));
	t.BresenhamLine(5,5,30,20,rgbaImage::pixel(255,255/2,0));
	t.flush();
#endif

	Sprite t_sprite;
	t_sprite.SetImage(&t);
	//t_sprite.SetStretchHeight(4);
	//t_sprite.SetStretchWidth(4);

	t_sprite.SetPosition(0,0);

	manager.Insert(&t_sprite,0);

	manager.Draw(0, 0);//let's get a single frame out before stuff locks up
	gwd.Flush();



	// Configure for use with WIFI on port 2300
	//DEBUG_Init(GDBSTUB_DEVICE_WIFI, 2300);
	DEBUG_Init(GDBSTUB_DEVICE_USB, 1);
	// This function call enters the debug stub for the first time 
	// It's needed to call this if one wants to start debugging.
	_break();

#define MP3TEST

	SilenceGenerator silence;
	SineGenerator sine;
#ifdef MP3TEST
	MemmoryMP3Player mp3Player((char*)embeded_mp3,embeded_mp3_size);
	OnePlayBackup mp3Backup(&mp3Player);
#endif
	SoundMixer mixer;
	SoundChannel sineChannel(&sine);
	SoundChannel silenceChannel(&silence);
#ifdef MP3TEST
	SoundChannel mp3Channel(&mp3Backup);
	mp3Channel.leftLevel=1;
	mp3Channel.rightLevel=1;
	mixer.addChannel(&mp3Channel);
#endif
	mixer.addChannel(&silenceChannel);
	mixer.addChannel(&sineChannel);

	sine.step=0.003;

#ifndef SOUNDDUMPER
	//soundCard.SetSource(&mixer);
	//soundCard.SetSource(&mp3Backup);
	//soundCard.SetSource(&sine);
#else
	t.SetSource(&mixer);
	soundCard.SetSource(&t);
#endif
	bool soundMode=false;

	bool keepRunning=true;

	while(keepRunning) {
		PAD_ScanPads();

		manager.Draw(0, 0);
		gwd.Flush();

		int buttonsDown = PAD_ButtonsDown(0);
		int buttonsHeld = PAD_ButtonsHeld(0);

		if( buttonsDown & PAD_TRIGGER_Z ) {
			keepRunning=false;
		}

		if(buttonsDown & PAD_BUTTON_UP) {
			t.scale+=1;
		} else if(buttonsDown & PAD_BUTTON_DOWN) {
			t.scale-=1;
		}

		if (buttonsHeld &PAD_BUTTON_A) {
			t.doDump();
		}

		if(buttonsDown & PAD_BUTTON_B) {
			soundMode=!soundMode;
			if(soundMode) {
				sineChannel.leftLevel=1;
				sineChannel.rightLevel=1;
			} else {
				sineChannel.leftLevel=0;
				sineChannel.rightLevel=0;
			}
		}

		if(buttonsDown & PAD_BUTTON_X) {
			sine.step+=0.003;
		} else if (buttonsDown & PAD_BUTTON_Y) {
			sine.step-=0.003;
		}

#ifdef MP3TEST
		//keep the song looping
		if(mp3Player.soundComplete) {
			mp3Player.seek(0);
		}
#endif

	}

	soundCard.SetSource(0);
	mixer.removeChannel(&sineChannel);
	sineChannel.SetSource(0);
#ifdef MP3TEST
	mp3Channel.SetSource(0);
#endif

	return 0;
};
