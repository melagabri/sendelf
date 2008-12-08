#include "FileParser.h"

namespace HSound {

	FileParser::FileParser(HSound::FileReader *reader_p) : reader(reader_p) {
		if(reader==0) {
			throw "reader can not be null!";
		}
	};

	FileParser::~FileParser() {};
};
