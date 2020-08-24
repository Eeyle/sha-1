#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Constants of the algorithm.
#define H0 0x67452301ul
#define H1 0xEFCDAB89ul
#define H2 0x98BADCFEul
#define H3 0x10325476ul
#define H4 0xC3D2E1F0ul

#define K0 0x5A827999ul
#define K1 0x6ED9EBA1ul
#define K2 0x8F1BBCDCul
#define K3 0xCA62C1D6ul


#define INTEGER_BIT_LENGTH 32


// -- Functions --

char* get_file_contents(char* path, size_t* length) {
	// Get all the contents of a file as a single string.
	FILE *f = fopen(path, "rb");
	if (f) {
		fseek(f, 0, SEEK_END);
		*length = ftell(f);
		fseek(f, 0, SEEK_SET);

		char* result = (char*) malloc(*length * sizeof(char));
		if (result) {
			if (fread(result, 1, *length, f) == *length) { 
				fclose(f);
				return result;
			}
			else {
				puts("Unable to read the full length of the file.");
			}
		}
		else {
			puts("Unable to allocate enough memory for the file.");
		}

		fclose(f);
	}
	else {
		puts("Unable to locate the file.");
	}	
	
	return NULL;
}

size_t get_padded_message_length(size_t message_length) { 
	// Returns the message length rounded up to the nearest multiple of 64 (so that it rounds up to a multiple of 512 bitwise).
	int factor = ((message_length % 64) < 56) ? 1 : 2; // need 8 extra bytes minimum at the end
	return ((message_length / 64) + factor) * 64; 
}

void get_message_length_string(char* result, size_t message_length) {
	// Converts the length of the message into a 64-bit big-endian binary number represented by a string of character bytes.
	size_t length = message_length * sizeof(char) * 8; // message_length in bits
	size_t mask = 0b11111111; // 0xff
	for (int i = 0; i < 8; i++) {
		result[7-i] = (unsigned char) ((length >> (8*i)) & mask);
	}
}

unsigned int left_rotate(unsigned int number, unsigned int n) {
	return (unsigned int) ((number << n) | (number >> (INTEGER_BIT_LENGTH - n)));
}

unsigned int parse_integer(char *location) {
	// Converts a chunk of four bytes at *location into a big-endian integer
	unsigned int n = *((unsigned int*) location); // first convert the memory at location into an unsigned integer
	return ( (n        & 0xff) << 24) | 
		   (((n >> 8 ) & 0xff) << 16) |
		   (((n >> 16) & 0xff) << 8 ) |
		   ( (n >> 24) & 0xff       );	// then flip the endianness of the four bytes
}

// -- Main --

