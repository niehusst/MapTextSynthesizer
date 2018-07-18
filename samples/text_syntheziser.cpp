#include <vector>
#include <iostream>
#include <map_text_synthesizer.hpp>

using namespace std;
using namespace cv;

int main() {
    auto s = MapTextSynthesizer::create();

    vector<String> caps;
    caps.push_back("AAA");
    caps.push_back("BBB");

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

    String label;
    Mat image;
    s->generateSample(label, image);

    cout << label << endl;

    return 0;
}
