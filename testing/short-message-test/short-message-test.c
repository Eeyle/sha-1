#include <stdio.h>

#define COEF 181 
#define INCR 11

#define SEED_FACTOR 148
#define SEED_OFFSET 13

unsigned char lcg(unsigned int n, unsigned char coef, unsigned char incr, unsigned char seed) {
	// Linear congruential generator. Uses integer arithmetic modulo INT_MAX to generate
	// a pseudorandom number. Begins with seed and uses n iterations.

	// Hull-Dobell theorem guarantees a period of INT_MAX as long as the following conditions are met
	// - (m, c) == 1
	// - a-1 is divisible by all prime factors of m
	// - a-1 is divisible by 4
	// for us this means
	// - (m, c) == 1
	// - a-1 is divisble by 4 but not 8
	unsigned char x = seed;
	for (unsigned int i = 0; i < n; i++) {
		x = coef * x + incr;
	}
	return x;
}

int main() {
	// This program generates 65 pseudorandom files with file-lengths as specified in SHAVS Short Message Test (byte-oriented).
	int filename_length = 7 + 2 + 1;
	char filename[filename_length];
	FILE *file;

	for (int i = 0; i < 65; i++) {
		unsigned char message[i];
		unsigned char seed = (unsigned char) SEED_FACTOR * i + SEED_OFFSET;
		for (int j = 0; j < i; j++) {
			message[j] = lcg(10 + 3 * j, COEF, INCR, seed);
		}	

		snprintf(filename, filename_length, "msg%02x.bin", i);
		file = fopen(filename, "wb");

		if (file) {
			fwrite(message, 1, i, file);

			fclose(file);
		}
	}
}

