#include <fstream>
#include <string>
#include <vector>
#include <iostream>

#include "mtsynth/map_text_synthesizer.hpp"
#include "mts_implementation.hpp"

using namespace std;

MapTextSynthesizer::MapTextSynthesizer(){}

vector<String> MapTextSynthesizer::readLines(string filename) {
    vector<String> lines;
    ifstream infile(filename);
    CV_Assert(infile.is_open());

    string line;
    while (std::getline(infile, line)) {   
        lines.push_back(String(line));
    }
    return lines;
}

Ptr<MapTextSynthesizer> MapTextSynthesizer::create(string config_file){
    Ptr<MapTextSynthesizer> mts(new MTSImplementation(config_file));
    return mts;
}
/*
   MapTextSynthesizer* MapTextSynthesizer::create(){
   MapTextSynthesizer* mts = new MTSImplementation();
   return mts;
   }
 */
