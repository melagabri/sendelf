#ifndef EOFEXCEPTION_H
#define EOFEXCEPTION_H

#include <exception>

namespace HSound {

	using std::exception;

	class EOFException : public exception {};
};
#endif
