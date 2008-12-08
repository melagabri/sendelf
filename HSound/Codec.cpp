#include "Codec.h"

namespace HSound {

	Codec::Codec(FileParser *p) : parser(p) {
		if(p==0) {
			throw "The parser can not be null!";
		}
	};
	Codec::~Codec() {};
};
