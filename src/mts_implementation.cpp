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

using namespace std;
using namespace cv;
using boost::random::beta_distribution;
using boost::random::variate_generator;

double MTSImplementation::getParam(string key) {
    double val = helper->getParam(key);
    return val;
}

unordered_map<string, double> MTSImplementation::parseConfig(string filename) {

    unordered_map<string, double> params = unordered_map<string, double>();

    string delimiter = "=";

    // open file
    ifstream infile(filename);
    if (! infile.is_open()) {
        cerr << "input file cannot be openned!" << endl;
        exit(1);
    }
    //CV_Assert(infile.is_open());

    string line, key, value;
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
            cerr << "A line does not contain delimiter in config file!" << endl;
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
            cout << "An unparseable value was encountered for variable "
                << key <<".\nPlease enter a valid number.\n";
            exit(1);
        }
        params.insert(pair<string, double>(key, val));

    }   
    // close file
    infile.close();
    return params;
}

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
    double scale = getParam("noise_sigma_scale");
    double shift = getParam("noise_sigma_shift");
    double sigma = round((pow(1/(noise_gen() + 0.1), 0.5) * scale + shift) * 100) / 100;

    // create noise matrix
    Mat noise = Mat(out.rows, out.cols, CV_32F);

    // populate noise with random values
    randn(noise, 0, sigma);

    // add noise to each channel
    out+=noise;
    threshold(out,out,1.0,1.0,THRESH_TRUNC);
    threshold(out,out,0,1.0,THRESH_TOZERO);
}

void MTSImplementation::addGaussianBlur(Mat& out) {
    // get user config parameters for kernel size
    int size_min = (int)getParam("blur_kernel_size_min") / 2;
    int size_max = (int)getParam("blur_kernel_size_max") / 2;
    int ker_size = (helper->rng() % (size_max-size_min) + size_min) * 2 + 1;

    GaussianBlur(out,out,Size(ker_size,ker_size),0,0,BORDER_REFLECT_101);
}

void MTSImplementation::updateFontNameList(std::vector<string>& font_list) {
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
        font_list.push_back(string(family_name));
    }   
    // clean up
    free (families);
}

MTSImplementation::MTSImplementation(string config_file)
    : MapTextSynthesizer(),  // initialize class fields
    fonts_{(&(this->blockyFonts_)),
        (&(this->regularFonts_)),
        (&(this->cursiveFonts_))},
        /*
    fonts_{std::shared_ptr<std::vector<string> >(&(this->blockyFonts_)),
        std::shared_ptr<std::vector<string> >(&(this->regularFonts_)),
        std::shared_ptr<std::vector<string> >(&(this->cursiveFonts_))},
        */
        helper(make_shared<MTS_BaseHelper>(MTS_BaseHelper(parseConfig(config_file)))),
        th(helper),
        bh(helper),
        noise_dist(getParam("noise_sigma_alpha"),
                getParam("noise_sigma_beta")),
        noise_gen(helper->rng2_, noise_dist)
{
    namedWindow("__w");
    waitKey(1);
    destroyWindow("__w");
    this->updateFontNameList(this->availableFonts_);

    //initialize rng in BaseHelper
    uint64 seed = (uint64)getParam("seed");
    helper->setSeed(seed != 0 ? seed : time(NULL));

    //set required fields for TextHelper instance (th)
    th.setFonts(fonts_);
    //th.setSampleCaptions(std::shared_ptr<std::vector<string> >(&(sampleCaptions_)));
    th.setSampleCaptions(&(sampleCaptions_));
}

MTSImplementation::~MTSImplementation() {
    cout << "impl destructed" << endl;
}

void MTSImplementation::setBlockyFonts(std::vector<string>& font_list){
    std::vector<string> availableList=this->availableFonts_;

    // loop through fonts in availableFonts_ to check if the system 
    // contains every font in the font_list
    for(size_t k = 0; k < font_list.size(); k++){
        if(std::find(availableList.begin(), availableList.end(), font_list[k]) == availableList.end()){
            //CV_Error(Error::StsError,"The font name list must only contain fonts in your system");
            cerr << "The blocky font name list must only contain fonts in your system" << endl;
            exit(1);
        }
    }
    this->blockyFonts_.assign(font_list.begin(),font_list.end());
}

