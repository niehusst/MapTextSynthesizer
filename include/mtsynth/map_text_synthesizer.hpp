#ifndef MAP_TEXT_SYNTHESIZER_HPP
#define MAP_TEXT_SYNTHESIZER_HPP

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
         * Setter method to initialize the blockyFonts_ field
         *
         * fntList - a list of fonts contained in a vector
         */
        CV_WRAP virtual void 
            setBlockyFonts(std::vector<String>& fntList) = 0;

        /*
         * Setter method to initialize the regularFonts_ field
         *
         * fntList - a list of fonts contained in a vector
         */
        CV_WRAP virtual void 
            setRegularFonts(std::vector<String>& fntList) = 0;

        /*
         * Setter method to initialize the cursiveFonts_ field
         *
         * fntList - a list of fonts contained in a vector
         */
        CV_WRAP virtual void 
            setCursiveFonts(std::vector<String>& fntList) = 0;

        /*
         * Set the collection of words to be displayed 
         *
         * words - a list of strings to be sampled
         */
        CV_WRAP virtual void 
            setSampleCaptions(std::vector<String>& words) = 0;

        /*
         * Generates a random bounded map-like text sample given a string
         * This is the principal function of the text synthciser
         *
         * caption - the label of the image. 
         * sample - the resulting text sample.
         */
        CV_WRAP virtual void 
            generateSample (CV_OUT String &caption, CV_OUT Mat& sample) = 0;

        /*
         * A wrapper for the protected MapTextSynthesizer constructor.
         * Use this method to create a MTS object.
         */
        CV_WRAP static Ptr<MapTextSynthesizer> 
        //CV_WRAP static MapTextSynthesizer* 
            create();

        /*
         * The destructor for the MapTextSynthesizer class 
         */ 
        virtual ~MapTextSynthesizer(){}
};

#endif // MAP_TEXT_SYNTHESIZER_HPP
