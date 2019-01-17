#!/bin/bash

#program to iterate over the subdirs in this directory and use MTS to generate
#images from the reproducible data stored in each dir

for path in reproducibleData/iterOverData/*; do
	[ -d "${path}" ] || continue #skip non-directories
	# feed path for dir into viewImage.cpp
	./viewImage ${path}
done