/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * A sample C++ program that runs benchmarks for the image generation speed   *
 * of MapTextSynthesizer class for specific features.                         *
 *                                                                            *
 * Copyright (C) 2018, Liam Niehus-Staab		                      *
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
#include <cstdlib>
#include <opencv2/opencv.hpp> // for imshow and Mat type

// header to include for using the synthesizer
#include "mtsynth/map_text_synthesizer.hpp"

using namespace std;
using namespace cv;

#define ROUNDS 150000
#define NUMFILES 30

/*
Program that iterates through all config files in the benchmarkConfigs directory
and runs a benchmark MTS test using each one. The resutls of each test are written to a 
file.
(may require environment variable LD_LIBRARY_PATH:
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../bin/
export LD_LIBRARY_PATH
)
--- TEST SPECIFICS ---
BASELINE means all features are turned off, and NORMAL means all features are turned on with a 10% probability of appearing. DEFAULT means the benchmark was run with all the parameters unchanged from values in the configurations file on the master branch (except height, which is fixed at 32). Other prefixes correspond to each feature in the parameter configuration file and mean only that feature is turned on with a 100% probability of appearing. The height of the image is fixed at 32 pixels unless otherwise specified.
All have a seed of 1.
*/
int main() {
  //array of parameter names that are being tested in each config file in order.
  //Parameter index in this array corresponds to the config file number.
  string configTests[] = {
    "DEFAULT",  
    "NORMAL",
    "BASELINE",
    "STRETCH",
    "SPACING",
    "TEXT_CURVE_NO_DEFORM",
    "TEXT_CURVE_DEFORM",
    "TEXT_CURVE_BASELINE",
    "MISSING_INK",
    "TEXT_ROTATION",
    "PADDING",
    "SCALING",
    "BLEND",
    "DIFF_BACKGROUND_COLOR_REGIONS",
    "DISTRACT_TEXT",
    "BOUNDARY_LINES",
    "COLOR_BLOB",
    "STRAIGHT_LINES",
    "GRID",
    "CITY_POINT",
    "PARALLEL_LINES",
    "VARYING_DISTANCE_PARALLEL_LINES",
    "TEXTURE",
    "RAILROADS",
    "RIVERS",
    "BIAS_FIELD",
    "DIGIT",
    "VARIABLE_HEIGHT",
    "ZERO_PADDED",
    "JPEG_COMPRESSION"};

    	
  string label;
  Mat image;
  int height, display_rate = ROUNDS/20;
  //open file to write results to
  filebuf out;
  out.open("benchmarkResults.txt", ios::out);
  ostream ofile(&out);

  for(int configFile = 0; configFile < NUMFILES; configFile++) {
    int k=0;
    String fileName = "benchmarkConfigs/config" + to_string(configFile) + ".txt";
    
    // Run a benchmark test of production speed with a new config file
    auto mts = MapTextSynthesizer::create(fileName);

    cout << "Running benchmark " << configTests[configFile] << flush;
    int start = time(NULL);
      
    // generate ROUNDS images from the synthesizer
    while (k<ROUNDS) {
      /*
        if(k % display_rate == 0) {
        cout << "." << flush;
        }*/
      // make the sample
      mts->generateSample(label, image, height);
      k++;
    }
    int end = time(NULL);
    cout << endl;

    // print the time it took to generate images and the production rate
    ofile << "----- " << configTests[configFile] << " -----" << endl;
    ofile << "Total runtime: " << end-start << " seconds" << endl;
    ofile << "Production rate: " << ROUNDS/(end-start) << " Hz" << endl;
       	
  }

  //close the ostream???

  return 0;
}

