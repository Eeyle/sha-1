#!/bin/bash

for file in msgs/msg*.bin; do
	../../sha-1 -f $file;
done > short-sha-1-test.sha
