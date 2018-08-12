#ifndef MTS_TEXTHELPER_HPP
#define MTS_TEXTHELPER_HPP

#include <vector>
#include <string>
#include <memory>

#include <pango/pangocairo.h>

#include "mts_basehelper.hpp"
#include "mts_config.hpp"

using boost::random::beta_distribution;
using boost::random::gamma_distribution;
using boost::random::variate_generator;

/*
 * A class to handle text transformation in vector space, and pango
 * text rendering 
 */ 
class MTS_TextHelper {
private:// --------------- PRIVATE METHODS AND FIELDS ------------------------

        /* Updates the list of available system fonts by
         * clearing and reloading font_list
         *
         * font_list - the output
         * Base of this method from Ben K. Bullock at
         * url: https://www.lemoda.net/pango/list-fonts/index.html
         */
        void updateFontNameList(std::vector<std::string>& font_list);


        /* Adds a list of fonts to fonts*/
        void addFontlist(std::vector<std::string>& font_list);
        void addFontlist(std::string font_file);


        /* Adds a list of captions to captions*/
        void addCaptionlist(std::vector<std::string>& words);
        void addCaptionlist(std::string caption_file);


        /* The list of available system font names. */
        std::vector<std::string> availableFonts_ = std::vector<std::string>();

        /* A list of fonts */
        std::vector<std::string> fonts_;

        /* A list of captions */
        std::vector<std::string> captions_;

        /* Generator for the spacing degree */
        beta_distribution<> spacing_dist;
        variate_generator<mt19937, beta_distribution<> > spacing_gen;

        /* Generator for the stretching degree */
        beta_distribution<> stretch_dist;
        variate_generator<mt19937, beta_distribution<> > stretch_gen;

        /* Generator for the digit length*/
        gamma_distribution<> digit_len_dist;
        variate_generator<mt19937, gamma_distribution<> > digit_len_gen;

        /*
         * Returns a random latin character or numeral or punctuation
         */
        char
            randomChar();

        /*
         * Returns a random digit
         */
        char
            randomDigit();

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
                             double &stretch_deg, int &x_pad, int &y_pad,
                             double &scale, PangoFontDescription *&desc,
                             int height);

        /*
         * Creates a curved text whose shape will not be deformed according
         * to the curvature
         *
         * cr - cairo context
         * layout - the PangoLayout used for the desired text
         * path - the cairo path to store the base line curve 
         * width - the width of the surface
         * height - the height of the surface
         * num_points - the number of points that form the path.
         *              This number determines how many curves are generated.
         * c_min - the min range value for squared variable in the first cubic
         *        curving equation
         * c_max - the max range value for squared variable in the first cubic
         *        curving equation
         * d_min - the min range value for cubed variable in the first cubic
         *        curving equation
         * d_max - the max range value for cubed variable in the first cubic
         * stretch_deg - the horizontal stretch degree
         * y_var_min_ratio - the minimum fluctuation of the fixing points of
         *                   the curve in y-direction w.r.t. the height of image
         * y_var_max_ratio - the maximum fluctuation of the fixing points of
         *                   the curve in y-direction w.r.t. the height of image
         */
        void
        create_curved_text(cairo_t *cr,PangoLayout *layout, cairo_path_t *&path,
                    double width, double height, int num_points, double c_min,
                    double c_max, double d_min, double d_max,double stretch_deg,
                    double y_var_min_ratio, double y_var_max_ratio);


  
        /*
         * Creates a curved text whose shape will be deformed according to
         * the curvature
         *
         * cr - cairo context
         * layout - the PangoLayout used for the desired text
         * path - the cairo path to store the base line curve 
         * width - the width of the surface
         * height - the height of the surface
         * num_points - the number of points that form the path.
         *              This number determines how many curves are generated.
         * c_min - the min range value for squared variable in the first cubic
         *        curving equation
         * c_max - the max range value for squared variable in the first cubic
         *        curving equation
         * d_min - the min range value for cubed variable in the first cubic
         *        curving equation
         * d_max - the max range value for cubed variable in the first cubic
         * stretch_deg - the horizontal stretch degree
         * y_var_min_ratio - The minimum fluctuation of the fixing points of 
         *                   the curve in y-direction w.r.t. the height of image
         * y_var_max_ratio - The maximum fluctuation of the fixing points of
         *                   the curve in y-direction w.r.t. the height of image
         */
        void create_curved_text_deformed(cairo_t *cr, PangoLayout *layout,
                cairo_path_t *&path, double width, double height,
                int num_points, double c_min, double c_max, double d_min,
                double d_max, double stretch_deg, 
                double y_var_min_ratio, double y_var_max_ratio);

        /*
         * Get the extents of a text 'ink'
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
            getTextExtents(PangoLayout *layout, PangoFontDescription *desc,
                           int &x, int &y, int &w, int &h, int &size);

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
            generateTextPatch(cairo_surface_t *&text_surface,
                              std::string caption,int height,int &width,
                              int text_color, bool distract);


public:// --------------------- PUBLIC METHODS -------------------------------

        /* The base helper */
        MTS_BaseHelper* helper;

        MTSConfig* config;

        /* Constructor */
        MTS_TextHelper(shared_ptr<MTS_BaseHelper> h, shared_ptr<MTSConfig> c);

        /* Destructor */
        ~MTS_TextHelper();

        
        /*
         * Provides the randomly rendered text 
         *
         * caption - the string which will be rendered. 
         * text_surface - an out variable containing a 32FC3 matrix with the 
         *                rendered text including border and shadow.
         * height - height of the surface
         * width - width of the surface that will be determined
         * text_color - the grayscale color value for the text
         * distract - flag that dictates whether distractor text will be present
         */
        void 
            generateTextSample(std::string &caption,
                               cairo_surface_t *&text_surface, int height,
                               int &width, int text_color, bool distract);

};

#endif
