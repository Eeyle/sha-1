#!/bin/bash

for file in msgs/msg*.bin; do
	sha1sum $file | grep -o "\([0-9a-f]\{40\}\)";
done > short-sha1sum-test.sha
