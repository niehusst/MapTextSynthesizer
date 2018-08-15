#ifndef MTS_BACKGROUND_HELPER_HPP
#define MTS_BACKGROUND_HELPER_HPP

#include <vector>

#include <pango/pangocairo.h>

#include "mts_basehelper.hpp"
#include "mts_config.hpp"

using std::string;
using std::vector;
using std::shared_ptr;

using boost::random::normal_distribution;
using boost::random::gamma_distribution;
using boost::random::beta_distribution;
using boost::random::variate_generator;

/*
 * A class to handle the synthetic generation of all background features
 */
class MTS_BackgroundHelper {

    private://---------------------- PRIVATE METHODS --------------------------

        /* Generator for variance in bg bias */
        gamma_distribution<> bias_var_dist;
        variate_generator<mt19937, gamma_distribution<> > bias_var_gen;

        /* Generator for texture width*/
        beta_distribution<> texture_distribution;
        variate_generator<mt19937, beta_distribution<> > texture_distrib_gen;

  
        /*
         * Makes a thicker line behind the original that is a different 
         * gray-scale hue
         *
         * cr - cairo context
         * linewidth - the width of the original line (used in scaling for line)
         * og_col - the color of the original line
         */
        void
            draw_boundary(cairo_t *cr, double linewidth, double og_col);

  
        /*
         * Draws the main line (thin) and then another line (thick) with a 
         * specific dash pattern over it such that it looks as if the 2nd line
         * is actually small perpendicular lines.
         *
         * cr - cairo context
         * linewidth - the width of the original line (used in scaling for line)
         */
        void
            draw_hatched(cairo_t *cr, double linewidth);

  
        /*
         * Sets an arbitrary dash pattern to the path stored by cr
         *
         * cr - cairo context
         */
        void
            set_dash_pattern(cairo_t *cr);


        /*
         * Generates a wiggly line path (but doesn't stroke it to surface)
         *
         * cr - cairo context
         * width - surface width
         * height - surface height
         * c_min - the min range value for squared variable in the first cubic 
         *         curving equation
         * c_max - the max range value for squared variable in the first cubic 
         *         curving equation
         * d_min - the min range value for cubed variable in the first cubic 
         *         curving equation
         * d_max - the max range value for cubed variable in the first cubic 
         *         curving equation
         * river - whether the curve is for a river line or not (optional)
         */
        void
            generate_curve(cairo_t *cr, int width, int height, double c_min,
                           double c_max, double d_min, double d_max,
                           bool river=false);


        /*
         * Sets path orientation through rotation and translation
         *
         * cr - cairo context
         * curved - informs whether the line will be curved (false = straight)
         * length - length of the curve
         * width - surface width
         * height - surface height
         */
        void
            orient_path(cairo_t *cr, bool curved, int length,
                        int width, int height);


        /*
         * Draws a simple texture of parallel lines angled from lower left to
         * upper right using recursion. 
         *
         * cr - cairo context
         * x1 - x position of the point that line is drawn from
         * y1 - y position of the point that line is drawn from
         * x2 - x position of the point that line is drawn to
         * y2 - y position of the point that line is drawn to
         * spacing - the spacing between lines (must be greater than 1)
         * width - surface width in pixels
         * height - surface height in pixels
         */
        static void
            diagonal_lines(cairo_t *cr, int x1, int y1, int x2, int y2,
                           int spacing, int width, int height);

        /*
         * Draws a perpendicular, crossed lines texture using recursion.
         *
         * cr - cairo context
         * x1 - x position of the point that line is drawn from
         * y1 - y position of the point that line is drawn from
         * x2 - x position of the point that line is drawn to
         * y2 - y position of the point that line is drawn to
         * spacing - the spacing between lines (must be greater than 1)
         * width - surface width in pixels
         * height - surface height in pixels
         */
        static void
            crossed_lines(cairo_t *cr, int x1, int y1, int x2, int y2,
                          int spacing, int width, int height);


