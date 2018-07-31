# File paths 
IDIR=../include/
LIBDIR=../inc/
SRCDIR=../src/
BINDIR=../bin/

# Compiler, flags, and packages
CXX=g++
FLAGS=-I. -I$(IDIR) -I$(LIBDIR) -shared -fPIC -std=c++11 -Wall
SFLAGS=-I. -I$(IDIR) -I${BINDIR} -L/home/chenziwe/MapTextSynthesizer/bin -lmtsynth -std=c++11 -Wall

# Link libraries
LIBS := opencv_ml opencv_calib3d opencv_features2d opencv_highgui \
		opencv_imgproc opencv_flann opencv_imgcodecs opencv_core png tiff jpeg
LDLIBS := -L$(LIBDIR) $(addprefix -l, $(LIBS)) 
PKG-CONFIG := `pkg-config --cflags --libs pangocairo glib-2.0 opencv`

# Source files
SOURCES := $(wildcard ${SRCDIR}*.cpp)
# Target files
TARGETS := $(basename ${SOURCES})

# Compile the shared library 
${BINDIR}libmtsynth.so: ${SOURCES} 
	if [ ! -d ${BINDIR} ];then mkdir ${BINDIR};fi
	${CXX} -o ${BINDIR}$@ $^ ${PKG-CONFIG} ${FLAGS}

# Compile a cpp example
sample: text_synthesizer.cpp
	LD_LIBRARY_PATH=/home/chenziwe/MapTextSynthesizer/bin ${CXX} $^ ${SFLAGS} ${LDLIBS}

clean:
	rm -f a.out *.o *~
