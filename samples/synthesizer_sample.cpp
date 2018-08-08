/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * A sample C++ program that uses the MapTextSynthesizer class.               *
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
#include <opencv2/opencv.hpp>

// header to include for using the synthesizer
#include "map_text_synthesizer.hpp"

using namespace std;
using namespace cv;

/*
 * A sample program that generates images using the MapTextSynthesizer classes.
 * Use command line arguments to specify whether to run a benchmark speed test
 * by generating 10000 images without showing the user, or to display the 
 * synthesized images so the user may examine them.
 *
 * Giving the commandline argument 'benchmark' will run a benchmark test as 
 * described above. Otherwise, the images will be displayed.
 * (Elements of the features can be changed by altering config.txt)
 * Example usage :
 * ./mts_sample benchmark
 */
int main(int argc, char **argv) {
    /* create a new MapTextSynthesizer object using parameters 
       found in input filename */
    auto mts = MapTextSynthesizer::create("config.txt");

    // add font types (these fonts should be available on most machines)
    // if you are getting an error about unavailable fonts, run 
    // list_available_fonts.cpp to see what fonts are available on your machine
    vector<string> blocky;
    blocky.push_back("URW Gothic L");

    vector<string> regular;
    regular.push_back("Sans");
    blocky.push_back("Serif");
    
    vector<string> cursive;
    cursive.push_back("URW Chancery L");

    // use the list of civil entity names in Iowa file for image captions
    mts->setSampleCaptions("IA/Civil.txt");
    // set the fonts
    mts->setBlockyFonts(blocky);
    mts->setRegularFonts(regular);
    mts->setCursiveFonts(cursive);

    int k=0;
    string label;
    Mat image;
    int height;

    if( (argc > 1) && (string(argv[1]) == "benchmark") ) {
      cout << "Running benchmark" << flush;
      int start = time(NULL);
      
      // generate 10000 images from the synthesizer
      while (k<10000) {
        if(k % 500 == 0) {
          cout << "." << flush;
        }
        // make the sample
        mts->generateSample(label, image, height);
        k++;
      }
      int end = time(NULL);

      // print the time it took to generate 10000 images and the production rate
      cout << endl << "Total runtime: " << end-start << " seconds" << endl;
      cout << "Production rate: " << 10000/(end-start) << " Hz" << endl;
      
    } else { // show the user images
      string input;
      cout << "While window is selected, press any button to advance the image."
           << endl << "To quit, use ^C with the terminal selected." << endl;
      do {
        // make the sample
        mts->generateSample(label, image, height);
        // show the image (using opencv) and print caption to terminal
        cout << label << endl;
        imshow("Sample image", image);
        waitKey(0);

        // repeat until user force quits with ^C
        // *** change this quit method; should be better ***
      } while(true);
    }
    
    return 0;
}
