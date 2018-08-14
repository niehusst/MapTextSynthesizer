# File paths 
IDIR=../include/
LIBDIR=../inc/
SRCDIR=../src/
BINDIR=../bin/

# Compiler, flags, and packages
CXX=g++
FLAGS=-std=c++11 -Wall
SOFLAGS=-I. -I$(IDIR) -I$(LIBDIR) -shared -fPIC ${FLAGS}
OFLAGS=-c -I. -I$(IDIR) -I$(LIBDIR) ${FLAGS}
SAMPLE_FLAGS=-I. -I$(IDIR) -L${BINDIR} -lmtsynth ${FLAGS}
STATIC_SAMPLE_FLAGS=-I. -I$(IDIR) ${FLAGS}

# Link libraries
LIBS := opencv_ml opencv_calib3d opencv_features2d opencv_highgui \
		opencv_imgproc opencv_flann opencv_imgcodecs opencv_core png tiff jpeg
LDLIBS := -L$(LIBDIR) $(addprefix -l, $(LIBS)) 
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

# Compile a cpp example
cpp_sample: text_synthesizer.cpp
	${CXX} $^ ${PKG-CONFIG} ${SAMPLE_FLAGS} -o mts_sample

# Compile a cpp example with static library
cpp_sample_static: text_synthesizer.cpp ${BINDIR}libmtsynth.a
	${CXX} $^ ${PKG-CONFIG} ${STATIC_SAMPLE_FLAGS} -o mts_sample_static

clean:
	rm -f a.out *.o *~
