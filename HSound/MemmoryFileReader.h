#ifndef MEMMORYFILEREADER_H
#define MEMMORYFILEREADER_H

#include "FileReader.h"
#include "EOFException.h"

#include <cstring>

namespace HSound {

	class MemmoryFileReader : public FileReader {
	public:
		MemmoryFileReader(const char *,size_t);
		~MemmoryFileReader();
		virtual size_t readAbs(size_t,char*,size_t) throw(EOFException);
		size_t size;
	private:
		const char *buffer;
	};
}
#endif
