#include <sys/resource.h>
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

int main(int argc, char **argv) {

  struct rlimit data_limit;
  data_limit.rlim_cur = 1000000000;
  data_limit.rlim_max = 1000000000;
  setrlimit(RLIMIT_DATA, &data_limit);
  auto mts = MapTextSynthesizer::create("config.txt");
    
    string label;
    Mat image;
    int height;
    int i = 0;
    while(i++ < 10000) {
      mts->generateSample(label, image, height);
    }
    return 0;
}
