#!/bin/bash

# Runs my implimentation of sha1, a program called sha-1, on all byte*.bin files in the bytes folder, and records the output.

for file in ./bytes/byte*.bin;
	do ../../sha-1 -f $file;
done > sha-1-test.sha
