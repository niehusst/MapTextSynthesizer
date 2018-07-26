
#include <memory>
#include <cstdlib>
#include <iostream>

#include "map_text_synthesizer.hpp"

/*
 * A sample program that generates images using the MapTextSynthesizer classes.
 * Use command line arguments to specify how many images to generate, and 
 * whether to save the image to the current directory, or just view them.
 *
 * The first command line argument should be an int, the number of images to 
 * synthesize. The second dictates whether or not to save produced images to the
 * current directory. Not specifying number of image (or 0) results in 
 * displaying an indefinite number of images. Not inputing the exact word "save"
 * as the second command line input opens a window for you to view produced
 * images in real time, specifying "save" will not display the window.
 * (Elements of the features can be changed by altering config.txt)
 * Example usage :
 * ./mts_sample 10 save
 */
int main(int argc, char **argv) {

  std::shared_ptr<MapTextSynthesizer> mts = MapTextSynthesizer::create();

  int num_pics = 0;
  std::string save("don't save");

  // if there are command line arguments, check to see if save is set
  if (argc > 1) {
    // if conversion fails, it remains 0
    num_pics = std::atoi(argv[1]);

    // construct string from 2nd command line argument
    save = std::string(argv[2]);
    if (std::string("save") == save) {
      /* save produced images */
    }
  } 

  cv::String img_text;
  cv::Mat img;

  // ensure nonzero values for num_pics are displayed correct number of times
  if (num_pics != 0) num_pics++;

  // generate num_pics number of sample images
  while(num_pics != 1) {
    
    // set image into img and caption into img_text
    mts->generateSample(img_text, img);

    std::cout << img_text << std::endl;
    
    //show img in window or save it ----------------------------------

    // decrement
    num_pics--;
  }


}
