/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * A cpp example for mtsynth.                                                 *
 *                                                                            *
 * Copyright (C) 2018                                                         *
 *                                                                            *
 * Written by Ziwen Chen <chenziwe@grinnell.edu>                              * 
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
#include <mtsynth/map_text_synthesizer.hpp>

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

#define ROUNDS 10000
#define SHOW_IMG
#define SAVE_IMG

int main() {
    auto s = MapTextSynthesizer::create("config.txt");

    int k=0;
    string label;
    Mat image;
    int height;

#ifdef SHOW_IMG
        cout << "Press any key to view the next image." << endl;
#endif

    int start = time(NULL);
    while (k<ROUNDS) {
        s->generateSample(label, image, height);
#ifdef SHOW_IMG
        imshow("Sample image", image);
        waitKey(0);
        cout << label << endl;
#endif
#ifdef SAVE_IMG
        imwrite("imgs/"+label+".png", image);
#endif
        k++;
    }
    int end = time(NULL);
    cout << "images generated: " << ROUNDS << endl;
    cout << "time elapsed (seconds): " << end-start << endl;
    cout << "generation speed (Hz): " << ROUNDS/double(end-start) << endl;

    return 0;
}
