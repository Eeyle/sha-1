#include <stdio.h>

// parameters used by gcc
#define COEF 1103515245 
#define INCR 12345

#define SEED_FACTOR 117
#define SEED_OFFSET 13

unsigned lcg(unsigned n, unsigned coef, unsigned incr, unsigned seed) {
    // Linear congruential generator. Uses integer arithmetic modulo INT_MAX to generate
    // a pseudorandom number. Begins with seed and uses n iterations.
    
    // Hull-Dobell theorem guarantees a period of INT_MAX as long as the following conditions are met
    // - (m, c) == 1
    // - a-1 is divisible by all prime factors of m
    // - a-1 is divisible by 4
    // for us this means
    // - (m, c) == 1
    // - a-1 is divisble by 4 but not 8
    unsigned x = seed;
    for (unsigned i = 0; i < n; i++) {
        x = coef * x + incr; 
    }
    return x;
}   

int main() {
	// This program generates 64 pseudorandom files with lengths as specified in the SHAVS Long Message Test (byte-oriented).
	int filename_length = 7 + 2 + 1;
	char filename[filename_length];
	FILE *file;

	for (int i = 0; i < 64; i++) {
		snprintf(filename, filename_length, "msg%02x.bin", i);	
		
		file = fopen(filename, "wb");
		if (file) {

			unsigned seed;
			unsigned message_length = 64 + 99 * (i + 1); // in bytes
			unsigned message[message_length / 4 + 1]; // 4 bytes per unsigned int
			for (int j = 0; j < message_length / 4 + 1; j++) {
				seed = SEED_FACTOR * (i + j) + SEED_OFFSET;
				message[j] = lcg(13 + j, COEF, INCR, seed);
			}

			fwrite(message, 1, message_length, file);
		}

		fclose(file);
	}
}
