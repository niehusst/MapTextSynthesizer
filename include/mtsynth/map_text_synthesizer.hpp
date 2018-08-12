#ifndef MAP_TEXT_SYNTHESIZER_HPP
#define MAP_TEXT_SYNTHESIZER_HPP

#include <string>
#include <memory>
#include <opencv2/core/mat.hpp> //cv::Mat

/*
 * Class that renders synthetic text images for training a CNN 
 * on word recognition in historical maps
 */
class CV_EXPORTS_W MapTextSynthesizer{

    protected://--------------------PROTECTED---------------------------
        /*
         * Constructor
         */
        MapTextSynthesizer();

    public://------------------------PUBLIC---------------------------

        /*
         * Generates a random bounded map-like text sample given a string
         * This is the principal function of the text synthesizer
         *
         * caption - the label of the image. 
         * sample - the resulting text sample.
         */
        CV_WRAP virtual void 
          generateSample (CV_OUT std::string &caption, CV_OUT cv::Mat &sample,
                            CV_OUT int &actual_height) = 0;

        /*
         * A wrapper for the protected MapTextSynthesizer constructor.
         * Use this method to create a MTS object.
         */
        CV_WRAP static cv::Ptr<MapTextSynthesizer> 
            create(std::string config_file);

        /*
         * The destructor for the MapTextSynthesizer class 
         */ 
        virtual ~MapTextSynthesizer(){}
};

#endif // MAP_TEXT_SYNTHESIZER_HPP
