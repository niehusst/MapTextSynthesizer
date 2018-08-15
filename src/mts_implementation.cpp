/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * mts_implementation.cpp holds definitions for methods of the                *
 * MTSImplementation class.                                                   *
 *                                                                            *
 * Copyright (C) 2018                                                         *
 *                                                                            *
 * Written by Ziwen Chen <chenziwe@grinnell.edu>                              * 
 * and Liam Niehus-Staab <niehusst@grinnell.edu>                              *
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


#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <ctime>
#include <errno.h>
#include <map>
#include <limits>
#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
#include <algorithm>
#include <iosfwd>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <unordered_map>

// Pango/cairo includes
#include <glib.h>
#include <pango/pangocairo.h>

#include "mts_implementation.hpp"

using std::string;
using std::vector;
using std::cerr;
using std::endl;
using std::make_shared;

using cv::Mat;

using boost::random::beta_distribution;
using boost::random::variate_generator;

void MTSImplementation::cairoToMat(cairo_surface_t *surface,Mat &mat) {
    // make a 4 channel opencv matrix
    Mat mat4 = Mat(cairo_image_surface_get_height(surface),cairo_image_surface_get_width(surface),CV_8UC4,cairo_image_surface_get_data(surface));

    vector<Mat> channels;

    cv::split(mat4,channels);

    //iterate through all channels
    mat = channels[0];
}

void MTSImplementation::addGaussianNoise(Mat& out) {
    // get and use user config parameters to set sigma
    double scale = config->getParamDouble("noise_sigma_scale");
    double shift = config->getParamDouble("noise_sigma_shift");
    double sigma = round((pow(1/(noise_gen() + 0.1), 0.5) * scale + shift) * 100) / 100;

    // create noise matrix
    Mat noise = Mat(out.rows, out.cols, CV_32F);

    // populate noise with random values
    randn(noise, 0, sigma);

    // add noise to each channel
    out+=noise;
    threshold(out,out,1.0,1.0,cv::THRESH_TRUNC);
    threshold(out,out,0,1.0,cv::THRESH_TOZERO);
}

void MTSImplementation::addGaussianBlur(Mat& out) {
    // get user config parameters for kernel size
    int size_min = config->getParamInt("blur_kernel_size_min") / 2;
    int size_max = config->getParamInt("blur_kernel_size_max") / 2;
    int ker_size = (helper->rndBetween(size_min,size_max)) * 2 + 1;

    GaussianBlur(out,out,cv::Size(ker_size,ker_size),0,0,cv::BORDER_REFLECT_101);
}

void MTSImplementation::addCompressionArtifacts(Mat& out){
    if(helper->rndProbUnder(config->getParamDouble("jpeg_prob"))){
        vector<uchar> buffer;
        vector<int> parameters;
        parameters.push_back(CV_IMWRITE_JPEG_QUALITY);
        int quality_min = config->getParamInt("jpeg_quality_min");
        int quality_max = config->getParamInt("jpeg_quality_max");
        int quality = helper->rndBetween(quality_min,quality_max);
        parameters.push_back(quality);
        Mat ucharImg;
        out.convertTo(ucharImg,CV_8UC1,255);
        cv::imencode(".jpg",ucharImg,buffer,parameters);
        ucharImg=cv::imdecode(buffer,CV_LOAD_IMAGE_GRAYSCALE);
        ucharImg.convertTo(out,CV_32FC1,1.0/255);
    }
}


MTSImplementation::MTSImplementation(string config_file)
    : MapTextSynthesizer(),  // initialize class fields
    config(make_shared<MTSConfig>(MTSConfig(config_file))),
    helper(make_shared<MTS_BaseHelper>(MTS_BaseHelper(config))),
    th(helper,config),
    bh(helper,config),
    noise_dist(config->getParamDouble("noise_sigma_alpha"),
            config->getParamDouble("noise_sigma_beta")),
    noise_gen(helper->rng2_, noise_dist)
{
    //initialize rng in BaseHelper
    uint64 seed = (uint64)config->getParamDouble("seed");
    helper->setSeed(seed != 0 ? seed : time(NULL));
}

