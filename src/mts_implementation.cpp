/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * mts_implementation.cpp contains the class method definitions for the       *
 * MTSImplementation class, which handles the integration of the background   *
 * and text into one synthetic image.                                         *
 * Copyright (C) 2018, Liam Niehus-Staab and Ziwen Chen                       *
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

// standard includes
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
#include <utility>
#include <vector>
#include <unordered_map>

// opencv includes
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/core/mat.hpp>   // cv::Mat

// Pango/cairo includes
#include <glib.h>
#include <pango/pangocairo.h>

#include "mts_implementation.hpp"


using boost::random::beta_distribution;
using boost::random::variate_generator;

double MTSImplementation::getParam(std::string key) {
    double val = helper->getParam(key);
    return val;
}

std::unordered_map<std::string, double> MTSImplementation::parseConfig(std::string filename) {

    std::unordered_map<std::string, double> params = std::unordered_map<std::string, double>();

    std::string delimiter = "=";

    // open file
    std::ifstream infile(filename);
    if (! infile.is_open()) {
        std::cerr << "input file cannot be openned!" << std::endl;
        exit(1);
    }

    std::string line, key, value;
    double val;

    // parse file line by line
    while (getline(infile, line)) {
        // if line is empty or is a comment, erase it
        size_t com_pos = line.find("//");
        if (com_pos != line.npos) {
            line.erase(com_pos);
        }
        if (line.length()==0) {
            continue;
        }
        size_t pos = line.find(delimiter);
        if (pos == line.npos) {
            std::cerr << "A line does not contain delimiter in config file!"
                      << std::endl;
            exit(1);
        }
        //CV_Assert(pos != line.npos);

        key = line.substr(0, pos);
        value = line.substr(pos+1, line.npos-pos);
        char * err_flag;
        val = strtod(value.c_str(), &err_flag);

        // check if strtod produced error in casting
        if (value.c_str() == err_flag && val == 0) { 
            // tell user there was an error at this point and exit failure
          std::cout << "An unparseable value was encountered for variable "
                << key <<".\nPlease enter a valid number.\n";
            exit(1);
        }
        params.insert(std::pair<std::string, double>(key, val));

    }   
    // close file
    infile.close();
    return params;
}

void MTSImplementation::cairoToMat(cairo_surface_t *surface,cv::Mat &mat) {
    // make a 4 channel opencv matrix
    cv::Mat mat4 = cv::Mat(cairo_image_surface_get_height(surface),cairo_image_surface_get_width(surface),CV_8UC4,cairo_image_surface_get_data(surface));

    std::vector<cv::Mat> channels;

    cv::split(mat4,channels);

    //iterate through all channels
    mat = channels[0];
}

void MTSImplementation::addGaussianNoise(cv::Mat& out) {
    // get and use user config parameters to set sigma
    double scale = getParam("noise_sigma_scale");
    double shift = getParam("noise_sigma_shift");
    double sigma = round((pow(1/(noise_gen() + 0.1), 0.5) * scale + shift) * 100) / 100;

    // create noise matrix
    cv::Mat noise = cv::Mat(out.rows, out.cols, CV_32F);

    // populate noise with random values
    randn(noise, 0, sigma);

    // add noise to each channel
    out+=noise;
    cv::threshold(out,out,1.0,1.0,cv::THRESH_TRUNC);
    cv::threshold(out,out,0,1.0,cv::THRESH_TOZERO);
}

void MTSImplementation::addGaussianBlur(cv::Mat& out) {
    // get user config parameters for kernel size
    int size_min = (int)getParam("blur_kernel_size_min") / 2;
    int size_max = (int)getParam("blur_kernel_size_max") / 2;
    int ker_size = (helper->rng() % (size_max-size_min) + size_min) * 2 + 1;

    cv::GaussianBlur(out,out,cv::Size(ker_size,ker_size),0,0,cv::BORDER_REFLECT_101);
}

