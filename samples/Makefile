# File paths 
IDIR=../include/mtsynth
LIBDIR=../inc/
SRCDIR=../src/
BINDIR=../bin/

# Compiler, flags, and packages
CXX=g++
FLAGS=-I. -I$(IDIR) -I$(LIBDIR) -shared -fPIC -std=c++11
SFLAGS=-I. -I$(IDIR) -I$(LIBDIR) -std=c++11

# Source files to compile with example file
S=map_text_synthesizer.cpp mts_basehelper.cpp mts_implementation.cpp \
	mts_bghelper.cpp mts_texthelper.cpp
H=map_text_synthesizer.hpp mts_basehelper.hpp mts_implementation.hpp \
	mts_bghelper.hpp mts_texthelper.hpp
HEADER=$(addprefix $(LIBDIR), $(H))
SRC=$(addprefix $(SRCDIR), $(S))

# Link libraries
LINKS := opencv_ml opencv_calib3d opencv_features2d opencv_highgui mtsynth \
		opencv_imgproc opencv_flann opencv_imgcodecs opencv_core png tiff jpeg
LIBS := $(addprefix -l, $(LINKS))
#use LDLIBS for compiling w/ shared object
LDLIBS := -L$(BINDIR) $(LIBS)
PKG-CONFIG := `pkg-config --cflags --libs pangocairo glib-2.0 opencv`


# Compile cpp examples
text_synthcc: text_synthesizer.cpp
	${CXX} ${SFLAGS} $^ ${SRC} ${PKG-CONFIG}


# Compile python example (REQUIRES A SHARED OBJECT LIBRARY & EXPORTED ENV VAR)
#      export LD_LIBRARY_PATH=/path/to/.so/obj


clean:
	rm -f a.out *.o *~ *.so

