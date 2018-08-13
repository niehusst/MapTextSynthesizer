#include <fstream>
#include <string>
#include <iostream>

#include "mtsynth/map_text_synthesizer.hpp"
#include "mts_implementation.hpp"

using std::string;
using cv::Mat;
using cv::Ptr;

MapTextSynthesizer::MapTextSynthesizer(){}

Ptr<MapTextSynthesizer> MapTextSynthesizer::create(string config_file){
    Ptr<MapTextSynthesizer> mts(new MTSImplementation(config_file));
    return mts;
}
