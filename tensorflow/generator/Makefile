# File paths 
IDIR=../../include/mtsynth
IDIR_COMPATIBILITY=../../include
LIBDIR=../../inc
SRCDIR=../../src/
BINDIR=../../bin/

# Compiler, flags, and packages
CXX=g++
FLAGS=-std=c++11
SOFLAGS=-I. -I$(IDIR) -I$(IDIR_COMPATIBILITY) -I$(LIBDIR) -shared -fPIC ${FLAGS}
OFLAGS=-c -I. -I$(IDIR) -I$(IDIR_COMPATIBILITY) -I$(LIBDIR) ${FLAGS}  \
        `pkg-config --cflags pangocairo glib-2.0 opencv`
SOURCES=${SRCDIR}*.cpp
OBJECTS=$(SOURCES:.cpp=.o)

# Link libraries
LIBS := opencv_ml opencv_calib3d opencv_features2d opencv_highgui \
		opencv_imgproc opencv_flann opencv_imgcodecs  \
		opencv_core png tiff jpeg cairo

LDLIBS := -L$(LIBDIR) $(addprefix -l, $(LIBS)) 
PKG-CONFIG := `pkg-config --libs pangocairo glib-2.0 opencv`

# Source files
SOURCES := $(wildcard ${SRCDIR}*.cpp)

# Make shared object and then compile sample
lib: objects ipc libmtsi.so

# Compile the object file
objects : ${SOURCES}
	${CXX} ${OFLAGS} $^ -fPIC -shared

ipc : 
	cd ./ipc_synth; make all; cd ..;

# Compile the shared library
libmtsi.so : textsynthinterface.cpp ipc_synth/*
	${CXX} -g -I./ipc_synth/  ./ipc_synth/prod_cons.o ./ipc_synth/master.o ./ipc_synth/consumer.o textsynthinterface.cpp ./*.o ${SOFLAGS} ${PKG-CONFIG} -o libmtsi.so

clean :
	rm -f *.pyc *~ *.o *.so
	cd ipc_synth; make clean; cd ../;