        /*
         * Calculates and returns the edge-length of a shape with
         * num_sides edges, and a width of diameter
         *
         * num_sides - the number of sides of the shape
         * radius - the width of the shape
         */
        static double
            get_edge_len(int num_sides, int radius);


        /*
         * Draws a single shape by recursively drawing edges
         *
         * cr - cairo context
         * x - x start coordinate
         * y - y start coordinate
         * angle - the angle in radians from 0 to the next edge
         * edge_len - the length to draw for each edge of the shape
         * num_sides - the number of sides to draw 
         * counter - a counter to keep track of how many sides have been drawn
         */
        static void
            shape_helper(cairo_t *cr, double x, double y, double angle,
                         double edge_len, int num_sides, int counter);


        /*
         * Draws a shape of specified sides and diameter starting from x,y
         *
         * cr - cairo context
         * x - x start coordinate
         * y - y start coordinate
         * num_sides - the number of sides to draw on a shape. 
         *             (if num_sides < 2, draws a circle)
         * radius - half the width of the shape
         */
        static void 
            draw_shape(cairo_t *cr, int x, int y, int num_sides, int radius);


        /*
         * A recursive helper function for make_shape_texture
         *
         * cr - cairo context
         * origin_x - x coordinate for shape
         * origin_y - y coordinate for shape
         * num_sides - the number of sides to draw on a shape. 
         *             (if num_sides < 2, draws a circle)
         * distance - the distance to put between shapes
         * even - helps the recursive function stagger the rows
         * radius - half the width of the shape
         * width - width of the surface
         * height - height of the surface
         */
        static void
            shape_texture_helper(cairo_t *cr, int origin_x, int origin_y,
                                 int num_sides, int distance, bool even,
                                 int radius, int width, int height);


        /*
         * Draws a texture of repeated shapes, each row is staggered a little
         * 
         * cr - cairo context
         * x - x position of top of shape 
         * y - y position of top of shape 
         * diameter - width of each shape
         * num_sides - number of sides of the shape (must be positive)
         * spacing - space between shapes
         * width - surface width in pixels
         * height - surface height in pixels
         */
        static void
            make_shape_texture(cairo_t *cr, int x, int y, int diameter,
                               int num_sides,int spacing,int width,int height);


        /*
         * Selects a texture function based on the input index and draws it
         * onto the surface stored in cr.
         *
         * cr - cairo context
         * texture - the index choice for the background texture
         *           (must be between 0 and 2 inclusive)
         *           ( 0 - diagonal lines )
         *           ( 1 - crossed lines  )
         *           ( 2 - shapes         )
         * brightness - the grayscale brightness level to make the texture
         * linewidth - width of the lines drawn (unused for shape texture)
         * diameter - the diameter of each shape 
         *            (if texture != 2, then this parameter is set to 0)
         * num_sides - the number of sides of each shape 
         *             (if texture != 2, then this parameter is set to 0)
         * spacing - the spacing between lines or dots in the texture
         * width - surface width in pixels
         * height - surface height in pixels
         */
        void
            draw_texture(cairo_t *cr, int texture, double brightness,
                         double linewidth, int diameter, int num_sides,
                         int spacing, int width, int height);


        /*
         * Sets the source of cr to be a texture that is selected by the texture
         * parameter.
         *
         * cr - cairo context
         * texture - the index choice for the background texture 
         *           (must be between 0 and 2 inclusive)
         *           ( 0 - diagonal lines )
         *           ( 1 - crossed lines  )
         *           ( 2 - shapes         )
         * brightness - the grayscale brightness level to make the texture
         * linewidth - width of the lines drawn (unused if texture is shapes)
         * spacing - the spacing between lines or dots in the texture 
         *           (if texture == 2, spacing must >= to diameter) 
         * width - surface width in pixels
         * height - surface height in pixels
         */
        void
            set_texture_source(cairo_t *cr, int texture, double brightness,
                          double linewidth, int spacing, int width, int height);


