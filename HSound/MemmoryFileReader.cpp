#include "MemmoryFileReader.h"

namespace HSound {

	MemmoryFileReader::MemmoryFileReader(const char *b, size_t s) : buffer(b), FileReader(s) {
	};

	MemmoryFileReader::~MemmoryFileReader() {
	};

	size_t MemmoryFileReader::readAbs(size_t pos,char* outBuffer,size_t readLength) throw(EOFException) {
		if(pos>=size) throw EOFException();
		if(readLength+pos>size) {
			size_t endpos=readLength+pos;
			
			readLength=endpos-pos+1;
		}

		memcpy(outBuffer,const_cast<char *>(buffer),readLength);

		return readLength;
	}


};