void MTSImplementation::updateFontNameList(std::vector<std::string>& font_list) {
    // clear existing fonts for a fresh load of available fonts
    font_list.clear(); 

    PangoFontFamily ** families;
    int num_families;
    PangoFontMap * fontmap;

    fontmap = pango_cairo_font_map_get_default();
    pango_font_map_list_families (fontmap, &families, &num_families);

    // iterativly add all available fonts to font_list
    for (int k = 0; k < num_families; k++) {
        PangoFontFamily * family = families[k];
        const char * family_name;
        family_name = pango_font_family_get_name (family);
        font_list.push_back(std::string(family_name));
    }   
    // clean up
    free (families);
}

MTSImplementation::MTSImplementation(std::string config_file)
    : MapTextSynthesizer(),  // initialize class fields
    fonts_{(&(this->blockyFonts_)),
        (&(this->regularFonts_)),
        (&(this->cursiveFonts_))},

        helper(std::make_shared<MTS_BaseHelper>(MTS_BaseHelper(parseConfig(config_file)))),
        th(helper),
        bh(helper),
        noise_dist(getParam("noise_sigma_alpha"),
                getParam("noise_sigma_beta")),
        noise_gen(helper->rng2_, noise_dist)
{
  cv::namedWindow("__w");
  cv::waitKey(1);
  cv::destroyWindow("__w");
    this->updateFontNameList(this->availableFonts_);

    //initialize rng in BaseHelper
    uint64 seed = (uint64)getParam("seed");
    helper->setSeed(seed != 0 ? seed : time(NULL));

    //set required fields for TextHelper instance (th)
    th.setFonts(fonts_);
    th.setSampleCaptions(&(sampleCaptions_));
}

MTSImplementation::~MTSImplementation() {}

void MTSImplementation::setBlockyFonts(std::vector<std::string>& font_list){
  std::vector<std::string> availableList=this->availableFonts_;

  // loop through fonts in availableFonts_ to check if the system 
  // contains every font in the font_list
  for(size_t k = 0; k < font_list.size(); k++){
    if(std::find(availableList.begin(), availableList.end(), font_list[k]) == availableList.end()){
      std::cerr << "The font name list must only contain fonts in your "
                << "system.\nThe font " << font_list[k]
                << " is not in your system.\n";
      exit(1);
    }
  }
  this->blockyFonts_.assign(font_list.begin(),font_list.end());
}

void MTSImplementation::setBlockyFonts(std::string font_file){
    std::vector<std::string> fonts = MapTextSynthesizer::readLines(font_file);
    setBlockyFonts(fonts);
}

void MTSImplementation::setRegularFonts(std::vector<std::string>& font_list){
    std::vector<std::string> availableList=this->availableFonts_;

    // loop through fonts in availableFonts_ to check if the system 
    // contains every font in the font_list
    for(size_t k = 0; k < font_list.size(); k++){
        if(std::find(availableList.begin(), availableList.end(), font_list[k]) == availableList.end()){
          std::cerr << "The font name list must only contain fonts in your "
                    << "system.\nThe font " << font_list[k]
                    << " is not in your system.\n";
          exit(1);
        }
    }
    this->regularFonts_.assign(font_list.begin(),font_list.end());
}

void MTSImplementation::setRegularFonts(std::string font_file){
    std::vector<std::string> fonts = MapTextSynthesizer::readLines(font_file);
    setRegularFonts(fonts);
}

void MTSImplementation::setCursiveFonts(std::vector<std::string>& font_list){
    std::vector<std::string> availableList=this->availableFonts_;

    // loop through fonts in availableFonts_ to check if the system 
    // contains every font in the font_list
    for(size_t k = 0; k < font_list.size(); k++){
        if(std::find(availableList.begin(), availableList.end(), font_list[k]) == availableList.end()){
            std::cerr << "The font name list must only contain fonts in your "
                      << "system.\nThe font " << font_list[k]
                      << " is not in your system.\n";
            exit(1);
        }
    }
    this->cursiveFonts_.assign(font_list.begin(),font_list.end());
}

