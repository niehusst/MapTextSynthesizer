# File paths 
IDIR=../include/text/
LIBDIR=../src

# Compiler, flags, and packages
CXX=g++
CXXFLAGS=-std=c++11 `pkg-config --cflags --libs pangocairo glib-2.0 opencv`
CPPFLAGS=-I. -I$(IDIR)

# Link libraries
LIBS := opencv_ml opencv_calib3d opencv_features2d opencv_highgui \
		opencv_imgproc opencv_flann opencv_imgcodecs opencv_core png tiff jpeg \
		mts
LDLIBS := -L$(LIBDIR) $(addprefix -l, $(LIBS)) 
LOADLIBES := `pkg-config --cflags --libs pangocairo glib-2.0 opencv`

# Source files
SOURCES := $(wildcard *.cpp)
# Target files
TARGETS := $(basename ${SOURCES})

# Compile all targets into executables
all: ${TARGETS}

clean:
		rm -f ${TARGETS}


