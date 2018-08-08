# File paths 
IDIR=../include/mtsynth
LIBDIR=../inc/
SRCDIR=../src/
BINDIR=../bin/

# Compiler, flags, and packages
CXX=g++
FLAGS=-std=c++11
SOFLAGS=-I. -I$(IDIR) -I$(LIBDIR) -shared -fPIC ${FLAGS}
OFLAGS=-c -I. -I$(IDIR) -I$(LIBDIR) ${FLAGS}
SAMPLE_FLAGS=-I. -I$(IDIR) -L${BINDIR} -lmtsynth ${FLAGS}
STATIC_SAMPLE_FLAGS=-I. -I$(IDIR) ${FLAGS}

# Link libraries
LINKS := opencv_ml opencv_calib3d opencv_features2d opencv_highgui mtsynth \
		opencv_imgproc opencv_flann opencv_imgcodecs opencv_core png tiff jpeg
LIBS := $(addprefix -l, $(LINKS))
#use LDLIBS for compiling w/ shared object
LDLIBS := -L$(BINDIR) $(LIBS)
PKG-CONFIG := `pkg-config --cflags --libs pangocairo glib-2.0 opencv`

# Source files
SOURCES := $(wildcard ${SRCDIR}*.cpp)

# Compile the shared library 
libmtsynth.so: ${SOURCES} 
	if [ ! -d ${BINDIR} ];then mkdir ${BINDIR};fi
	${CXX} -o ${BINDIR}$@ $^ ${PKG-CONFIG} ${SOFLAGS}

# Compile the static library 
libmtsynth.a: ${SOURCES} 
	if [ ! -d ${BINDIR} ];then mkdir ${BINDIR};fi
	${CXX} $^ ${PKG-CONFIG} ${OFLAGS}
	ar rsc ${BINDIR}$@ ./*.o 
	rm ./*.o

# Compile a cpp example from shared library
# NOTE: Must set environment variable LD_LIBRARY_PATH=/path/to/bin/
cpp_sample: synthesizer_sample.cpp
	${CXX} -o shared_sample $^ ${PKG-CONFIG} ${SAMPLE_FLAGS}

# Compile a cpp example with static library
cpp_sample_static: synthesizer_sample.cpp ${BINDIR}libmtsynth.a
	${CXX} -o static_sample $^ ${PKG-CONFIG} ${STATIC_SAMPLE_FLAGS}


clean:
	rm -f a.out *.o *~ *.so \#*#

