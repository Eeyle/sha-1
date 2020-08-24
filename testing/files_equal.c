#include <stdio.h>

int main(int argc, char** argv) {
	// This program checks whether two files given as arguments have identical contents.
	
	FILE *f1 = fopen(argv[1], "r");
	FILE *f2  = fopen(argv[2], "r");

	if (f1 && f2) {

		char a = fgetc(f1);
		char b = fgetc(f2);
		while ((a != EOF) && (b != EOF) && (a == b)) {
			a = fgetc(f1);
			b = fgetc(f2);
		}

		fclose(f1);
		fclose(f2);

		puts(a == b ? "Files are equal." : "Files differ.");
		return 0;
	}
	
	return 1;
}
