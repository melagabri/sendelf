#include "MemmoryFileReader.h"

namespace HSound {

	MemmoryFileReader::MemmoryFileReader(const char *b, size_t s) : buffer(b), FileReader(s) {
	};

	MemmoryFileReader::~MemmoryFileReader() {
	};

	void MemmoryFileReader::readAbs(size_t pos,char* outBuffer,size_t readLength) throw(EOFException) {
		if(pos+readLength>=size) throw EOFException();

		memcpy(outBuffer,const_cast<char *>(buffer),readLength);
	}


};
