# MapTextSynthesizer

MapTextSynthesizer is a program to dynamically generate grey-scale, synthetic images containing text, which appear to be from historical maps. The purpose of the produced images is to serve as training data for a Convolutional Neural Network that recognizes text in scanned images of historical maps. 
(Data not intended for training a text detection model!)

![MTS produced image, Grundy](samples/imgs/Grundy.png)
![MTS produced image, Otter](samples/imgs/Otter.png)


## Getting Started

### Prerequisites/Dependencies

* **Pango**, a text rendering library. See [pango.org](https://www.pango.org/) for more information.
* **Cairo**, a vector graphics library. See [cairographics.org](https://cairographics.org/) for more information.
* **OpenCV**, a computer vision repository. Find it on github at [opencv/opencv](https://github.com/opencv/opencv).
* **Boost**, a collection C++ source libraries. See [boost.org](https://www.boost.org/) for more information.
* **Google Fonts**, a collection of open-source fonts. This isn't necessary for the synthesizer to function, but it is highly recommended for training robust models. Find it on github at [google/fonts](https://github.com/google/fonts/).

##### Installing dependencies on MacOS and Linux

You will need OpenCV2, Boost and pangocairo to run the synthesizer.  
Pangocairo is a crucial tool for our synthesizer; it is used for drawing all the backgrounds and text in synthesized images. If you are running Linux, you should already have pangocairo installed in your system. To check whether it is installed, run `pkg-config --cflags --libs pangocairo` in your terminal. If you have it, your terminal should spit back a series of compiler flags that make up the pkg-config. If you don't have pangocairo, follow the download instructions on the Pango [website](https://www.pango.org/Download).  
To install pangocairo on MacOS using homebrew, run ```brew install pango``` in the terminal. Since pango is the parent of pangocairo, pangocairo will be downloaded implicitly. 

OpenCV is used for adding Gaussian blur and noise to the final image to make it more realistic.
To install OpenCV on Linux using apt-get, follow these steps from [learnopencv.com](https://www.learnopencv.com/install-opencv3-on-ubuntu/).   
To install with homebrew on MacOS, run ```brew install opencv``` in the terminal.

Boost is used for the distributions it provides, allowing our random samples to be more specific in shape. Boost-Python is used in the TensorFlow/Python integration of MapTextSynthesizer. To install Boost on Linux using apt-get, run ```sudo apt-get install libboost-all-dev``` and ```sudo apt-get  install libboost-python-dev``` in your terminal. If you don't have sudo privledges, follow the download instructions on their [website](https://www.boost.org/users/download/).   
To install both Boost libraries on MacOS using homebrew, run ```brew install boost``` and ```brew install boost-python``` in the terminal.

After installing the dependencies, you should be able to jump right into compiling sample programs.

## Compiling Samples

### Compile samples with Makefile on UNIX  

#### Python Samples

To compile a Ctypes Python sample that uses a shared library, call ```make python_ctypes``` from the base directory to compile a shared object file and the C code wrapper for the MTS C++ code. Then navigate to the samples directory and run the code from your terminal; ```python text_synthesizer.py```.

#### C++ Samples

C++ sample file: ```samples/text_synthesizer.cpp```

To compile a C++ sample from a shared library, call ```make shared``` from the base directory to create the shared library file in a bin subdirectory of MapTextSynthesizer, followed by ```make cpp_sample``` to make the executable. To run the resulting executable (shared_sample) found in the samples directory, set an environment variable that allows your executable to find the shared library to your specific path to the shared library file: ```export LD_LIBRARY_PATH=/directory/path/to/bin/``` and then run the executable from the samples directory with ```./mts_sample```.

To compile using a static library, ```make static``` followed by ```make cpp_sample_static```. To run the resulting executable (static_sample) located in the samples directory, call ```./mts_sample_static``` in the samples directory.

To toggle whether to display the images generated or to save the images generated, comment or uncomment ```#define SHOW_IMG``` and ```#define SAVE_IMG``` in text_synthesizer.cpp.

### Compiling samples with CMake:

To install MapTextSynthesizer in your machine using CMake, open install.sh using a text editor and fill in the necessary environment variables with complete paths to this repository and, if you are using one, to your virtual environment. 

Once you have corrected the environment variables, run `` $ ./install.sh ``. The resulting files will be in the new build folder.

Now that MapTextSynthesizer is installed on your machine, you can easily compile C++ programs that use MapTextSynthesizer with pkg-config:

(if using virtual env,) `` export PKG_CONFIG_PATH=[install_prefix]/share/pkgconfig ``
(if using virtual env,) `` export LD_LIBRARY_PATH="[install_prefix]/lib" ``

``g++ syntheziser_sample.cpp `pkg-config --cflags --libs mtsynth`; ./a.out``


### For More in-depth Information

If you want still more information about the nitty-gritty of how this program works or how to modify it, please look at the DESIGN file. It has information about the file architecture, the purpose of the files, configuration instructions, and notes for contributors or devolopers who may wish to integrate this synthesizer into TensorFlow.

### Future Work

Future work for this project that we hypothesize would lead to a more robustly trained model may include:  
* Generating punctuation in text (in valid positions
* Generating characters with accent marks
* A more map-realistic way to simulate mountains than the existing textures
* Adding glyph/symbol patterns to textures (this could be useful for swamp simulation)
* Vertical baseline jitter in text; map text doesn't always have a straight baseline
* Captions that are split, as if across a background feature
* Abbreviations where the last letter is stacked above the period. This is common in some historical maps.

## Authors

* **Ziwen Chen** - [arthurhero](https://github.com/arthurhero)
* **Liam Niehus-Staab** - [niehusst](https://github.com/niehusst)
* **Benjamin Gafford** - [gaffordb](https://github.com/gaffordb)

## Acknowledgments

* [Jerod Weinman](https://github.com/weinman) for his unwavering support as a mentor and guide through this project.
* [Anguelos Nicolaou](https://github.com/anguelos) for a starting base synthetic image generator in their fork of [opencv_contrib](https://github.com/anguelos/opencv_contrib/blob/gsoc_final_submission/modules/text/samples/text_synthesiser.py). 
* [Behdad Esfahbod](https://github.com/behdad), a developer of both Pango and cairo, for a number of functions he wrote in [cairotwisted.c](https://github.com/phuang/pango/blob/master/examples/cairotwisted.c) which we use to curve pango text baselines using cairo.