MTSImplementation::~MTSImplementation() {
    //cout << "impl destructed" << endl;
}

void MTSImplementation::generateSample(string &caption, Mat &sample, int &actual_height){

    //cout << "start generate sample" << endl;
    vector<BGFeature> bg_features;
    bh.generateBgFeatures(bg_features);

    // set bg and text color (brightness) based on user configured parameters
    int bgcolor_min = config->getParamInt("bg_color_min");
    int textcolor_max = config->getParamInt("text_color_max");
    // assert colors are valid values
    if (bgcolor_min > 255 || textcolor_max < 0 || bgcolor_min<=textcolor_max) {
        cerr << "Invalid color input!" << endl;
        exit(1);
    }

    int bg_brightness = helper->rndBetween(bgcolor_min,255);
    int text_color = helper->rndBetween(0,textcolor_max);
    int contrast = bg_brightness - text_color;

    cairo_surface_t *text_surface;
    int height;
    int width;

    // set image height from user configured parameters
    int height_min = config->getParamInt("height_min");
    int height_max = config->getParamInt("height_max");
    if (height_min == height_max) {
        height = height_min;
    } else {
        height = helper->rndBetween(height_min,height_max); 
    }
    actual_height = height;

    // use TextHelper instance to generate synthetic text
    if (find(bg_features.begin(), bg_features.end(), Distracttext)!= bg_features.end()) {
        // do generate distractor text
        th.generateTextSample(caption,text_surface,height,width,text_color,true);
    } else {
        // dont generate distractor text
        th.generateTextSample(caption,text_surface,height,width,text_color,false);
    }

    //cout << "generating bg sample" << endl;
    //cout << "bg feature num " << bg_features.size() << endl; 

    // use BackgroundHelper to generate the background image
    cairo_surface_t *bg_surface;
    bh.generateBgSample(bg_surface, bg_features, height, width, bg_brightness, contrast);
    cairo_t *cr = cairo_create(bg_surface);
    cairo_set_source_surface(cr, text_surface, 0, 0);

    // set the blend alpha range using user configured parameters
    double blend_min=config->getParamDouble("blend_alpha_min");
    double blend_max=config->getParamDouble("blend_alpha_max");

    double blend_alpha=helper->rndBetween(blend_min,blend_max);

    // blend with alpha or not based on user set probability
    if(helper->rndProbUnder(config->getParamDouble("blend_prob"))){
        cairo_paint_with_alpha(cr, blend_alpha);
    } else {
        cairo_paint(cr); // dont blend
    }

    Mat sample_uchar = Mat(height,width,CV_8UC1,cv::Scalar_<uchar>(0,0,0));
    Mat sample_float = Mat(height,width,CV_32FC1,cv::Scalar_<float>(0,0,0));

    // convert cairo image to openCV Mat object
    cairoToMat(bg_surface, sample_uchar);

    sample_uchar.convertTo(sample_float, CV_32FC1, 1.0/255.0);

    // clean up cairo objects
    cairo_destroy(cr);
    cairo_surface_destroy(text_surface);
    cairo_surface_destroy(bg_surface);

    // add image smoothing using blur and noise
    addGaussianNoise(sample_float);
    addGaussianBlur(sample_float);

    addCompressionArtifacts(sample_float);

    bool zero_padding = true;
    if (config->getParamDouble("zero_padding")==0) zero_padding = false;

    if (!zero_padding) {
        sample = Mat(height,width,CV_8UC1,cv::Scalar_<uchar>(0,0,0));
    } else {
        int height_max = int(config->getParamDouble("height_max"));
        sample = Mat(height_max,width,CV_8UC1,cv::Scalar_<uchar>(0,0,0));
    }

    sample_float.convertTo(sample_uchar, CV_8UC1, 255.0);

    Mat sample_roi = sample(cv::Rect(0, 0, width, height));
    sample_uchar.copyTo(sample_roi);
}
