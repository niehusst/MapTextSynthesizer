# File paths 
IDIR=../include/text/

# Compiler, flags, and packages
CXX=g++
CXXFLAGS=`pkg-config --cflags --libs pangocairo glib-2.0 opencv` -g -Wall -std=c++11 -fPIC
CPPFLAGS=-I. -I$(IDIR)

# Link libraries
LIBS := opencv_ml opencv_calib3d opencv_features2d opencv_highgui \
		opencv_imgproc opencv_flann opencv_imgcodecs opencv_core png tiff jpeg
LDLIBS := $(addprefix -l, $(LIBS))

# Ensure Unix shell
SHELL=/bin/bash

# Source and object files
SOURCES=main.cpp map_text_synthesizer.cpp mts_basehelper.cpp mts_bghelper.cpp mts_implementation.cpp mts_texthelper.cpp mts_utilities.cpp
OBJECTS=$(SOURCES:.cpp=.o)

#all: ${TARGETS}
# explicitly compile each file

EXECUTABLE=mts.so

all: $(SOURCES) $(EXECUTABLE)

# Produce a .so (shared object) file
$(EXECUTABLE): $(OBJECTS) 
		$(CXX) $(LDLIBS) -shared $(OBJECTS) -o $@

.cpp.o:
		$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< -o $@

clean:
		rm -f ${TARGETS} *~


