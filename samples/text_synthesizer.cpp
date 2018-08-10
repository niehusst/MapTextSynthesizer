#include <vector>
#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <mtsynth/map_text_synthesizer.hpp>

using namespace std;
using namespace cv;

int main() {
    auto s = MapTextSynthesizer::create("config.txt");

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
