# MapTextSynthesizer

MapTextSynthesizer is a program to dynamically generate synthetic images containing text, which appear to be from historical maps. The produced images serve as training data for a Convolutional Neural Network that recognizes text in scanned images of historical maps. (Data not intended for training a text detection model)

Earlier work on this project (including most of the source code development) occurred in a separate repository. That work can be found on branch 'dev' at [niehusst/opencv_contrib](https://github.com/niehusst/opencv_contrib/tree/dev).

## Getting Started

### Prerequisites/Dependencies

* **Pango**, a text formatting library. It comes preinstalled on many Linux machines. Otherwise, follow download from their [website](https://www.pango.org/).
* **Cairo**, a vector graphics library. It comes preinstalled on many Linux machines. Otherwise, follow download from their [website](https://cairographics.org/).
* **OpenCV**, a computer vision repository. It can be found and downloaded from their [github](https://github.com/opencv).
* **glib-2.0**, a low-level Gnome library. glib is a dependency of pango and cairo; by downloading the other libraries, you will get glib-2.0. Their website can be found [here](https://lazka.github.io/pgi-docs/GLib-2.0/index.html).

##### Installing dependencies on MacOS and Linux

You will need OpenCV2 and pangocairo to run the synthesizer.
If you are running Linux, you should already have pangocairo installed in your system. To check whether it is installed, run `pkg-config --cflags --libs pangocairo` in your terminal. If you have it, your terminal should spit back a series of compiler flags that make up the pkg-config.
To install pangocairo on MacOS using homebrew, run ```brew install pango``` in the terminal. Since pango is the parent of pangocairo, pangocairo will be downloaded implicitly. 

OpenCV is used for adding Gaussian blur and noise to the final image to make it more realistic.
To install OpenCV on Linux, follow the steps [here](https://www.learnopencv.com/install-opencv3-on-ubuntu/). To install with homebrew on MacOS, run ```brew install opencv``` in the terminal.

After installing the dependencies, you should be able to jump into compiling sample programs.

### Compile with CMake on Linux

open install.sh using a text editor and type in corresponding paths

e.g. Edited Where is the source code: [path-to]/MapTextSynthesizer

`` $ ./install.sh ``

Resulting files will be in build folder.

### Compile with Makefile on Linux

`` $ make ``

Use `` $ make libmtsynth.a `` to make a static library instead of a shared library.

Resulting files will be in bin folder.

### Examples

#### Python

TODO

#### C++

To compile a C++ sample from a shared library, do ```make``` follwoed by ```make cpp_sample```. To run the resulting program (a.out) found in the samples directory, set an environment variable that allows your executable to find the shared library to your specific path to the shared library file: ```LD_LIBRARY_PATH=/directory/path/to/bin/libmtsynth.so``` and then run the executable with ```./a.out```.

Or to compile using a static library, ```make libmtsynth.a``` followed by ```make cpp_sample_static```. To run the resulting executable (a.out) located in the samples directory, call ```./a.out``` in the terminal.

##### Sample compiling steps for having made a shared object with CMake:

(if using virtual env,) `` export PKG_CONFIG_PATH=[install_prefix]/share/pkgconfig ``

(if using virtual env,) `` export LD_LIBRARY_PATH="[install_prefix]/lib" ``

``g++ text_syntheziser.cpp `pkg-config --cflags --libs mtsynth`; ./a.out``

###### Sample compiling and running steps using Makefile:

``make cpp_sample; cd samples; export LD_LIBRARY_PATH=[path-to]/bin; ./a.out``

or

``make cpp_sample_static ; cd samples; ./a.out`` to use the .a static library.

### For More in-depth Information

If you want still more information about the nitty-gritty of how this program works or how to modify it, please look at the DESIGN file. It has information about the file architecture, the purpose of the files, configuration instructions, and notes for contributors or devolopers who may wish to integrate this synthesizer into TensorFlow.

## Authors

* **Ziwen Chen** - [arthurhero](https://github.com/arthurhero)
* **Liam Niehus-Staab** - [niehusst](https://github.com/niehusst)

## Acknowledgments

* [Jerod Weinman](https://github.com/weinman) for his unwavering support as a mentor and guide through this project.
* [Anguelos](https://github.com/anguelos) for a starting base synthetic image generator in their fork of [opencv_contrib](https://github.com/anguelos/opencv_contrib/blob/gsoc_final_submission/modules/text/samples/text_synthesiser.py). 
* [Behdad Esfahbod](https://github.com/behdad), a developer of both Pango and cairo, for a number of functions he wrote in [cairotwisted.c](https://github.com/phuang/pango/blob/master/examples/cairotwisted.c) which we use to curve pango text baselines using cairo.

