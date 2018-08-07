#include <vector>
#include <iostream>
#include <fstream>
#include <memory>
#include <string>

// header to include for using the synthesizer
#include "map_text_synthesizer.hpp"

using namespace std;
using namespace cv;


// set up the synthesizer and generate 10000 images (without displaying them)
int main() {
    /* create a new MapTextSynthesizer object using parameters 
       found in input filename */
    auto mts = MapTextSynthesizer::create("config.txt");

    // add font types (these fonts should be available on most machines)
    vector<string> blocky;
    //blocky.push_back("Chromaletter");
    blocky.push_back("Serif");

    vector<string> regular;
    //regular.push_back("jsMath-cmmi10");
    regular.push_back("Sans");
    regular.push_back("Serif");

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
    
    // generate 10000 images from the synthesizer
    while (k<10000) {
        mts->generateSample(label, image, height);
        k++;
    }
    int end = time(NULL);

    // print the time it took to generate 10000 images
    cout << "time " << end-start << endl;
    return 0;
}
