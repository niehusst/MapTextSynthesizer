#include <vector>
#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <mtsynth/map_text_synthesizer.hpp>

using namespace std;
using namespace cv;

void read_words(string path, vector<string> &caps){
    ifstream infile(path);
    string line;
    while (std::getline(infile, line))
    {
        caps.push_back(line);
    }
}

int main() {
    auto s = MapTextSynthesizer::create("config.txt");

    vector<string> blocky;
    blocky.push_back("Chromaletter");

    vector<string> regular;
    regular.push_back("jsMath-cmmi10");
    regular.push_back("Sans");
    regular.push_back("Serif");

    vector<string> cursive;
    cursive.push_back("URW Chancery L");

    s->setSampleCaptions("IA/Civil.txt");
    s->setBlockyFonts(blocky);
    s->setRegularFonts(regular);
    s->setCursiveFonts(cursive);


    int k=0;
    string label;
    Mat image;
    int height;
    //int start = time(NULL);
    while (k<10000) {
        s->generateSample(label, image, height);
        imshow("Sample image", image);
        waitKey(0);
        cout << label << endl;
        k++;
    }
    //int end = time(NULL);
    //cout << "time " << end-start << endl;

    return 0;
}
