#!/bin/bash

#program to iterate over the subdirs in this directory and use MTS to generate
#images from the reproducible data stored in each dir

if [ ! -f viewImage ]; then
	g++ viewImage.cpp `pkg-config --cflags --libs pangocairo glib-2.0 opencv` -I. -I../include/ -L../bin/ -lmtsynth -std=c++11 -o viewImage
fi

for path in reproducibleData/iterOverData/*; do
	[ -d "${path}" ] || continue #skip non-directories
	# feed path for dir into viewImage.cpp
	./viewImage ${path}
done