#ifndef MTS_IMPLEMENTATION_HPP
#define MTS_IMPLEMENTATION_HPP

#include <string>

// pango/cairo includes
#include <glib.h>
#include <pango/pangocairo.h>

// opencv includes
#include <opencv2/core/mat.hpp> //cv::Mat

// local files
#include "map_text_synthesizer.hpp"
#include "mts_basehelper.hpp"
#include "mts_config.hpp"
#include "mts_texthelper.hpp"
#include "mts_bghelper.hpp"

using boost::random::mt19937;
using boost::random::gamma_distribution;
using boost::random::variate_generator;

class MTSImplementation: public MapTextSynthesizer{

protected://-------------PROTECTED METHODS AND FIELDS------------------------

        /* Converts cairo surface to mat object in opencv
         *
         * surface - the cairo surface to be converted
         * mat - the output map object containing the RGB channel
         *      of surface. alpha channel is thrown away.
         *
         * Original code for this method is from Andrey Smorodov
         * url: https://stackoverflow.com/questions/19948319/how-to-convert-cairo-image-surface-to-opencv-mat-in-c
         */
        static void cairoToMat(cairo_surface_t *surface,cv::Mat &mat);

  
        /* Adds Gaussian noise to out
         *
         * out - the input and output image
         */
        void addGaussianNoise(cv::Mat& out);

  
        /* Adds Gaussian blur to out
         *
         * out - the input and output image
         */
        void addGaussianBlur(cv::Mat& out);

  
        std::shared_ptr<MTSConfig> config;
        std::shared_ptr<MTS_BaseHelper> helper;
        MTS_TextHelper th;
        MTS_BackgroundHelper bh;

        /* Generator for sigma used in Gaussian noise method. */
        gamma_distribution<> noise_dist;
        variate_generator<mt19937, gamma_distribution<> > noise_gen;

public://-----------------PUBLIC METHODS AND FIELDS------------------------

        /* Constructor */
        MTSImplementation(std::string config_file);

        /* Destructor */
        ~MTSImplementation();


        /*
         * Generate a sample image
         *
         * caption - the text displayed in the image
         * sample - the opencv matrix that actually contains the image data
         * actual_height - the actual height of sample in pixels. 
         */
        void generateSample(CV_OUT std::string &caption, CV_OUT cv::Mat &sample,
                            CV_OUT int &actual_height);

};

#endif