void MTSImplementation::setCursiveFonts(std::string font_file){
    std::vector<std::string> fonts = MapTextSynthesizer::readLines(font_file);
    setCursiveFonts(fonts);
}

void MTSImplementation::setSampleCaptions(std::vector<std::string>& words) {
    this->sampleCaptions_.assign(words.begin(),words.end());
}

void MTSImplementation::setSampleCaptions(std::string caption_file){
    std::vector<std::string> captions = MapTextSynthesizer::readLines(caption_file);
    setSampleCaptions(captions);
}

void MTSImplementation::generateSample(CV_OUT std::string &caption, CV_OUT cv::Mat &sample, CV_OUT int &actual_height){

    std::vector<BGFeature> bg_features;
    bh.generateBgFeatures(bg_features);

    // set bg and text color (brightness) based on user configured parameters
    int bgcolor_min = (int)getParam("bg_color_min");
    int textcolor_max = (int)getParam("text_color_max");
    // assert colors are valid values

    if (bgcolor_min > 255 || textcolor_max < 0 || bgcolor_min<=textcolor_max) {
        std::cerr << "Invalid color input!" << std::endl;
        exit(1);
    }

    int bg_brightness = helper->rng()%(255-bgcolor_min+1)+bgcolor_min;
    int text_color = helper->rng()%(textcolor_max+1);
    int contrast = bg_brightness - text_color;

    cairo_surface_t *text_surface;
    int height;
    int width;

    // set image height from user configured parameters
    int height_min = (int)getParam("height_min");
    int height_max = (int)getParam("height_max");

    if (height_min == height_max) {
        height = height_min;
    } else {
        height = (helper->rng() % (height_max - height_min + 1)) + height_min;
    }

    actual_height = height;

    std::string text;

    // use TextHelper instance to generate synthetic text
    if (std::find(bg_features.begin(), bg_features.end(), Distracttext)!= bg_features.end()) {
        // do generate distractor text
        th.generateTextSample(text,text_surface,height,width,text_color,true);
    } else {
        // dont generate distractor text
        th.generateTextSample(text,text_surface,height,width,text_color,false);
    }
    caption = std::string(text);

    // use BackgroundHelper to generate the background image
    cairo_surface_t *bg_surface;
    bh.generateBgSample(bg_surface, bg_features, height, width, bg_brightness, contrast);
    cairo_t *cr = cairo_create(bg_surface);
    cairo_set_source_surface(cr, text_surface, 0, 0);

    // set the blend alpha range using user configured parameters
    int blend_min=(int)(100 * getParam("blend_alpha_min"));
    int blend_max=(int)(100 * getParam("blend_alpha_max"));

    double blend_alpha=(helper->rng()%(blend_max-blend_min+1)+blend_min)/100.0;

    // blend with alpha or not based on user set probability
    if(helper->rndProbUnder(getParam("blend_prob"))){
        cairo_paint_with_alpha(cr, blend_alpha);
    } else { // dont blend
        cairo_paint(cr);
    }

    cv::Mat sample_uchar = cv::Mat(height,width,CV_8UC1,cv::Scalar_<uchar>(0,0,0));
    cv::Mat sample_float = cv::Mat(height,width,CV_32FC1,cv::Scalar_<float>(0,0,0));

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

    bool zero_padding = true;
    if (getParam("zero_padding")==0) zero_padding = false;

    if (!zero_padding) {
      sample = cv::Mat(height,width,CV_8UC1,cv::Scalar_<uchar>(0,0,0));
    } else {
        int height_max = int(getParam("height_max"));
        sample = cv::Mat(height_max,width,CV_8UC1,cv::Scalar_<uchar>(0,0,0));
    }

    sample_float.convertTo(sample_uchar, CV_8UC1, 255.0);

    cv::Mat sample_roi = sample(cv::Rect(0, 0, width, height));
    sample_uchar.copyTo(sample_roi);
}
