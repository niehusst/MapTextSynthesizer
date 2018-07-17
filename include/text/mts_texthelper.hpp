#ifndef MTS_TEXTHELPER_HPP
#define MTS_TEXTHELPER_HPP

#include <pango/pangocairo.h>
#include <vector>
#include <string>
#include <memory>

#include "mts_basehelper.hpp"

using boost::random::beta_distribution;
using boost::random::variate_generator;

namespace cv{
    namespace text{

        /*
         * A class to handle text transformation in vector space, and pango
         * text rendering 
         */ 
        class MTS_TextHelper {
            private:// --------------- PRIVATE METHODS AND FIELDS ------------------------

                /* See MapTextSynthesizer class for the documentation*/
                std::shared_ptr<std::vector<String> > *fonts_;
                std::shared_ptr<std::vector<String> > sampleCaptions_;

                /* Generator for the spacing degree */
                beta_distribution<> spacing_dist;
                variate_generator<mt19937, beta_distribution<> > spacing_gen;

                /* Generator for the stretching degree */
                beta_distribution<> stretch_dist;
                variate_generator<mt19937, beta_distribution<> > stretch_gen;

                /* Returns the value of the parameter given key */
                double
                    getParam(string key);

                /*
                 * Returns a random latin character or numeral
                 */
                char
                    randomChar();

                /*
                 * Generates distractor text with random size and rotation to appear
                 * on the surface along with the main text
                 *
                 * cr - cairo context
                 * width - surface width
                 * height - surface height
                 * font - cstring holding the desired font for the distractor text
                 */
                void
                    distractText (cairo_t *cr, int width, int height, char *font); 


                /*
                 * Generates a string that represents a font + fontsize.
                 *
                 * font - output that will store the generated string
                 * fontsize - the size of the font
                 */
                void
                    generateFont(char *font, int fontsize);

                /*
                 * Generates the text features and pass back to outputs
                 *
                 * rotate_angle - the angle to rotate
                 * curved - whether to curve or not
                 * spacing- the spacing between characters (in point)
                 * stretch_deg - the horizontal text scaling factor
                 * x_pad - padding in x-direction
                 * y_pad - padding in y-direction
                 * scale - scaling factor of the entire text
                 * desc - the pango font description
                 * height - height of canvas
                 */
                void
                    generateFeatures(double &rotated_angle, bool &curved, 
                            double &spacing_deg, double &spacing, 
                            double &stretch_deg, int &x_pad, 
                            int &y_pad, double &scale, PangoFontDescription *&desc,
                            int height);

                /*
                 * Get the extents of a text ink
                 *
                 * layout - the pango layout
                 * desc - the pango font description
                 * x - the x coord of the top left corner
                 * y - the y coord of the top left corner
                 * w - the width of the ink
                 * h - the height of the ink
                 * size - the size of the text
                 */
                void
                    getTextExtents(PangoLayout *layout, PangoFontDescription *desc, int &x, int &y, int &w, int &h, int &size);

                /*
                 * Generates a text image without background
                 *
                 * text_surface - surface to draw the text on
                 * caption - the text to draw
                 * height - the height of the canvas
                 * width - output to store the width of the generated image
                 * text_color - color of text
                 * distract - whether to draw distracting text or not
                 */
                void
                    generateTextPatch(cairo_surface_t *&text_surface, std::string caption,int height,int &width, int text_color, bool distract);


            public:// --------------------- PUBLIC METHODS -------------------------------

                /* The base helper */
                shared_ptr<MTS_BaseHelper> helper;

                /* Constructor */
                MTS_TextHelper(shared_ptr<MTS_BaseHelper> h);

                /*
                 * A setter method for the private fonts_ field
                 *
                 * data - an array of vectors of strings that are font names
                 */
                void
                    setFonts(std::shared_ptr<std::vector<String> > *data);


                /*
                 * A setter method for the private sampleCaptions_ field
                 *
                 * data - a vector of strings containing words to be displayed
                 */
                void
                    setSampleCaptions(std::shared_ptr<std::vector<String> > data);


                /*
                 * Provides the randomly rendered text 
                 *
                 * caption - the string which will be rendered. 
                 * text_surface - an out variable containing a 32FC3 matrix with the rendered 
                 *          text including border and shadow.
                 * height - height of the surface
                 * width - width of the surface that will be determined
                 * text_color - the grayscale color value for the text
                 * distract - flag that dictates whether distractor text will be present
                 */
                void 
                    generateTextSample(std::string &caption, cairo_surface_t *&text_surface, int height, int &width, int text_color, bool distract);

        };

    }
}

#endif
