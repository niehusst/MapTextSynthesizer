#include <vector>
#include <iostream>
#include <map_text_synthesizer.hpp>

using namespace std;
using namespace cv;

int main() {
    shared_ptr<MapTextSynthesizer> s = MapTextSynthesizer::create();

    vector<String> caps;
    caps.push_back("AAA");
    caps.push_back("BBB");
    caps.push_back("CCC");

    vector<String> blocky;
    blocky.push_back("MathJax_Fraktur");

    vector<String> regular;
    regular.push_back("cmmi10");

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
