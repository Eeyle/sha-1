#!/bin/bash

sha="2" # seed string
for (( i=0; i<100; i++ )); do
	for (( j=0; j<1000; j++ )); do
		# Pipe the previous hash into sha1sum to get the next hash. 
		# Get the right output with grep, since sha1sum appends the filename to the output.
		# echo -n: no newline appended to echo output
		# grep -o: output only capture groups, not the entire line
		sha=$(echo -n $sha | sha1sum | echo -n $(grep -o "\([0-9a-f]\{40\}\)"));
	done;
	# Record 100 checkpoints.
	echo $sha; 
done > monte-carlo-sha1sum-test.sha
