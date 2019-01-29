/*
This file requires a shared object file to compile MTS with. Be sure to run `make shared`
in the base directory before trying to compile this file.
*/
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

string getDirFromPath(string path) {
	string dirName;
	char delim = '/';
	int curPos, prevPos = 0;
	curPos = path.find(delim);

	//find the 3rd token from the path when split on /
	for(int i = 0; i < 2; i++) {
		prevPos = curPos+1;
		curPos = path.find(delim, prevPos);
	}
	dirName = path.substr(prevPos, curPos - prevPos);
	return dirName;
}


int main(int argc, char ** argv) {
	cout << "starting" << endl;
	
	//pull directory name to display image for from command line argv
	if(argc < 2) {
		cerr << "Error, no command line arguments" << endl;
		return 1;
	}
	
	string full_dir_name = string(argv[1]);
	string dir_name = getDirFromPath(full_dir_name);
	cout << "Making image from data in directory " << dir_name << endl;

	//edit config file to point MTS to correct file locations
	string configFileName = full_dir_name + string("/config") + dir_name + string(".txt");

	//use MTS to make image specified by the font, caption and config files
	auto mts = MapTextSynthesizer::create(configFileName);
	Mat image;
	string input, label;
	int height;

	mts->generateSample(label, image, height);

	//show image
	cout << "Press any key with image window selected to advance program." << endl;
	cout << label << endl;

	imshow(label, image);
	waitKey(0);
	
	return 0;
}