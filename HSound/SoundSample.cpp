#include "SoundSample.h"

namespace HSound {

	SoundSample::SoundSample(void) : left(0), right(0) {
	};

	SoundSample::SoundSample(const signed short left_p, const signed short right_p) : left(left_p), right(right_p) {
	};

	SoundSample::~SoundSample(void) {
	};

	void SoundSample::operator +=(const SoundSample &s2) {
		left+=s2.left;
		right+=s2.right;
	};

	void SoundSample::operator *=(const SoundSample &s2) {
		left*=s2.left;
		right*=s2.right;
	};

	void SoundSample::operator =(const SoundSample &s2) {
		left=s2.left;
		right=s2.right;
	};

	void SoundSample::operator =(const signed short s) {
		left+=s;
		right+=s;
	};
};
