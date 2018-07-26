#include <vector>
#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <mtsynth/map_text_synthesizer.hpp>

using namespace std;
using namespace cv;

void read_words(string path, vector<String> &caps){
    ifstream infile(path);
    string line;
    while (std::getline(infile, line))
    {
        caps.push_back(String(line));
    }
}

int main() {
    auto s = MapTextSynthesizer::create();

    vector<String> caps;
    read_words("IA/Civil.txt",caps);

    vector<String> blocky;
    blocky.push_back("MathJax_Fraktur");
    blocky.push_back("eufm10");

    vector<String> regular;
    regular.push_back("cmmi10");
    regular.push_back("Sans");
    regular.push_back("Serif");

    vector<String> cursive;
    cursive.push_back("URW Chancery L");

    s->setSampleCaptions(caps);
    s->setBlockyFonts(blocky);
    s->setRegularFonts(regular);
    s->setCursiveFonts(cursive);


    int k=0;
    String label;
    Mat image;
    int start = time(NULL);
    while (k<10000) {
        s->generateSample(label, image);
        k++;
    }
    int end = time(NULL);
    cout << "time " << end-start << endl;
    //imshow("Sample image", image);
    //waitKey(0);
    //cout << label << endl;

    return 0;
}
