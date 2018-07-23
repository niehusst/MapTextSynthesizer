# File paths 
IDIR=../include/text/

# Compiler, flags, and packages
CXX=g++
CXXFLAGS=`pkg-config --cflags --libs pangocairo glib-2.0 opencv` -std=c++11 -fPIC
CPPFLAGS=-I. -I$(IDIR)

# Link libraries
LIBS := opencv_ml opencv_calib3d opencv_features2d opencv_highgui \
		opencv_imgproc opencv_flann opencv_imgcodecs opencv_core png tiff jpeg
LDLIBS := $(addprefix -l, $(LIBS))

# Source and object files
SOURCES=map_text_synthesizer.cpp mts_basehelper.cpp mts_bghelper.cpp mts_implementation.cpp mts_texthelper.cpp mts_utilities.cpp
OBJECTS=$(SOURCES:.cpp=.o)

# Shared object file name
EXECUTABLE=mts.so

all: $(SOURCES) $(EXECUTABLE)

# Produce a .so (shared object) file
$(EXECUTABLE): $(OBJECTS) 
		$(CXX) $(LDLIBS) -shared -o $@

.cpp.o:
		$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<

clean:
		rm -f *.o *.so core* *~



