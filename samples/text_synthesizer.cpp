/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * A sample C++ program that uses the MapTextSynthesizer class.               *
 *                                                                            *
 * Copyright (C) 2018, Liam Niehus-Staab and Ziwen Chen                       *
 *                                                                            *
 * This program is free software: you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 3 of the License, or          *
 * (at your option) any later version.                                        * 
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.      *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/

#include <vector>
#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <opencv2/opencv.hpp> // for imshow and Mat type

// header to include for using the synthesizer
#include "mtsynth/map_text_synthesizer.hpp"

using namespace std;
using namespace cv;

#define ROUNDS 10000

/*
 * A sample program that generates images using the MapTextSynthesizer classes.
 * Use command line arguments to specify whether to run a benchmark speed test
 * by generating ROUNDS images without showing the user, or to display the 
 * synthesized images so the user may examine them. You may also specify if you
 * would like to save the images or not with a command line argument.
 *
 * Giving the commandline argument 'benchmark' will run a benchmark test as 
 * described above, and the argument 'save' will save generated images to the
 * samples/images folder of the repo. By default, the images will be displayed.
 * (Elements of the features can be changed by editing config.txt)
 * Example usage :
 * ./mts_sample_static benchmark
 * ./mts_sample_static save
 */
int main(int argc, char **argv) {
    /* create a new MapTextSynthesizer object using parameters 
       found in input filename */
    auto mts = MapTextSynthesizer::create("config.txt");

    int k=0;
    string label;
    Mat image;
    int height, display_rate = ROUNDS/20;
    
    // Run a benchmark test of production speed
    if( (argc > 1) && (string(argv[1]) == "benchmark") ) {
      cout << "Running benchmark" << flush;
      int start = time(NULL);
      
      // generate 10000 images from the synthesizer
      while (k<ROUNDS) {
        if(k % display_rate == 0) {
          cout << "." << flush;
        }
        // make the sample
        mts->generateSample(label, image, height);
        k++;
      }
      int end = time(NULL);

      // print the time it took to generate 10000 images and the production rate
      cout << endl << "Total runtime: " << end-start << " seconds" << endl;
      cout << "Images generated: " << ROUNDS << endl;
      cout << "Production rate: " << ROUNDS/(end-start) << " Hz" << endl;
      
    } else { // show the user images
      string input;
      cout << "While window is selected, press any key to advance the image."
           << endl << "To quit before " << ROUNDS << " images have been "
           << "displayed, \nuse ^C with the terminal selected." << endl;
      do {
        // make the sample
        mts->generateSample(label, image, height);
        // show the image (using opencv) and print caption to terminal
        cout << label << endl;
        imshow("Sample image", image);
        waitKey(0);
        k++;

        // check if user wants the image saved
        if ( (argc > 1) && (string(argv[1]) == "save") ) {
          // save the image to samples/images using opencv
          imwrite("images/"+label+".png", image);
        }
      } while(k < ROUNDS);
    }
    
    return 0;
}
