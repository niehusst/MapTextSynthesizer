# MapTextSynthesizer

MapTextSynthesizer is a program to dynamically generate grey-scale, synthetic images containing text, which appear to be from historical maps. The purpose of the produced images is to serve as training data for a Convolutional Neural Network that recognizes text in scanned images of historical maps. 
(Data not intended for training a text detection model!)

![MTS produced image, caption: Shambaugh](samples/images/Shambaugh.png)
![MTS produced image, caption: Maynard](samples/images/Maynard.png)
![MTS produced image, caption: Emerson](samples/images/Emerson.png)
![MTS produced image, caption: Greeley](samples/images/Greeley.png)

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

##### Installing Google Fonts

If you wish to utilize the wide variaty of fonts available in the Google Fonts repository, simply past the following Linux code into your terminal to download the google/fonts repo, copy all the font .ttf files into a `.fonts` folder in your home directory, and then delete the google/fonts repo. 

```
git clone https://github.com/google/fonts.git
mkdir ~/.fonts
cd fonts/ofl
cp -r */*.ttf ~/.fonts
cd ../apache
cp -r */*.ttf ~/.fonts
cd ../..
rm -rf fonts
```

On MacOS, you will want to run similar commands, but copying the .ttf files into the `~/Library/Fonts/` directory for Pango to be able to see the new fonts.

```
git clone https://github.com/google/fonts.git
cd fonts/ofl
cp -r */*.ttf ~/Library/Fonts
cd ../apache
cp -r */*.ttf ~/Library/Fonts
cd ../..
rm -rf fonts
```

Also be sure to change the fonts parameter in the `config.txt` file so that MTS will actually use the newly available fonts. If you wish to use the same selection google fonts as us, your fonts parameter should look like this:

```
fonts = fonts/blocky.txt, fonts/regular.txt, fonts/cursive.txt
```

## Compiling Samples

### Compile samples with Makefile on UNIX

#### Python Samples

Python sample file: `samples/text_synthesizer.py`

To compile a Ctypes Python sample that uses a shared library, call ```make python_ctypes``` from the base directory to compile a shared object file and the C code wrapper for the MTS C++ code. Then navigate to the samples directory and run the code from your terminal; ```python text_synthesizer.py```.
Unlike the C++ samples, the Python sample uses a GUI that allows you to dynamically adjust the pause time between displayed images.

A benchmark test can also be run by passing the command line argument 'benchmark' when you run the sample; ```python text_synthesizer.py benchmark```.

#### C++ Samples

C++ sample file: `samples/text_synthesizer.cpp`

To compile the C++ sample from a shared library, call `make shared` from the base directory to create the shared library file in a bin subdirectory of MapTextSynthesizer, followed by `make cpp_sample` to make the executable. To run the resulting executable (shared_sample) found in the samples directory, set an environment variable that allows your executable to find the shared library to your specific path to the shared library file: `export LD_LIBRARY_PATH=/directory/path/to/bin/` and then run the executable from the samples directory with `./mts_sample_shared`.

To compile using a static library, `make static` followed by `make cpp_sample_static`. To run the resulting executable (static_sample) located in the samples directory, call `./mts_sample_static` in the samples directory.

The C++ sample is capable of running a benchmark test of the production rate, showing and saving, or just showing the generated images. All of this can be determined by giving the executable one of either command line argument `benchmark` or `save`. For example: 

```
./mts_sample_shared benchmark
```

### Compiling C++ samples with CMake:

To install MapTextSynthesizer in your machine using CMake, open install.sh using a text editor and fill in the necessary environment variables with complete paths to this repository and, if you are using one, to your virtual environment. 

Once you have corrected the environment variables, run `./install.sh`. The resulting files will be in the new build folder.

Now that MapTextSynthesizer is installed on your machine, you can easily compile C++ programs that use MapTextSynthesizer with pkg-config:

(if using virtual env,) `export PKG_CONFIG_PATH=[install_prefix]/share/pkgconfig`
(if using virtual env,) `export LD_LIBRARY_PATH="[install_prefix]/lib`

Then

```
g++ syntheziser_sample.cpp `pkg-config --cflags --libs mtsynth -o synthesizer_sample
./synthesizer_sample
```

### Tensorflow generator

The following commands (from the repository root) construct a Python generator for use with [tf.data.Dataset.from_generator](https://www.tensorflow.org/api_docs/python/tf/data/Dataset#from_generator):

```
make static
export PKG_CONFIG_PATH=`pwd`
cd ./tensorflow/generator/
make lib
```

To use the library, set the following environment variables:

```
export PYTHONPATH=$PYTHONPATH:`pwd`
export PATH=$PATH:`pwd`/ipc_synth
export MTS_IPC=`pwd`/ipc_synth
export OPENCV_OPENCL_RUNTIME=null
export OPENCV_OPENCL_DEVICE=disabled
```

Note: 
  * `OPENCV_*` environmental variables are specified to prevent OpenCV
    from trying to use GPU when converting an image from 4 (RGBA)
    channels to 1 (gray) channel.
  * `PYTHONPATH` is specified so that `maptextsynth.py` can be found
    when `import`ing.
  * `PATH` is specified so that `producer` and `base` can be found
    when `execvp`ing for IPC multiprocess synthesis.
  * `MTS_IPC` is to get a pathname for unique IPC key generation

When launched successfully, you _should_ see `Failed to load OpenCL
runtime` for each producer spawned. (It means that OpenCV isn't using
the GPU.)
   
### For More in-depth Information

If you want still more information about the nitty-gritty of how this program works or how to modify it, please look at the DESIGN file. It has information about the file architecture, the purpose of the files, configuration instructions, and notes for contributors or devolopers who may wish to integrate this synthesizer into TensorFlow.

### Future Work

Future work for this project that we hypothesize would lead to a more robustly trained model may include:
* Generating punctuation in text (in valid positions)
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

## Citation

Please cite the following [paper](https://www.cs.grinnell.edu/~weinman/pubs/weinman19deep.pdf) if you use this code in your own research work:

```text
@inproceedings{ weinman19deep,
    author = {Jerod Weinman and Ziwen Chen and Ben Gafford and Nathan Gifford and Abyaya Lamsal and Liam Niehus-Staab},
    title = {Deep Neural Networks for Text Detection and Recognition in Historical Maps},
    booktitle = {Proc. IAPR International Conference on Document Analysis and Recognition},
    month = {Sep.},
    year = {2019},
    location = {Sydney, Australia}
} 
```

## Acknowledgments

* [Jerod Weinman](https://github.com/weinman) for his unwavering support as a mentor and guide through this project.
* [Anguelos Nicolaou](https://github.com/anguelos) for a starting base synthetic image generator in his fork of [opencv_contrib](https://github.com/anguelos/opencv_contrib/blob/gsoc_final_submission/modules/text/samples/text_synthesiser.py). 
* [Behdad Esfahbod](https://github.com/behdad), a developer of both Pango and cairo, for a number of functions he wrote in [cairotwisted.c](https://github.com/phuang/pango/blob/master/examples/cairotwisted.c) which we use to curve pango text baselines using cairo.
* [USGS GNIS](https://geonames.usgs.gov/domestic/index.html) for the massive collection of sample Iowa place-name captions freely provided under U.S. Government Work license.

This work was supported in part by the National Science Foundation under grant Grant Number [1526350](http://www.nsf.gov/awardsearch/showAward.do?AwardNumber=1526350).
