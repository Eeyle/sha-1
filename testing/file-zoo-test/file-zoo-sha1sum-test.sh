#!/bin/bash

# Runs sha1sum on all byte*.bin files in the bytes folder and records the output to a file. 

for file in ./bytes/byte*.bin; 
	do sha1sum $file;
done > sha1sum-test.sha
