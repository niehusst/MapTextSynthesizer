#ifndef MAP_TEXT_SYNTHESIZER_HPP
#define MAP_TEXT_SYNTHESIZER_HPP

#include <string>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

/*
 * Class that renders synthetic text images for training a CNN 
 * on word recognition in historical maps
 */
class CV_EXPORTS_W MapTextSynthesizer{

    protected:
        /*
         * Constructor. 
         */
        MapTextSynthesizer();

    public:

        /*
         * Generates a random bounded map-like text sample given a string
         * This is the principal function of the text synthesizer
         *
         * caption - the label of the image. 
         * sample - the resulting text sample.
         */
        CV_WRAP virtual void 
            generateSample (CV_OUT string &caption, CV_OUT Mat &sample, CV_OUT int &actual_height) = 0;

        /*
         * A wrapper for the protected MapTextSynthesizer constructor.
         * Use this method to create a MTS object.
         */
        CV_WRAP static Ptr<MapTextSynthesizer> 
        //CV_WRAP static MapTextSynthesizer* 
            create(string config_file);

        /*
         * The destructor for the MapTextSynthesizer class 
         */ 
        virtual ~MapTextSynthesizer(){}
};

#endif // MAP_TEXT_SYNTHESIZER_HPP
