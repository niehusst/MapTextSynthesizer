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
#include "mts_config.hpp"
#include "mts_texthelper.hpp"
#include "mts_bghelper.hpp"

using std::string;
using std::shared_ptr;
using cv::Mat;
using boost::random::mt19937;
using boost::random::gamma_distribution;
using boost::random::variate_generator;

class MTSImplementation: public MapTextSynthesizer{

    protected:

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

        /* Adds jpeg compression artifacts to img
         *
         * out - the input and output image
         * Adapted from Anguelos's code: https://github.com/anguelos/opencv_contrib/blob/gsoc_final_submission/modules/text/src/text_synthesizer.cpp
         */
        void addCompressionArtifacts(Mat& out);

        shared_ptr<MTSConfig> config;
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

        /* Generate one image sample with ground truth label
         *
         * caption - the generated label
         * sample - the generated image 
         * actual_height - the actaul height of generated image 
         */
        void generateSample(string &caption, Mat &sample, int &actual_height);
};

#endif
