# File paths 
IDIR=../include/text/

# Compiler, flags, and packages
CXX=g++
CXXFLAGS=`pkg-config --cflags --libs pangocairo glib-2.0 opencv` -g -Wall
CPPFLAGS=-I. -I$(IDIR)

# Link libraries
LIBS := opencv_ml opencv_calib3d opencv_features2d opencv_highgui \
		opencv_imgproc opencv_flann opencv_imgcodecs opencv_core png tiff jpeg
LDLIBS := $(addprefix -l, $(LIBS))

# Ensure Unix shell
SHELL=/bin/bash

# Source files
SOURCES := $(wildcard *.cpp)
# Target files
TARGETS := $(basename ${SOURCES})

#all: ${TARGETS}

# 
all:  main

main:  main.o map_text_synthesizer.o mts_utilities.o mts_basehelper.o mst_implementation.o mts_bghelper.o mts_texthelper.o
		$(CXX) $(LDLIBS) -o $@ $^

main.o:  main.cpp map_text_synthesizer.hpp
		$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $<

map_text_synthesizer.o:  map_text_synthesizer.cpp map_text_synthesizer.hpp mts_implementation.hpp
		$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $<

mts_utilities.o:  mts_utilities.cpp mts_utilities.hpp
		$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $<

mts_basehelper.o:  mts_basehelper.cpp mts_basehelper.hpp
		$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $<

mts_implementation.o:  mts_implementation.cpp mts_implementation.hpp map_text_synthesizer.hpp mts_utilities.hpp mts_basehelper.hpp mts_texthelper.hpp mts_bghelper.hpp
		$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $<

mts_bghelper.o:  mts_bghelper.cpp mts_bghelper.hpp mts_basehelper.hpp
		$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $<

mts_texthelper.o:  mts_texthelper.cpp mts_texthelper.hpp
		$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $<

clean:
		rm -f ${TARGETS} *~


