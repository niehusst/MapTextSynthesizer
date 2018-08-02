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
    setBlockyFonts(std::vector<std::string> &font_list) = 0;

  /*
   * An overload of setBlockyfonts that allows the
   * user to set the blocky fonts used from a file
   * instead of from a vector.
   *
   * caption_file - the name of the file where the desired 
   *                fonts can be found
   */
  CV_WRAP virtual void 
    setBlockyFonts(std::string font_file) = 0;

  /*
   * Setter method to initialize the regularFonts_ field
   *
   * font_list - a list of fonts contained in a vector
   */
  CV_WRAP virtual void 
    setRegularFonts(std::vector<std::string> &font_list) = 0;

  /*
   * An overload of setRegularfonts that allows the
   * user to set the regular fonts used from a file
   * instead of from a vector.
   *
   * caption_file - the name of the file where the desired 
   *                fonts can be found
   */
  CV_WRAP virtual void 
    setRegularFonts(std::string font_file) = 0;

  /*
   * Setter method to initialize the cursiveFonts_ field
   *
   * font_list - a list of fonts contained in a vector
   */
  CV_WRAP virtual void 
    setCursiveFonts(std::vector<std::string> &font_list) = 0;

  /*
   * An overload of setCursivefonts that allows the
   * user to set the cursive fonts used from a file
   * instead of from a vector.
   *
   * caption_file - the name of the file where the desired 
   *                fonts can be found
   */
  CV_WRAP virtual void 
    setCursiveFonts(std::string font_file) = 0;

  /*
   * Set the collection of words to be displayed 
   *
   * words - a list of std::strings to be sampled
   */
  CV_WRAP virtual void 
    setSampleCaptions(std::vector<std::string> &words) = 0;

  /*
   * An overload of setSampleCaptions that allows the
   * user to set the image captions used from a file
   * instead of from a vector.
   *
   * caption_file - the name of the file where the desired 
   *                words can be found
   */
  CV_WRAP virtual void 
    setSampleCaptions(std::string caption_file) = 0;

  /*
   * Generates a random bounded map-like text sample given a std::string
   * This is the principal function of the text synthesizer
   *
   * caption - the label of the image. 
   * sample - the resulting text sample.
   */
  CV_WRAP virtual void 
    generateSample (CV_OUT std::string &caption, CV_OUT cv::Mat &sample,
                    CV_OUT int &actual_height) = 0;

  /*
   * A Helper method for users to easily read lines from a file
   *
   * filename - the path to the file. 
   */
  CV_WRAP static std::vector<std::string> 
    readLines(std::string filename);
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
