# MapTextSynthesizer

MapTextSynthesizer is a program to dynamically generate synthetic images containing text, which appear to be from historical maps. The produced images serve as training data for a 
Convolutional Neural Network that recognizes text in scanned images of historical maps. (Data not intended for training a text spotting model)

## Getting Started

#### Prerequisites/Dependencies

* **Pango** comes preinstalled on many Linux machines. Otherwise, follow download from their [website](https://www.pango.org/).
* **Cairo** comes preinstalled on many Linux machines. Otherwise, follow download from their [website](https://cairographics.org/).
* **OpenCV** computer vision repository can be found on [github](https://github.com/opencv) .
* ??more??

### Install with CMake on Linux

open install.sh using a text editor and type in corresponding paths

e.g. Edited Where is the source code: [path-to]/MapTextSynthesizer

`` $ ./install.sh ``

Resulting files will be in build folder.

### Install with Makefile on Linux

`` $ make ``

Use `` $ make libmtsynth.a `` to make a static library instead of a shared library.

Resulting files will be in bin folder.

### Examples

#### Python

see samples/text_synthesizer.py

#### C++

see samples/text_synthesizer.cpp

###### Sample compiling and running steps using CMake:

(if using virtual env,) `` export PKG_CONFIG_PATH=[install_prefix]/share/pkgconfig ``

(if using virtual env,) `` export LD_LIBRARY_PATH="[install_prefix]/lib" ``

``g++ text_syntheziser.cpp `pkg-config --cflags --libs mtsynth`; ./a.out``

###### Sample compiling and running steps using Makefile:

``make cpp_sample; cd samples; export LD_LIBRARY_PATH=[path-to]/bin; ./a.out``

or

``make cpp_sample_static ; cd samples; ./a.out`` to use the .a static library.

## Authors

* **Ziwen Chen** - [arthurhero](https://github.com/arthurhero)
* **Liam Niehus-Staab** - [niehusst](https://github.com/niehusst)

## Acknowledgments

* [Jerod Weinman](https://github.com/weinman) for his unwavering support as a mentor and guide through this project.
* [Anguelos](https://github.com/anguelos) for a starting base synthetic image generator in their fork of [opencv_contrib](https://github.com/anguelos/opencv_contrib/blob/gsoc_final_submission/modules/text/samples/text_synthesiser.py) 
* [Behdad Esfahbod](https://github.com/behdad), a developer of both Pango and cairo, for a number of functions he wrote in [cairotwisted.c](https://github.com/phuang/pango/blob/master/examples/cairotwisted.c) which we use to curve pango text baselines using cairo.
* ??more??
