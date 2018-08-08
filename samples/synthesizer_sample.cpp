#include <vector>
#include <iostream>
#include <fstream>
#include <memory>
#include <string>

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
    vector<string> blocky;
    blocky.push_back("Serif");

    vector<string> regular;
    regular.push_back("Sans");

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
    int start = time(NULL);

    if( (argc > 1) && (argv[1] == "benchmark") ) {
      cout << "Running benchmark" << flush;
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
      do {
        // make the sample
        mts->generateSample(label, image, height);
        // show the image 
        imshow("Sample image", image);
        waitKey(0);
        cout << label << endl;

        // repeat until user inputs 'q'
        cin >> input;
      } while(input != 'q');
    }
    
    return 0;
}
