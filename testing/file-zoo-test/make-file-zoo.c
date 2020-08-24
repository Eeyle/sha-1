#include <stdio.h>
#include <stdlib.h> // atoi

#define N_BYTES 2 // 256x256 == 65536 maximum possible number of files
			 // 4 // 256**4 == 4,294,967,296 == max(unsigned int) + 1

int main(int argc, char** argv) {
	// This program generates a number of files equal to argv[1]
	// each containing a number in raw binary form, the integer index of the file.
	// This ensures that every file has different content, which is necessary for testing hash functions.

	// Places the files in the same directory as the program.
	
	if (argc < 2) return 1;

	int filename_length = 8 + 2 * N_BYTES + 1; // two hex digits per byte plus terminating character
	char filename[filename_length];
	FILE *f;

	for (unsigned int i = 0; i < atoi(argv[1]); i++) {
		snprintf(filename, filename_length, "byte%0*x.bin", 2 * N_BYTES, i);
		f = fopen(filename, "wb");

		fwrite(&i, 1, N_BYTES, f); // write the bytes of the integer i as if they were character bytes

		fclose(f);
	}

	return 0;
}