        /*
         * Draws num_lines thick swaths of texture onto the surface
         *
         * cr - cairo context
         * curved - describes if line will be curved (false = not curved)
         * brightness - the grayscale brightness level of the texture
         * width - surface width
         * height - surface height
         * c_min - the min range value for squared variable in the first cubic 
         *        curving equation
         * c_max - the max range value for squared variable in the first cubic 
         *        curving equation
         * d_min - the min range value for cubed variable in the first cubic 
         *        curving equation
         * d_max - the max range value for cubed variable in the first cubic 
         *        curving equation
         */
        void
            addTexture(cairo_t *cr, bool curved,  double brightness, 
                    int width, int height, double c_min, double c_max,
                       double d_min, double d_max);


        /* 
         * Draws a line with random placement that has the characteristics
         * specified by the boolean parameters
         *
         * cr - cairo context
         * boundary - if true, add a colored line that runs next to the original
         * hatched - if true, add short perpendicular lines through the original
         * dashed - if true, make line dashed with arbitrary pattern
         * curved - if true, add curvature with create_curved_path from pc
         * doubleline - if true, add another line parallel next to the original
         * river - if true, the line is a river
         * width - the width of the layout in pixels
         * height - the height of the layout in pixels
         * c_min - the min range value for squared variable in the first cubic 
         *        curving equation (optional)
         * c_max - the max range value for squared variable in the first cubic 
         *        curving equation (optional) 
         * d_min - the min range value for cubed variable in the first cubic 
         *        curving equation (optional)
         * d_max - the max range value for cubed variable in the first cubic 
         *        curving equation (optional)
         * color - color of the line to be drawn 
         *         (already set, should only be used for boundary)
         */
        void
            addLines(cairo_t *cr, bool boundary, bool hatched, bool dashed, 
                    bool curved, bool doubleline, bool river, int width, 
                    int height, double c_min=0, double c_max=0, 
                    double d_min=0, double d_max=0, double color=0);


        /*
         * Makes the background variably colored to simulate
         * stained or worn map paper
         *
         * cr - cairo context
         * width - width of canvas
         * height - height of canvas
         * color - the original bg color
         */
        void
            addBgBias(cairo_t *cr, int width, int height, int color);

        /* 
         * Add bg patters like even-spaced straight line, uneven-spaced
         * straight line, grid, etc.
         *
         * cr - cairo context
         * width - width of the canvas
         * height - height of the canvas
         * even - whether the lines are evenly spaced or not
         * grid - whether the pattern is a grid or not
         * curved - whether the lines are curved or not
         */
        void
            addBgPattern(cairo_t *cr, int width, int height, 
                    bool even, bool grid, bool curved);

        /*
         * Makes differently colored areas in the background (at most 2)
         *
         * cr - cairo context
         * width - width of the canvas
         * height - height of the canvas
         * color_min - darkest color
         * color_max - brightest color
         */
        void
            colorDiff(cairo_t *cr, int width, int height, double color_min,
                      double color_max); 


        /*
         * Draws a circle along an arbitrary edge of the surface
         * 
         * cr - cairo context
         * width - surface width
         * height - surface height
         * hollow - whether the point is hollow or not
         */
        void
            cityPoint(cairo_t *cr, int width, int height, bool hollow);


    public://------------------------ PUBLIC METHODS ---------------------------

        // a helper class memeber that holds important functions
        MTS_BaseHelper* helper;

        /* An MTSConfig instance to get parameters from. */
        MTSConfig* config;

        //Constructor
        MTS_BackgroundHelper(shared_ptr<MTS_BaseHelper> h,
                             shared_ptr<MTSConfig> c);

        //Destructor
        ~MTS_BackgroundHelper();

        /*
         * Generate bg features that will be drawn on current image
         * basing on the probabilities the user gives
         *
         * bg_features - the ouput array of features
         */
        void
            generateBgFeatures(vector<BGFeature> &bg_features);

        /*
         * Generates a map-like background 
         *
         * bg_surface - the image surface
         * features - the list of features to be added
         * width - surface width in pixels
         * bg_color - the grayscale color value for the background
         * contrast - the contrast level
         */
        void
            generateBgSample(cairo_surface_t *&bg_surface,
                             vector<BGFeature>&features, int height,
                             int width, int bg_color, int contrast);
};

#endif
