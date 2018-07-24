#ifndef MTS_IMPLEMENTATION_HPP
#define MTS_IMPLEMENTATION_HPP

#include <string>

// pango/cairo includes
#include <glib.h>
#include <pango/pangocairo.h>

// opencv includes
#include <opencv2/opencv.hpp>

// our method includes
#include "mtsynth/map_text_synthesizer.hpp"
#include "mts_basehelper.hpp"
#include "mts_texthelper.hpp"
#include "mts_bghelper.hpp"

using namespace std;
using namespace cv;
using boost::random::mt19937;
using boost::random::gamma_distribution;
using boost::random::variate_generator;

class MTSImplementation: public MapTextSynthesizer{

    protected:
        /* Returns the corresponding value given key in config.txt
         *
         * key - key in hashmap constructed from config.txt
         */
        double getParam(string key);

        /*
         * Parses a text file for variable names and values, using '='
         * as the delimeter, and places the data into parameter_map.
         *
         * filename - the name of the file to parse for values
         */
        static unordered_map<string, double> parseConfig(string filename);

        /* Converts cairo surface to mat object in opencv
         *
         * surface - the cairo surface to be converted
         * mat - the output map object containing the RGB channel
         *      of surface. alpha channel is thrown away.
         *
         * Original code for this method is from Andrey Smorodov
         * url: https://stackoverflow.com/questions/19948319/how-to-convert-cairo-image-surface-to-opencv-mat-in-c
         */
        static void cairoToMat(cairo_surface_t *surface,Mat &mat);

        /* Adds Gaussian noise to out
         *
         * out - the input and output image
         */
        void addGaussianNoise(Mat& out);

        /* Adds Gaussian blur to out
         *
         * out - the input and output image
         */
        void addGaussianBlur(Mat& out);

        /* Updates the list of available system fonts by 
         * clearing and reloading font_list
         *
         * font_list - the output
         * Base of this method from Ben K. Bullock at
         * url: https://www.lemoda.net/pango/list-fonts/index.html
         */
        void updateFontNameList(std::vector<string>& font_list);

        /* The list of blocky font names set by user. */
        std::vector<string> blockyFonts_ = std::vector<string>();

        /* The list of regular font names set by user. */
        std::vector<string> regularFonts_ = std::vector<string>();

        /* The list of cursive font names set by user. */
        std::vector<string> cursiveFonts_ = std::vector<string>();

        /* The list of available system font names. */
        std::vector<string> availableFonts_ = std::vector<string>();

        /* Pointers pointing to blocky, regular, cursive font lists. */
        //std::shared_ptr<std::vector<string> > fonts_[3];
        std::vector<string>* fonts_[3];

        /* The list of captions to sample from.*/
        std::vector<string> sampleCaptions_ = std::vector<string>();

        shared_ptr<MTS_BaseHelper> helper;
        MTS_TextHelper th;
        MTS_BackgroundHelper bh;

        /* Generator for sigma used in Gaussian noise method. */
        gamma_distribution<> noise_dist;
        variate_generator<mt19937, gamma_distribution<> > noise_gen;

    public:

        /* Constructor */
        MTSImplementation(string config_file);

        /* Destructor */
        ~MTSImplementation();

        /* See MapTextSynthesizer for documentations.*/

        void setBlockyFonts(std::vector<string>& font_list);
        void setBlockyFonts(string font_file);

        void setRegularFonts(std::vector<string>& font_list);
        void setRegularFonts(string font_file);

        void setCursiveFonts(std::vector<string>& font_list);
        void setCursiveFonts(string font_file);

        void setSampleCaptions(std::vector<string>& words);
        void setSampleCaptions(string caption_file);

        void generateSample(CV_OUT string &caption, CV_OUT Mat &sample, CV_OUT int &actual_height);
};

#endif
