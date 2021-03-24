#include "RNG.hpp"
#include <cstdint>
#include <ctime>
#include <random>

// Not included in my <random>? copied from:
// https://en.cppreference.com/w/cpp/numeric/random/linear_congruential_engine
// IDK if I'm not pulling the right header or something.
// Range will be from 0 to uint32 max
typedef std::linear_congruential_engine<std::uint_fast32_t, 48271, 0, 2147483647> minstd_rand;

minstd_rand lceRand;

void initRNG() {
	std::uint_fast32_t seed = time(0); // to ensure that we can shove seed into there.
	lceRand.seed(seed);
}

unsigned int getRandUint() { return lceRand(); }

unsigned int getRandUint(unsigned int min, unsigned int max) { return lceRand() / ((lceRand.max() + min) / max); }

int getRandInt(int min, int max) { return ((int)getRandUint(0, (unsigned int)(max - min))) + min; }

double getRandDouble(double min, double max) {
	double percent = ((double)getRandUint()) / ((double)lceRand.max());

	return (percent * (max - min)) + min;
}

bool getRandBool() { return getRandUint() % 2; }