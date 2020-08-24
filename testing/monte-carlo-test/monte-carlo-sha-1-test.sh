#!/bin/bash

# A Monte-Carlo test takes the previous output(s) of the sha function as the next input to it.
# Checkpoints are taken every 1000 iterations to ensure the functions are equal at every step of the way.

sha="2";
for (( i=0; i<100; i++)); do
	for (( j=0; j<1000; j++)); do
		sha=$(../../sha-1 -t $(echo -n $sha));
	done;
	echo $sha;
done > monte-carlo-sha-1-test.sha
