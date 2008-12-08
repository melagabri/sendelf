#ifndef FILEREADER_H
#define FILEREADER_H

#include <cstring>
#include "EOFException.h"

namespace HSound {
	using std::size_t;
	using std::exception;
	class FileReader {
	public:

		FileReader(size_t size);
		virtual ~FileReader();

		size_t size;
		virtual void readAbs(size_t,char*,size_t) throw(EOFException)=0;
		
	};
};
#endif
