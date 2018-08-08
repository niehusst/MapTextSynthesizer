# MapTextSynthesizer

MapTextSynthesizer is a program to dynamically generate synthetic images containing text, which appear to be from historical maps. The produced images serve as training data for a Convolutional Neural Network that recognizes text in scanned images of historical maps. (Data not intended for training a text detection model)

## Getting Started

### Prerequisites/Dependencies

* **Pango**, a text formatting library. It comes preinstalled on many Linux machines. See their [website](https://www.pango.org/) for more information.
* **Cairo**, a vector graphics library. It comes preinstalled on many Linux machines. See their [website](https://cairographics.org/) for more information.
* **OpenCV**, a computer vision repository. It can be found on [github](https://github.com/opencv).
* **glib-2.0**, a low-level Gnome library. glib is a dependency of pango and cairo; by downloading the other libraries, you will get glib-2.0. Their website can be found [here](https://lazka.github.io/pgi-docs/GLib-2.0/index.html).
* **Google Fonts**, a collection of open-source fonts. This isn't necessary for the synthesizer to function, but it is highly recommended for training robust models. Find it on [github](https://github.com/google/fonts/).

##### Installing dependencies on MacOS and Linux

You will need OpenCV2 and pangocairo to run the synthesizer.
If you are running Linux, you should already have pangocairo installed in your system. To check whether it is installed, run `pkg-config --cflags --libs pangocairo` in your terminal. If you have it, your terminal should spit back a series of compiler flags that make up the pkg-config.
To install pangocairo on MacOS using homebrew, run ```brew install pango``` in the terminal. Since pango is the parent of pangocairo, pangocairo will be downloaded implicitly. 

OpenCV is used for adding Gaussian blur and noise to the final image to make it more realistic.
To install OpenCV on Linux, follow the steps [here](https://www.learnopencv.com/install-opencv3-on-ubuntu/). To install with homebrew on MacOS, run ```brew install opencv``` in the terminal.

After installing the dependencies, you should be able to jump into compiling sample programs.

### Compile with CMake on Linux

Open install.sh using a text editor and type in corresponding paths

e.g. Edited Where is the source code: [path-to]/MapTextSynthesizer

Then run `` $ ./install.sh ``.

Resulting files will be in build folder.

### Compile samples with Makefile on UNIX  

#### Python

TODO (requires Ben's ctype code and a shared object library)

#### C++

To compile a C++ sample from a shared library, do ```make shared```, this creates the shared library file in a bin subdirectory of MapTextSynthesizer, followed by ```make cpp_sample``` to make the executable. To run the resulting program (shared_sample) found in the samples directory, set an environment variable that allows your executable to find the shared library to your specific path to the shared library file: ```LD_LIBRARY_PATH=/directory/path/to/bin/``` and then run the executable from the samples directory with ```./shared_sample```.

To compile using a static library, ```make static``` followed by ```make cpp_sample_static```. To run the resulting executable (static_sample) located in the samples directory, call ```./static_sample``` in the samples directory.

##### Compiling samples with CMake:

Once you have followed the CMake installation instructions that download the MapTextSynthesizer into your machine, you can easily compile using pkg-config.

(if using virtual env,) `` export PKG_CONFIG_PATH=[install_prefix]/share/pkgconfig ``

(if using virtual env,) `` export LD_LIBRARY_PATH="[install_prefix]/lib" ``

``g++ text_syntheziser.cpp `pkg-config --cflags --libs mtsynth`; ./a.out``


### For More in-depth Information

If you want still more information about the nitty-gritty of how this program works or how to modify it, please look at the DESIGN file. It has information about the file architecture, the purpose of the files, configuration instructions, and notes for contributors or devolopers who may wish to integrate this synthesizer into TensorFlow.

## Authors

* **Ziwen Chen** - [arthurhero](https://github.com/arthurhero)
* **Liam Niehus-Staab** - [niehusst](https://github.com/niehusst)
* **Benjamin Gafford** - [gaffordb](https://github.com/gaffordb)

## Acknowledgments

* [Jerod Weinman](https://github.com/weinman) for his unwavering support as a mentor and guide through this project.
* [Anguelos](https://github.com/anguelos) for a starting base synthetic image generator in their fork of [opencv_contrib](https://github.com/anguelos/opencv_contrib/blob/gsoc_final_submission/modules/text/samples/text_synthesiser.py). 
* [Behdad Esfahbod](https://github.com/behdad), a developer of both Pango and cairo, for a number of functions he wrote in [cairotwisted.c](https://github.com/phuang/pango/blob/master/examples/cairotwisted.c) which we use to curve pango text baselines using cairo.

