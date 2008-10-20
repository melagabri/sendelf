#pragma once
#ifndef MODPLAYER_H
#define MODPLAYER_H

#include "SoundPlayer.h"

namespace HSound {

	class ModPlayer : public SoundPlayer {
	public:
		ModPlayer(void);
		virtual ~ModPlayer(void);
	};

}
#endif
