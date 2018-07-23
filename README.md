# MapTextSynthesizer

MapTextSynthesizer is a program to dynamically generate synthetic images containing text, which appear to be from historical maps. The produced images serve as training data for a Convolutional Neural Network that recognizes text in scanned images of historical maps. (Data not intended for training a text detection model)

Earlier work on this project (including most of the source code development) occurred in a separate repository. That work can be found on branch 'dev' at [niehusst/opencv_contrib](https://github.com/niehusst/opencv_contrib/tree/dev).

## Getting Started

To run a sample usage of the code, fork or clone this repository, run **add make instructions** 

**add a c++ program to make and run as well.**

#### Prerequisites/Dependencies

* **Pango** comes preinstalled on many Linux machines. Otherwise, follow download from their [website](https://www.pango.org/).
* **Cairo** comes preinstalled on many Linux machines. Otherwise, follow download from their [website](https://cairographics.org/).
* **OpenCV** computer vision repository can be found on [github](https://github.com/opencv) .

## Authors

* **Ziwen Chen** - [arthurhero](https://github.com/arthurhero)
* **Liam Niehus-Staab** - [niehusst](https://github.com/niehusst)

## Acknowledgments

* [Jerod Weinman](https://github.com/weinman) for his unwavering support as a mentor and guide through this project.
* [Anguelos](https://github.com/anguelos) for a starting base synthetic image generator in their fork of [opencv_contrib](https://github.com/anguelos/opencv_contrib/blob/gsoc_final_submission/modules/text/samples/text_synthesiser.py) 
* [Behdad Esfahbod](https://github.com/behdad), a developer of both Pango and cairo, for a number of functions he wrote in [cairotwisted.c](https://github.com/phuang/pango/blob/master/examples/cairotwisted.c) which we use to curve pango text baselines using cairo.
