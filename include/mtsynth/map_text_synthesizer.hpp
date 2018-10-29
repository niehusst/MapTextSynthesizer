#ifndef MAP_TEXT_SYNTHESIZER_HPP
#define MAP_TEXT_SYNTHESIZER_HPP

#include <string>
#include <memory>
#include <opencv2/core/mat.hpp> //cv::Mat

/*
 * Class that renders synthetic text images for training a CNN 
 * on word recognition in historical maps
 */
class MapTextSynthesizer{

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
         * actual_height - the actual height of sample.
         * x1-x4,y1-y4 - coordinates of the text bounding box
         */
        virtual void 
            generateSample (std::string &caption, cv::Mat &sample, 
                    int &actual_height, double &x1,double &y1,double &x2,
                    double &y2,double &x3,double &y3,double &x4,double &y4) = 0;

        /*
         * A wrapper for the protected MapTextSynthesizer constructor.
         * Use this method to create a MTS object.
         */
        static cv::Ptr<MapTextSynthesizer> 
            create(std::string config_file);

        /*
         * The destructor for the MapTextSynthesizer class 
         */ 
        virtual ~MapTextSynthesizer(){}
};

#endif // MAP_TEXT_SYNTHESIZER_HPP
