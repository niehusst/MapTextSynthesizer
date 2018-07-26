# File paths 
IDIR=../inc/
LIBDIR=../src

# Compiler, flags, and packages
CXX=g++
CXXFLAGS=-std=c++11 `pkg-config --cflags --libs pangocairo glib-2.0 opencv`
CPPFLAGS=-I/usr/include/python2.7

# Link libraries
LIBS :=`pkg-config --cflags --libs mtsynth` -lpthread -lboost_python -lpython2.7
LDLIBS := -L$(LIBDIR) $(addprefix -l, $(LIBS)) 
LOADLIBES := `pkg-config --cflags --libs pangocairo glib-2.0 opencv`

# Source files
SOURCES := $(wildcard *.cpp)
# Target files
TARGETS := $(basename ${SOURCES})

# Compile all targets into executables
all: ${TARGETS}

library : text_synthesizer_py.cpp
	$(CXX) -shared text_synthesizer_py.cpp $(CPPFLAGS) \
	$(LIBS) -fPIC -o mtsynth.so

clean:
	rm -f ${TARGETS}





