#!/bin/bash

#make and run runAllBenchMarks.cpp

#make shared library object if it doesn't exist
if [ ! -f ../bin/libmtsynth.so ]; then
	cd ..
	make shared
	cd samples
fi

#make executable if it doesn't exist
if [ -f runAllBenchMarks ]; then
	./runAllBenchMarks
else
	#compile cpp file
	g++ runAllBenchMarks.cpp `pkg-config --cflags --libs pangocairo glib-2.0 opencv` -I. -I../include/ -L../bin/ -lmtsynth -std=c++11 -o runAllBenchMarks
	#run it
	./runAllBenchMarks
fi
