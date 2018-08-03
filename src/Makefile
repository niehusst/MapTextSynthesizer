# File paths 
IDIR=../inc/
IDIR2=../include/mtsynth/
BINDIR=../bin/

# Compiler, flags, and packages
CXX=g++
CXXFLAGS=-std=c++11 -fPIC `pkg-config --cflags --libs pangocairo glib-2.0 opencv`
CPPFLAGS=-I. -I$(IDIR) -I$(IDIR2)

# Link libraries
LIBS := opencv_ml opencv_calib3d opencv_features2d opencv_highgui \
		opencv_imgproc opencv_flann opencv_imgcodecs opencv_core png tiff jpeg
LDLIBS := $(addprefix -l, $(LIBS))

# Source and object files
SOURCES=map_text_synthesizer.cpp mts_basehelper.cpp mts_bghelper.cpp \
	mts_implementation.cpp mts_texthelper.cpp
OBJECTS=$(SOURCES:.cpp=.o)

# Shared object file name
SHARED_LIBRARY=libmtsynth.so

all: $(SOURCES) $(SHARED_LIBRARY)

# Produce a .so (shared object) file
$(SHARED_LIBRARY): $(OBJECTS)
		if [ ! -d ${BINDIR} ];then mkdir ${BINDIR};fi
		$(CXX) $(LDLIBS) -shared -o $(BINDIR)$@ $^ $(CXXFLAGS)

.cpp.o:
		$(CXX) $(CPPFLAGS) -c $< $(CXXFLAGS)

clean:
		rm -f *.o *.so core* *~