int main(int argc, char **argv) 
{
	if (argc < 3 || argv[1][0] != '-') { 
		puts("Please include a parameter. See sha-1 -h for detail.");
		return 1;
	}
	
	if (argv[1][1] == 'h') {
		puts(" -t \"text\" : computes the sha-1 hash of the text between quotation marks.");
		puts(" -f path     : computes the sha-1 hash of the file located at path.");
		puts(" -h          : show this help message.");
		return 0;
	}

	char* message;
	size_t message_length;
	if (argv[1][1] == 't') {
		// Read the next argument as literal quoted text.
		message = argv[2];
		if (!message) return 1;
		message_length = strlen(message);
	}
	else if (argv[1][1] == 'f') {
		// Read the next argument as a filepath.
		message = get_file_contents(argv[2], &message_length);
		if (!message) return 1;
	}
	
	// Initialize the padded message.
	size_t padded_message_length = get_padded_message_length(message_length);
	char padded_message[padded_message_length];

	// -- Preprocessing --
	// 1. Append the bit 1 to the end of the message. 
	// 2. Fill the padded message with zeros until there are only 8 bytes (64 bits) left.
	// 3. Fill the final 8 bytes of the message with message_length as a 64-bit binary integer.
	 
	size_t padding_length = padded_message_length - 8 - message_length;
	char padding_string[padding_length];
	padding_string[0] = 0x80; // 1. Append 0b10000000
	for (int i = 1; i < padding_length; i++) padding_string[i] = 0; // 2. Fill with 0s 

	// 3. 
	char message_length_string[8];
    get_message_length_string(message_length_string, message_length);

	// Copy everything over to the padded message.
	memcpy(padded_message, message, message_length); // the message itself
	memcpy(padded_message + message_length, padding_string, padding_length); // 1. and 2.
	memcpy(padded_message + message_length + padding_length, message_length_string, 8); // 3.
	
	if (argv[1][1] == 'f') free(message);

	// -- Processing --

	// Initialize the h variables which will form the final output string.
	unsigned int h[5];
	h[0] = H0;
	h[1] = H1;
	h[2] = H2;
	h[3] = H3;
	h[4] = H4;

	// Keep the K variable in an array to avoid excessive branching.
	unsigned int K[4];
	K[0] = K0;
	K[1] = K1;
	K[2] = K2;
	K[3] = K3;
	
	// Process the message in 512-bit (64 byte) chunks.
	for (size_t i = 0; i < padded_message_length; i += 64) {
		// Allocate memory for 80x 32-bit (4 byte) words.
		unsigned int words[80];

		// The first sixteen words are given by this chunk of the message.
		// pseudocode: memcpy(words, padded_messge + i, 64); // in practice need to parse every integer for endianness
		for (int j = 0; j < 16; j++) {
			words[j] = parse_integer(padded_message + i + j * sizeof(unsigned int));
		}
//		for (int j = 0; j < 16; j++) { printf("W(j=%d): %08x\n", j, words[j]); } // debug: print the 16 seed-words for each chunk

		// The rest are generated.
		for (int j = 16; j < 80; j++) {
			words[j] = left_rotate(words[j-3] ^ words[j-8] ^ words[j-14] ^ words[j-16], 1);
		}		
		// The following commented code is theoretically identical to the for loop above, and may incur a performance boost among other benefits.
//		for (int j = 16; j < 32; j++) {
//			words[j] = left_rotate(words[j-3] ^ words[j-8] ^ words[j-14] ^ words[j-16], 1);
//		} 
//		for (int j = 32; j < 80; j++) {
//			words[j] = left_rotate(words[j-6] ^ words[j-16] ^ words[j-28] ^ words[j-32], 2);
//		}

		// Initialize hash values for this chunk.
		unsigned int a = h[0];
		unsigned int b = h[1];
		unsigned int c = h[2];
		unsigned int d = h[3];
		unsigned int e = h[4];

		// Loop 80 times to randomize this chunk's hash values.
		unsigned int f;
		unsigned int k;
		unsigned int T;
		for (int j = 0; j < 80; j++) {
			// The following commented code is pseudocode for the version below.
//			if (j < 20) { 
//				f = (b & c) | ((~b) & d);
//				k = K0;
//			} else if (j < 40) {
//				f = b ^ c ^ d;
//				k = K1;
//			} else if (j < 60) {
//				f = (b & c) | (b & d) | (c & d);
//				k = K2;	
//			} else {
//				f = b ^ c ^ d;
//				k = K3;
//			}
			// This version is theoretically faster because the computer can load one set of instructions to be repeated on loop rather than having to branch.
			f = ( j < 20						   ) * ( (b & c) | ((~b) & d)		 ) + // first case
				( (j >= 20 && j < 40) || (j >= 60) ) * ( b ^ c ^ d					 ) + // second and last cases of the if-block
				( j >= 40 && j < 60				   ) * ( (b & c) | (b & d) | (c & d) );  // third case
			k = K[j / 20];

			T = left_rotate(a, 5) + e + f + k + words[j];
			e = d;
			d = c;
			c = left_rotate(b, 30);
			b = a;
			a = T;	

//			printf("t=%d: %08x %08x %08x %08x %08x\n", j, a, b, c, d, e); // debug: print the intermediate a,b,c,d,e values every time
		}

		// Then add those hash values to the result so far.
		h[0] += a;
	   	h[1] += b;	
		h[2] += c;
		h[3] += d;
		h[4] += e;
	
	}

//	for (int i = 0; i < padded_message_length; i++) printf("%c", padded_message[i] ? padded_message[i] : ' '); // debug: print the entire message, padding and all

	// Concatenate the values of h to get the result.
	for (int i = 0; i < 5; i++) {
		printf("%08x", h[i]);
	} printf("\n");

	return 0;
}




















