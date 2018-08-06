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
         * Setter method to initialize the blockyFonts_ field
         *
         * font_list - a list of fonts contained in a vector
         */
        CV_WRAP virtual void 
            setBlockyFonts(std::vector<string>& font_list) = 0;

        CV_WRAP virtual void 
            setBlockyFonts(string font_file) = 0;

        /*
         * Setter method to initialize the regularFonts_ field
         *
         * font_list - a list of fonts contained in a vector
         */
        CV_WRAP virtual void 
            setRegularFonts(std::vector<string>& font_list) = 0;

        CV_WRAP virtual void 
            setRegularFonts(string font_file) = 0;

        /*
         * Setter method to initialize the cursiveFonts_ field
         *
         * font_list - a list of fonts contained in a vector
         */
        CV_WRAP virtual void 
            setCursiveFonts(std::vector<string>& font_list) = 0;

        CV_WRAP virtual void 
            setCursiveFonts(string font_file) = 0;

        /*
         * Set the collection of words to be displayed 
         *
         * words - a list of strings to be sampled
         */
        CV_WRAP virtual void 
            setSampleCaptions(std::vector<string>& words) = 0;

        CV_WRAP virtual void 
            setSampleCaptions(string caption_file) = 0;

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
         * A Helper method for users to easily read lines from a file
         *
         * filename - the path to the file. 
         */
        CV_WRAP static vector<string> 
            readLines(string filename);
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