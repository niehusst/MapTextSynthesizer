# File paths 
IDIR=../include/text/

# Compile variables, flags, and libraries
CXX=g++
CXXFLAGS=`pkg-config --cflags --libs pangocairo glib-2.0 opencv` #-g -Wall
CPPFLAGS=-I. -I$(IDIR)  

#LDFLAGS+=-L/usr/local/lib/ 
# Link libraries
LIBS := opencv_ml opencv_calib3d opencv_features2d opencv_highgui \
		opencv_imgproc opencv_flann opencv_imgcodecs opencv_core png tiff jpeg
LDLIBS := $(addprefix -l, $(LIBS))

SHELL=/bin/sh

# Source files
SOURCES := $(wildcard *.cpp)
# Target files
TARGETS := $(basename ${SOURCES})

all: ${TARGETS}

clean:
		rm -f ${TARGETS} *~


