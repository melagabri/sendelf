#include "FileParser.h"

namespace HSound {

	FileParser::FileParser(SoundPlayer *player_p) : player(player_p) {
		if(player==0) {
			throw "player can not be null!";
		}
	};

	FileParser::~FileParser() {};
};