void MTSImplementation::setBlockyFonts(string font_file){
    std::vector<string> fonts = MapTextSynthesizer::readLines(font_file);
    setBlockyFonts(fonts);
}

void MTSImplementation::setRegularFonts(std::vector<string>& font_list){
    std::vector<string> availableList=this->availableFonts_;

    // loop through fonts in availableFonts_ to check if the system 
    // contains every font in the font_list
    for(size_t k = 0; k < font_list.size(); k++){
        if(std::find(availableList.begin(), availableList.end(), font_list[k]) == availableList.end()){
            //CV_Error(Error::StsError,"The font name list must only contain fonts in your system");
            cerr << "The regular font name list must only contain fonts in your system" << endl;
            exit(1);
        }
    }
    this->regularFonts_.assign(font_list.begin(),font_list.end());
}

void MTSImplementation::setRegularFonts(string font_file){
    std::vector<string> fonts = MapTextSynthesizer::readLines(font_file);
    setRegularFonts(fonts);
}

void MTSImplementation::setCursiveFonts(std::vector<string>& font_list){
    std::vector<string> availableList=this->availableFonts_;

    // loop through fonts in availableFonts_ to check if the system 
    // contains every font in the font_list
    for(size_t k = 0; k < font_list.size(); k++){
        if(std::find(availableList.begin(), availableList.end(), font_list[k]) == availableList.end()){
            //CV_Error(Error::StsError,"The font name list must only contain fonts in your system");
            cerr << "The cursive font name list must only contain fonts in your system" << endl;
            exit(1);
        }
    }
    this->cursiveFonts_.assign(font_list.begin(),font_list.end());
}

void MTSImplementation::setCursiveFonts(string font_file){
    std::vector<string> fonts = MapTextSynthesizer::readLines(font_file);
    setCursiveFonts(fonts);
}

void MTSImplementation::setSampleCaptions(std::vector<string>& words) {
    this->sampleCaptions_.assign(words.begin(),words.end());
}

void MTSImplementation::setSampleCaptions(string caption_file){
    std::vector<string> captions = MapTextSynthesizer::readLines(caption_file);
    setSampleCaptions(captions);
}

void MTSImplementation::generateSample(CV_OUT string &caption, CV_OUT Mat &sample, CV_OUT int &actual_height){

    //cout << "start generate sample" << endl;
    std::vector<BGFeature> bg_features;
    bh.generateBgFeatures(bg_features);
    //cout << "after bg feature" << endl;

    // set bg and text color (brightness) based on user configured parameters
    int bgcolor_min = (int)getParam("bg_color_min");
    int textcolor_max = (int)getParam("text_color_max");
    // assert colors are valid values
    if (bgcolor_min > 255 || textcolor_max < 0 || bgcolor_min<=textcolor_max) {
        cerr << "Invalid color input!" << endl;
        exit(1);
    }
    //CV_Assert(bgcolor_min<=255);
    //CV_Assert(textcolor_max>=0);
    //CV_Assert(bgcolor_min>textcolor_max);

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
        height = (helper->rng()%(height_max-height_min+1))+height_min;
    }
    actual_height = height;

    string text;
    //cout << "generating text sample" << endl;
    // use TextHelper instance to generate synthetic text
    if (find(bg_features.begin(), bg_features.end(), Distracttext)!= bg_features.end()) {
        // do generate distractor text
        th.generateTextSample(text,text_surface,height,width,text_color,true);
    } else {
        // dont generate distractor text
        th.generateTextSample(text,text_surface,height,width,text_color,false);
    }
    caption = string(text);

    //cout << "generating bg sample" << endl;
    cout << "bg feature num " << bg_features.size() << endl; 

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
    } else {
        cairo_paint(cr); // dont blend
    }

    Mat sample_uchar = Mat(height,width,CV_8UC1,Scalar_<uchar>(0,0,0));
    Mat sample_float = Mat(height,width,CV_32FC1,Scalar_<float>(0,0,0));

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
        sample = Mat(height,width,CV_8UC1,Scalar_<uchar>(0,0,0));
    } else {
        int height_max = int(getParam("height_max"));
        sample = Mat(height_max,width,CV_8UC1,Scalar_<uchar>(0,0,0));
    }

    sample_float.convertTo(sample_uchar, CV_8UC1, 255.0);

    Mat sample_roi = sample(Rect(0, 0, width, height));
    sample_uchar.copyTo(sample_roi);
}
