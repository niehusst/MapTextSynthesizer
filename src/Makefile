# File paths 
IDIR=../include/text/

# Compile variables, flags, and libraries
CC=g++
CFLAGS=`pkg-config --cflags --libs pangocairo glib-2.0`
CPPFLAGS=-I. -I$(IDIR) $(CFLAGS)
LDFLAGS+=

SHELL=/bin/sh

# Source files
SOURCES := $(wildcard *.cpp)
# Target files
TARGETS := $(basename ${SOURCES})

all: ${TARGETS}

clean:
		rm -f ${TARGETS}


